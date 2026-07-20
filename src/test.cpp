#include "test.hpp"
#include "search.hpp"

void move_validation()
{
    position_init(INIT_MATRIX, R);
    const std::vector<Move> quiet = gen_all_quiet_moves();
    const std::vector<Move> capture = gen_all_capture_moves();
    const bool result = quiet.size() + capture.size() == 44;
    std::cout << "Validation: " << (result ? "OK" : "Error") << std::endl;
}

void movepicker_1sec_count()
{
    position_init(INIT_MATRIX, R);
    Timer t { 1000 };
    uint64_t iterations = 0, id = 0, num = 0;
    while (!t.time_up()) {
        MovePicker a { 1 };
        for (Move m = a.next(); m; m = a.next()) {
            id += m.beg;
            id += m.end;
            num++;
        }
        iterations++;
    }
    std::cout << "[MovePicker Generation 1 second]";
    std::cout << " iterations: " << iterations;
    std::cout << " num: " << num;
    std::cout << " id: " << id;
    std::cout << std::endl;
}

void movegen_1sec_count()
{
    position_init(INIT_MATRIX, R);
    Timer t { 1000 };
    uint64_t iterations = 0, id = 0, num = 0;
    while (!t.time_up()) {
        for (const Move m : gen_all_quiet_moves()) {
            id += m.beg;
            id += m.end;
            num++;
        }
        for (const Move m : gen_all_capture_moves()) {
            id += m.beg;
            id += m.end;
            num++;
        }
        iterations++;
    }
    std::cout << "[MoveGen Generation 1 second]";
    std::cout << " iterations: " << iterations;
    std::cout << " num: " << num;
    std::cout << " id: " << id;
    std::cout << std::endl;
}
