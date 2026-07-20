#include "main.hpp"
#include "test.hpp"
#include "ucci.hpp"

int main(int argc, char* argv[])
{
    set_process_high_priority();
    tt_init();
    killer_init();
    history_init();
    // test code
    move_validation();
    search_validation();
    system("pause");
    return 0;
}
