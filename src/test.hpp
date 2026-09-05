#pragma once
#include "search.hpp"


void move_preformance_test()
{
    assert(gen_all_moves().size() == 44);
    Timer t { 1000 };
    std::int64_t iterations = 0, id = 0, num = 0;
    while (!t.time_up()) {
        MovePicker a { 1 };
        for (Move m = a.next(); m; m = a.next()) {
            id += m.beg, id += m.end, num++;
        }
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
        for (const Move m : gen_all_moves()) {
            id += m.beg, id += m.end;
            num++;
        }
        iterations++;
    }
    std::cout << "[Normal Generation 1 second]";
    std::cout << " iterations: " << iterations;
    std::cout << " num: " << num;
    std::cout << " id: " << id;
    std::cout << std::endl;
}

// pure minimax: random leaf, no qsearch / alphabeta / tt
std::int64_t minmax_nodes_ { 0 };
DEPTH test_ply_ { 0 };

VL minmax_vl_(DEPTH depth)
{
    minmax_nodes_++;
    if (depth == 0) {
        static std::uint32_t rng = 1;
        rng = rng * 1664525u + 1013904223u + std::uint32_t(g_hashkey);
        return VL(std::int16_t(rng));
    }
    VL vlbest { -INF };
    for (const Move m : gen_all_moves()) {
        position_move(m), test_ply_++;
        const VL vl = -minmax_vl_(DEPTH(depth - 1));
        position_undo(), test_ply_--;
        if (vl > vlbest) vlbest = vl;
    }
    if (vlbest == -INF) vlbest = VL(-INF + test_ply_);
    return vlbest;
}

void minmax_example()
{
    position_init(fen_to_matrix("rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RNBAKABNR w"), R);
    minmax_nodes_ = 0;
    test_ply_ = 0;
    const DEPTH depth = 5;
    const Timer timer { };
    const VL vl = minmax_vl_(depth);
    volatile VL keep_vl = vl;
    volatile std::int64_t keep_nodes = minmax_nodes_;
    std::cout << "minmax depth " << int(depth);
    std::cout << " vl " << int(keep_vl);
    std::cout << " nodes " << keep_nodes;
    std::cout << " time " << timer.duration();
    std::cout << std::endl;
}
