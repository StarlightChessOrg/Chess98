#include "ucci.hpp"
#include "test.hpp"

int main()
{
    // INITIALIZE DO NOT REMOVE
    position_init(fen_to_matrix("rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RNBAKABNR w"), R);
    tt_init();
    history_init();
    killer_init();
    // END
    ucci_loop();
    return 0;
}
