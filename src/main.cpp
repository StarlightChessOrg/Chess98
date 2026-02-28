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
        0, R_ROOK, B_KNIGHT, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, R_KING, 0, 0, 0, 0
    };

    position_init(test_matrix, R);
    utils_print_matrix<PTYPE>(g_board);
    utils_print_matrix<PID>(pos_pid_r_);
    utils_print_matrix<PID>(pos_pid_b_);
    utils_print_vector<POS>(pos_list_r_);
    utils_print_vector<POS>(pos_list_b_);

    position_move(Move(64, 65));
    utils_print_matrix<PTYPE>(g_board);
    utils_print_matrix<PID>(pos_pid_r_);
    utils_print_matrix<PID>(pos_pid_b_);
    utils_print_vector<POS>(pos_list_r_);
    utils_print_vector<POS>(pos_list_b_);

    position_undo();
    utils_print_matrix<PTYPE>(g_board);
    utils_print_matrix<PID>(pos_pid_r_);
    utils_print_matrix<PID>(pos_pid_b_);
    utils_print_vector<POS>(pos_list_r_);
    utils_print_vector<POS>(pos_list_b_);

    return 0;
}
