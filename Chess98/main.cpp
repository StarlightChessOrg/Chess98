// #include "ucci.hpp"
#include "base.hpp"

int main()
{
    FEN fen = "rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RNBAKABNR w";
    // TEAM team = R;
    // int max_time = 3000;
    // int max_depth = 20;

    std::cout << fen::to_fen(fen::to_matrix(fen)) << std::endl;

    // UCCI ucci(fen, team, max_time, max_depth);
    // ucci.cli();

    return 0;
}
