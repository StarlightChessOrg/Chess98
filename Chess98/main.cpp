#include "test.hpp"
#include "genfiles.hpp"

int main()
{
    NNUE model = NNUE("./nnue.pt");
    auto res = model.evaluate(fenToPieceidMap("rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RNBAKABNR w - - 0 1"), RED);
    std::cout << res << std::endl;
#ifndef GENFILES
    testByUI();
#else
    genfiles();
#endif

    return 0;
}
