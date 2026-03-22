#include "test.hpp"
#include "ucci.hpp"

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
        0, R_ROOK, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, R_KING, 0, 0, 0, 0
    };
    position_init(test_matrix, R);
    print_pregen_table_rook(ROOK_TARGETS, 3);
    Timer t1 {};
    std::vector<Move> m1 {};
    for (int i = 0; i < 100000000; i++)
        m1 = gen_rook_<false>(64);
    std::cout << t1.duration() << "  ";
    Timer t2 {};
    std::vector<Move> m2 {};
    for (int i = 0; i < 100000000; i++)
        m2 = gen_rook_bit_<false>(64);
    std::cout << t2.duration();
    utils_print_vector<Move>(m1);
    utils_print_vector<Move>(m2);
    return 0;
}
