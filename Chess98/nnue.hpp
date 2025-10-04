#pragma once
#include <torch/script.h>
#include <torch/torch.h>
#include "base.hpp"

using MODEL = torch::jit::script::Module;
using TENSOR = torch::Tensor;

class NNUE {
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
            torch::Tensor test_output = model.forward({ test_input }).toTensor();

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
        catch (const c10::Error& e)
        {
            std::cerr << "Error during model evaluation: " << e.what() << std::endl;
            return 0.0f;
        }
    }

protected:
    // 获取输入数据，一个展平的棋盘向量
    TENSOR nnueInput(const PIECEID_MAP& pieceidMap, TEAM team) {
        // 1. 创建 7×9×10 的三维张量并填充数据
        TENSOR input = torch::zeros({ 7, 9, 10 }, torch::kFloat32);
        for (int x = 0; x < 9; x++) {
            for (int y = 0; y < 10; y++) {
                PIECEID pieceId = pieceidMap[x][y];
                if (pieceId != EMPTY_PIECEID) {
                    int pieceType = abs(pieceId) - 1;  // 转换为0-6的索引
                    float value = (pieceId > 0) ? 1.0f : -1.0f;
                    input[pieceType][x][y] = value;
                }
            }
        }

        // 2. 展平为一维张量 [630]，然后添加批次维度 [1, 630]
        return input.flatten().unsqueeze(0);  // 或等价写法：.reshape({1, 630})
    }
};
