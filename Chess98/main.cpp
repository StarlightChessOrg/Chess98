#include "test.hpp"

int main()
{
#ifndef GENFILES
    testByUI();
#else
    while (true)
    {
        testGenerateGENFILES();
    }
#endif

    return 0;
}
