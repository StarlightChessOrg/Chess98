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
    Timer t {};
    int k = 0;
    int i = 0;
    // for (; t.duration() < 1000; i++) {
    //     MovePicker mp {0};
    //     for (Move m = mp.next(); m; m = mp.next()) {
    //         k += m.end;    
    //     }
    // }
    // std::cout << t.duration() << "ms, " << i << " iterations, " << k << std::endl;
    system("pause");
    return 0;
}
