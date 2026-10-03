#include "test.hpp"
#include "ucci.hpp"

int main()
{
    // DO_NOT_REMOVE: THIS IS INITIALIZATION
    position_init(fen_to_matrix(START), R);
    tt_init(12);
    history_init();
    killer_init();
    // END DO_NOT_REMOVE
    ucci_loop();
    return 0;
}
