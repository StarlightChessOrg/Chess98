#include "test.hpp"
#include "ucci.hpp"

constexpr MATRIX test_matrix {
    0, 0, 0, 0, B_KING, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    R_CANNON, 0, 0, 0, 0, 0, 0, 0, 0,
    R_ROOK, B_PAWN, B_CANNON, R_CANNON, B_PAWN, 0, 0, 0, 0,
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
    for (int i = 0; i < 100; i++) {
        const Move m = mp.next();
        if (m) {
            std::cout << "Move: " << (int)m.beg << " -> " << (int)m.end << "\n";
        } else {
            std::cout << "No more moves\n";
            break;
        }
    }
    return 0;
}
