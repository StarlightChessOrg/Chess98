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
    print_pregen_table_cannon(CANNON_TARGETS, 4);
    return 0;
}
