#pragma once
#include <torch/script.h>
#include <torch/torch.h>
#include "base.hpp"

using MODULE = torch::jit::script::Module;

class NNUE {
public:
    NNUE(std::string model_path)
    {
        std::ifstream fin("./nnue.pt", std::ios::binary);
        assert(fin != nullptr);
        this->model = torch::jit::load(fin);
    }

protected:
    MODULE model;
};