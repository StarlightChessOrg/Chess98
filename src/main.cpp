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
        B_KNIGHT, 0, R_CANNON, 0, 0, 0, 0, R_CANNON, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, B_PAWN, 0, 0, R_KING, 0, 0, 0, 0
    };
    position_init(test_matrix, R);
    // test code
    constexpr POS R_ROOK_POS = 65;
    std::vector<Move> moves1 = gen_rook_legacy_<true>(R_ROOK_POS);
    std::vector<Move> moves2 = gen_rook_bit_<true>(R_ROOK_POS);
    std::vector<Move> moves1_no_capture = gen_rook_legacy_<false>(R_ROOK_POS);
    std::vector<Move> moves2_no_capture = gen_rook_bit_<false>(R_ROOK_POS);
    // preformance test
    Timer t1 {};
    for (int i = 0; i < 100000000; i++) {
        gen_cannon_legacy_<true>(R_ROOK_POS);
    }
    std::cout << t1.duration() << "ms\n";
    Timer t2 {};
    for (int i = 0; i < 100000000; i++) {
        gen_cannon_bit_<true>(R_ROOK_POS);
    }
    std::cout << t2.duration() << "ms\n";
    return 0;
}
