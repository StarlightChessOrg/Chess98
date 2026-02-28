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
        0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, R_KNIGHT, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, R_KING, 0, 0, 0, 0
    };
    position_init(test_matrix, R);
    print_board(g_board);
    print_vector<POS>(pos_list_r_);
    print_vector<POS>(pos_list_b_);

    position_move(Move(4, 5));
    print_board(g_board);
    print_vector<POS>(pos_list_r_);
    print_vector<POS>(pos_list_b_);

    position_undo();
    print_board(g_board);
    print_vector<POS>(pos_list_r_);
    print_vector<POS>(pos_list_b_);
    return 0;
}
