#include "test.hpp"
#include "ucci.hpp"

constexpr MATRIX INIT_MATRIX {
    B_ROOK, B_KNIGHT, B_BISHOP, B_ADVISOR, B_KING,
    B_ADVISOR, B_BISHOP, B_KNIGHT, B_ROOK,
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, B_CANNON, 0, 0, 0, 0, 0, B_CANNON, 0,
    B_PAWN, 0, B_PAWN, 0, B_PAWN, 0, B_PAWN, 0, B_PAWN,
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    R_PAWN, 0, R_PAWN, 0, R_PAWN, 0, R_PAWN, 0, R_PAWN,
    0, R_CANNON, 0, 0, 0, 0, 0, R_CANNON, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    R_ROOK, R_KNIGHT, R_BISHOP, R_ADVISOR, R_KING,
    R_ADVISOR, R_BISHOP, R_KNIGHT, R_ROOK
};

constexpr MATRIX TEST_MATRIX {
    0, B_KNIGHT, B_BISHOP, B_ADVISOR, B_KING,
    B_ADVISOR, B_BISHOP, B_KNIGHT, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    B_PAWN, 0, B_PAWN, 0, B_PAWN, 0, B_PAWN, 0, B_PAWN,
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    R_PAWN, 0, R_PAWN, 0, R_PAWN, 0, R_PAWN, 0, R_PAWN,
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    R_ROOK, R_KNIGHT, R_BISHOP, R_ADVISOR, R_KING,
    R_ADVISOR, R_BISHOP, R_KNIGHT, 0
};

int main()
{
    position_init(TEST_MATRIX, R);
    history_init();
    killer_init();
    tt_init();
    // test code
    search_test();
    move_preformance_test();
    system("pause");
    return 0;
}
