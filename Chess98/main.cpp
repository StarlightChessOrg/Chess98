#include "test.hpp"
#include "genfiles.hpp"

int main()
{
    NNUE("./nnue.pt");
#ifndef GENFILES
    testByUI();
#else
    genfiles();
#endif

    return 0;
}
