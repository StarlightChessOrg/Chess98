#pragma once

// #define NNUE

#ifdef NNUE

#include <torch/script.h>
#include <torch/torch.h>
#include "base.hpp"

using MODEL = torch::jit::script::Module;
using TENSOR = torch::Tensor;

class NNUE
{
public:
    NNUE(std::string model_path)
    {
        std::ifstream fin("./nnue.pt", std::ios::binary);
        assert(fin);
        this->model = torch::jit::load(fin);
        this->model.eval();
    }

public:
    MODEL model;

public:
    float evaluate(const PIECEID_MAP& pieceidMap, TEAM team)
    {
        try
        {
            // 1. 生成随机输入张量 [1, 630] (与Python的torch.randn(1, 630)等价)
            torch::Tensor test_input = nnueInput(pieceidMap, team);

            // 2. 运行模型推理
            torch::NoGradGuard no_grad;
            torch::jit::Stack input{test_input};
            torch::Tensor test_output = model.forward(input).toTensor();

            // 3. 提取并打印输出结果 (与Python的test_output[0].tolist()等价)
            auto output_accessor = test_output.accessor<float, 2>(); // [1, 2]的二维访问器
            float first_val = output_accessor[0][0];
            float second_val = output_accessor[0][1];

            // 4. 格式化输出
            std::cout << "C++ output: [" << first_val << ", " << second_val << "]" << std::endl;

            // 可选：打印更多调试信息
            std::cout << "Input shape: " << test_input.sizes() << std::endl;
            std::cout << "Output shape: " << test_output.sizes() << std::endl;
        }
        catch (const std::exception e)
        {
            std::cerr << "Error during model evaluation: " << e.what() << std::endl;
            return 0.0f;
        }
    }

protected:
    // 获取输入数据，一个展平的棋盘向量
    TENSOR nnueInput(const PIECEID_MAP& pieceidMap, TEAM team)
    {
        // 1. 创建 7×9×10 的三维张量并填充数据
        TENSOR input = torch::zeros({7, 9, 10}, torch::kFloat32);
        for (int x = 0; x < 9; x++)
        {
            for (int y = 0; y < 10; y++)
            {
                PIECEID pieceId = pieceidMap[x][y];
                if (pieceId != EMPTY_PIECEID)
                {
                    int pieceType = abs(pieceId) - 1; // 转换为0-6的索引
                    float value = (pieceId > 0) ? 1.0f : -1.0f;
                    input[pieceType][x][y] = value;
                }
            }
        }

        // 2. 展平为一维张量 [630]，然后添加批次维度 [1, 630]
        return input.reshape({1, 630}); // 或等价写法：.reshape({1, 630})
    }
};

void testNNUE()
{
    NNUE model = NNUE("./nnue.pt");
    std::vector<std::vector<std::string>> fens = {
        {"rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C2C4/9/RNBAKABNR b - - 0 1", "炮二平五"},
        {"rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/4C2C1/9/RNBAKABNR w - - 0 1", "炮八平五"},
        {"rnbakab1r/9/1c5cn/p1p1p1p1p/9/9/P1P1P1P1P/1C2C4/9/RNBAKABNR w - - 0 1", "炮二平五->马8进9"},
        {"rnbakab1r/9/1c4nc1/p1p1p1p1p/9/9/P1P1P1P1P/1C2C4/9/RNBAKABNR w - - 0 1", "炮二平五->马8进7"},
        {"rnbakab1r/9/1c5cn/p1p1C1p1p/9/9/P1P1P1P1P/1C7/9/RNBAKABNR b - - 0 1", "炮二平五->马8进9->炮五进四"},
        {"rCbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/7C1/9/RNBAKABNR b - - 0 1", "炮八进七"},
        {"1rbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/7C1/9/RNBAKABNR w - - 0 1", "炮八进七->车2平1"},
        {"rnbakabnr/1N1R1R3/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/2BAKABN1 w - - 0 1",
         "红方双车卡黑方九宫肋道，辅以红马叫杀"},
        {"2bakabn1/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/1n1r1r3/RNBAKABNR b - - 0 1",
         "黑方双车卡红方九宫肋道，辅以黑马叫杀"},
        {"1Cbak4/9/3a5/p1p1c1p1p/5r3/2P1P4/P7P/9/4A4/2BK1Ar2 w - - 0 1", "红方沉底炮，黑方双车带炮占大优"},
        {"2bak4/9/3a5/p1p1c1p1p/5r3/2P1P4/P7P/9/4A4/2BK1Ar2 w - - 0 1", "去掉红方沉底炮，黑方双车带炮占大优"},
        {"r1bakabnr/9/1cn4c1/p1p1p1p1p/9/2P6/P3P1P1P/1C5C1/9/RNBAKABNR w - - 0 1", "兵七进一->马2进3"},
        {"rnbakab1r/9/1c4nc1/p1p1p1p1p/9/2P6/P3P1P1P/1C5C1/9/RNBAKABNR w - - 0 1", "兵七进一->马8进7"},
        {"rnbakabnr/9/1c5c1/p1p1p3p/6p2/2P6/P3P1P1P/1C5C1/9/RNBAKABNR w - - 0 1", "兵七进一->兵7进1"},
        {"rnbakabnr/9/1c7/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/1NBAKABN1 w - - 0 1", "开局红方让双车，黑方让单炮"},
        {"3k5/9/9/9/9/9/9/4K4/4A4/4C4 w - - 0 1", "残局，红帅升天居中，帅后店士，士后垫炮，除黑将外再无任何子力"},
        {"3k5/9/9/9/9/9/9/5K3/4A4/4C4 w - - 0 1", "残局，红帅升天居右，帅后店士，士后垫炮，除黑将外再无任何子力"},
        {"3k5/9/9/9/9/9/9/9/4A4/4CK3 w - - 0 1", "残局，红帅居起始位置右侧，帅后店士，士后垫炮，除黑将外再无任何子力"},
        {"rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RNBAKABNR w - - 0 1", "开局尚未动任何一子"},
        {"rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/4K4/RNBA1ABNR b - - 0 1", "帅五进一"},
        {"rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RNBA1ABNR w - - 0 1", "红缺帅"},
        {"rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/BC5C1/9/RN1AKABNR b - - 0 1", "相七进九"},
        {"rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/BC5C1/9/RN1AKABNR w - - 0 1", "相七进九"},
    };
    for (int i = 0; i < fens.size(); i++)
    {
        auto res = model.evaluate(fenToPieceidmap(fens[i][0]), fens[i][0].find('w') ? RED : BLACK);
        std::cout << fens[i][1] << ": " << res << std::endl;
    }
}

#endif
