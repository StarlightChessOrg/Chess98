#pragma once
#include <torch/script.h>
#include <torch/torch.h>
#include "base.hpp"

using MODULE = torch::jit::script::Module;

class NNUE {
public:
    NNUE(std::string model_path)
    {
        // 从stream加载模型
        std::ifstream fin("./nnue.ptd", std::ios::binary);
        this->model = torch::jit::load(fin);
    }

protected:
    MODULE model;
};