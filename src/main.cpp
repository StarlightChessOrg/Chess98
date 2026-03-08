#include "ucci.hpp"
#include "test.hpp"

int main()
{
    const MATRIX test_matrix {
        0, 0, 0, 0, B_KING, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, B_CANNON, 0, 0, 0, 0, 0, 0, 0,
        0, R_ROOK, B_KNIGHT, 0, 0, 0, 0, 0, 0,
        0, B_CANNON, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, R_KING, 0, 0, 0, 0
    };

    position_init(test_matrix, R);
    position_move(Move(85, 86));
    auto a = gen_rook_<CAPTURE>(73);

    return 0;
}
