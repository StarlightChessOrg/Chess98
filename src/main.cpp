#include "test.hpp"
#include "ucci.hpp"

constexpr MATRIX test_matrix {
    0, 0, 0, 0, B_KING, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    R_ROOK, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0,
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
    MovePicker mp(0);
    int i = 0;
    for (Move m = mp.next(); m; m = mp.next()) {
        i++;
        std::cout << i << ": "
                  << (int)m.beg << "->"
                  << (int)m.end << std::endl;
    }
    return 0;
}
