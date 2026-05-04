#include "test.hpp"
#include "ucci.hpp"

int main()
{
    auto fen = "rnakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RNAKABNR w";
    const MATRIX matrix = utils_fen_to_matrix(fen);
    position_init(matrix, R);
    history_init();
    killer_init();
    tt_init();
    // test code
    Timer t {};
    int k = 0;
    int i = 0;
    for (; t.duration() < 1000; i++) {
        MovePicker mp {0};
        for (Move m = mp.next(); m; m = mp.next()) {
            k += m.end;    
        }
        
    }
    std::cout << t.duration() << "ms, " << i << " iterations, " << k << std::endl;
    
    return 0;
}
