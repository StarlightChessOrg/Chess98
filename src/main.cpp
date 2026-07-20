#include "main.hpp"
#include "test.hpp"
#include "ucci.hpp"

int main(int argc, char* argv[])
{
    set_process_high_priority();
    // test code
    movepicker_1sec_count();
    movegen_1sec_count();
    system("pause");
    return 0;
}
