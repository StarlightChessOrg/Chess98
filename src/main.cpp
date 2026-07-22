#include "test.hpp"
#include "ucci.hpp"

int main()
{
    position_init(INIT_MATRIX, R);
    history_init();
    killer_init();
    tt_init();
    // test code
    move_preformance_test();
    system("pause");
    return 0;
}
