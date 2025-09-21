#include "test.hpp"

int main()
{
    auto model = loadModel();
#ifndef GENFILES
    std::cout << "You're runing in UI mode. And you can run nnue filegen by adding 'define GENFILES' in nnuefile.hpp!\n"
              << std::endl;
    testByUI();
#else
    std::cout << "You're running in GENFILES file generate mode. And it could be disabled by remove 'define GENFILES' in nnuefile.hpp!\n"
              << std::endl;
    while (true)
    {
        testGenerateGENFILES();
    }
#endif

    return 0;
}
