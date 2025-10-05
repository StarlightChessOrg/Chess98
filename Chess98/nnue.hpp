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
    float evaluate(const PIECEID_MAP& pieceidMap, TEAM team) {
        try {
            // 1. 生成输入张量
            TENSOR input = nnueInput(pieceidMap, team);

            // 2. 模型推理
            torch::NoGradGuard no_grad;
            TENSOR output = model.forward({ input }).toTensor();

            // 3. 处理输出
            auto output_accessor = output.accessor<float, 2>();
            float score = (team == RED) ? output_accessor[0][1] : output_accessor[0][0];
            score *= 1000;  // 与Python端缩放一致

            // 打印最终评估值

            return score;

        }
        catch (const std::exception& e) {
            std::cerr << "[ERROR] Evaluation failed: " << e.what() << std::endl;
            std::cerr << "  Exception type: " << typeid(e).name() << std::endl;
            return 0.0f;
        }
        catch (...) {
            std::cerr << "[ERROR] Unknown exception during evaluation!" << std::endl;
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
        return input.flatten().reshape({ 1, 630 });
    }
};
