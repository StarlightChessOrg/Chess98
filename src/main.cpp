#include "test.hpp"
#include "ucci.hpp"

int main()
{
    const MATRIX test_matrix {
        0, 0, 0, 0, B_KING, 0, 0, 0, 0,
        0, 0, B_CANNON, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, R_CANNON, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, R_KING, 0, 0, 0, 0
    };
    position_init(test_matrix, R);
    bool a = false;
    // test code
    for (int i = 0; i < 25 * 10000000; i++) {
        a=in_check();
    }
    return 0;
}
