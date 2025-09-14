#include <torch/script.h>
#include "test.hpp"

int main()
{
#ifndef NNUE
    std::cout << "You're runing in UI mode. And you can run nnue filegen by adding 'define NNUE' in nnuefile.hpp!\n"
              << std::endl;
    testByUI();
#else
    std::cout << "You're running in NNUE file generate mode. And it could be disabled by remove 'define NNUE' in nnuefile.hpp!\n"
              << std::endl;
    while (true)
    {
        testGenerateNNUE();
    }
#endif

    return 0;
}
