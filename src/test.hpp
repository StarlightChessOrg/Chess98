#pragma once
#include "search.hpp"

void move_preformance_test()
{
    Timer t { 1000 };
    uint64_t iterations = 0, id = 0, num = 0;
    while (!t.time_up()) {
        MovePicker a { 1 };
        for (Move m = a.next(); m; m = a.next()) id += m.beg + m.end, num++;
        iterations++;
    }
    std::cout << "[MovePicker Generation 1 second]";
    std::cout << " iterations: " << iterations;
    std::cout << " num: " << num;
    std::cout << " id: " << id;
    std::cout << std::endl;
    Timer t2 { 1000 };
    iterations = 0, id = 0, num = 0;
    while (!t2.time_up()) {
        for (const Move m : gen_all_quiet_moves()) id += m.beg + m.end, num++;
        for (const Move m : gen_all_capture_moves()) id += m.beg + m.end, num++;
        iterations++;
    }
    std::cout << "[MoveGen Generation 1 second]";
    std::cout << " iterations: " << iterations;
    std::cout << " num: " << num;
    std::cout << " id: " << id;
    std::cout << std::endl;
}

void search_test()
{
    const SEARCH_RET ret = search();
    std::cout << "[Search Test] best move: " << move_to_ucimove(ret.first);
    std::cout << " best vl: " << ret.second << std::endl;
}
