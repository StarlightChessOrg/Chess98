#include "ucci.hpp"
#include "test.hpp"

int main()
{
    // DO_NOT_REMOVE: THIS IS INITIALIZATION
    position_init(fen_to_matrix(START), R);
    tt_init(12);
    history_init();
    killer_init();
    // END DO_NOT_REMOVE
    move_preformance_test();
    minmax_example();
    ucci_loop();
    return 0;
}
