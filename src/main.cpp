#include "test.hpp"
#include "ucci.hpp"


int main()
{
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
    movegen_1sec_count();
    system("pause");
    return 0;
}
