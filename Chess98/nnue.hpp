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
    }

protected:
    MODEL model;

public:
    int evaluate(const PIECEID_MAP& pieceidMap, TEAM team)
    {
        
    }

protected:
    TENSOR nnueInput(const PIECEID_MAP& pieceidMap, TEAM team)
    {
        TENSOR input = torch::zeros({ 7, 9, 10 }, torch::kFloat32);
        for (int x = 0; x < 9; ++x) {
            for (int y = 0; y < 10; ++y) {
                PIECEID pieceId = pieceidMap[x][y];
                if (pieceId != EMPTY_PIECEID) {
                    // 确定棋子类型（忽略颜色）
                    int pieceType = abs(pieceId) - 1; // 转换为0-6的索引
                    // 确定值：红棋为1，黑棋为-1
                    float value = (pieceId > 0) ? 1.0f : -1.0f;
                    input[pieceType][x][y] = value;
                }
            }
        }
        return input;
    }
};
