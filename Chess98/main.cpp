#include "test.hpp"
#include "genfiles.hpp"

int main()
{
#ifndef GENFILES
    testByUI();
#else
    genfiles();
#endif

    return 0;
}
