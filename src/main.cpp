#include "test.hpp"
#include "ucci.hpp"

constexpr MATRIX test_matrix {
    0, 0, 0, 0, B_KING, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    R_CANNON, 0, 0, 0, 0, 0, 0, 0, 0,
    R_ROOK, B_PAWN, R_KNIGHT, B_CANNON, B_PAWN, 0, 0, 0, 0,
    B_CANNON, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, R_KING, 0, 0, 0, 0, 0
};

int main()
{
    position_init(test_matrix, R);
    history_init();
    killer_init();
    tt_init();
    // test code
    MovePicker mp(0);
    std::cout << see_ge(Move(45, 46), 0);
    return 0;
}
