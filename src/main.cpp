#include "test.hpp"
#include "ucci.hpp"

constexpr MATRIX test_matrix {
    0, 0, 0, 0, B_KING, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    R_CANNON, 0, 0, 0, 0, 0, 0, 0, 0,
    R_ROOK, B_PAWN, B_CANNON, B_ROOK, B_PAWN, 0, 0, 0, 0,
    B_CANNON, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, R_KING, 0, 0, 0, 0, 0
};

void init_everything()
{
    position_init(test_matrix, R);
    history_init();
    killer_init();
    tt_init();
}

int main()
{
    init_everything();
    // test code
    std::vector<Move> a { { 45, 54 }, { 45, 46 }, { 45, 47 }, { 45, 48 } };
    mvvlva(a);
    return 0;
}
