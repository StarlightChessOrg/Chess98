#include "test.hpp"
#include "ucci.hpp"

int main()
{
    const MATRIX matrix {
        B_ROOK, B_KNIGHT, B_BISHOP, B_ADVISOR, B_KING, B_ADVISOR, B_BISHOP, B_KNIGHT, B_ROOK,
        0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, B_CANNON, 0, 0, 0, 0, 0, B_CANNON, 0,
        B_PAWN, 0, B_PAWN, 0, B_PAWN, 0, B_PAWN, 0, B_PAWN,
        0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0,
        R_PAWN, 0, R_PAWN, 0, R_PAWN, 0, R_PAWN, 0, R_PAWN,
        0, R_CANNON, 0, 0, 0, 0, 0, R_CANNON, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0,
        R_ROOK, R_KNIGHT, R_BISHOP, R_ADVISOR, R_KING, R_ADVISOR, R_BISHOP, R_KNIGHT, R_ROOK
    };
    //const MATRIX matrix{
    //    0, B_KNIGHT, 0, 0, B_KING, 0, 0, 0, 0,
    //    0, 0, 0, 0, 0, 0, 0, 0, 0,
    //    0, B_CANNON, 0, 0, 0, 0, 0, 0, 0,
    //    0, 0, 0, 0, 0, 0, 0, 0, 0,
    //    0, 0, 0, 0, 0, 0, 0, 0, 0,
    //    0, 0, 0, 0, 0, 0, 0, 0, 0,
    //    0, 0, 0, 0, 0, 0, 0, 0, 0,
    //    0, R_ROOK, 0, 0, 0, 0, 0, 0, 0,
    //    0, 0, 0, 0, R_ADVISOR, 0, 0, 0, 0,
    //    0, R_KNIGHT, 0, 0, R_KING, 0, 0, 0, 0,
    //};
    position_init(matrix, R);
    history_init();
    killer_init();
    tt_init();
    // test code
    int i = 0;
    for (Move m : gen_all_capture_moves()) {
        i++;
        std::cout << (int)m.beg << " " << (int)m.end << "\n";
    }
    for (Move m : gen_all_quiet_moves()) {
        i++;
        std::cout << (int)m.beg << " " << (int)m.end << "\n";
    }
    std::cout << i;

    return 0;
}
