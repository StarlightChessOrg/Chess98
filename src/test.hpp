#pragma once
#include "search.hpp"

// alpha-beta + history + killer (MovePicker; no TT/PVS/qsearch)
DEPTH test_ply_ { 0 };
std::uint64_t ab_nodes_ { 0 };
std::uint64_t ab_nodes_total_ { 0 };

VL alphabeta_vl_(DEPTH depth, VL a, VL b)
{
    ab_nodes_++;
    if (depth == 0) return evaluate();
    const VL original_alpha = a;
    VL vlbest { -INF };
    Move movebest { };
    MovePicker mp { depth };
    for (Move m = mp.next(); m; m = mp.next()) {
        position_move(m), test_ply_++;
        const VL vl = -alphabeta_vl_(depth - 1, -b, -a);
        position_undo(), test_ply_--;
        if (vl > vlbest) {
            movebest = m;
            vlbest = vl;
            a = std::max(a, vl);
            if (vl >= b) break;
        }
    }
    if (movebest) {
        if (vlbest > original_alpha) killer_set(movebest, depth); // fail-high / exact
        history_set(movebest, g_team, depth);
    }
    return vlbest != -INF ? vlbest : VL(-INF + test_ply_);
}

// iterative deepening alpha-beta + history + killer
SEARCH_RET search_alphabeta()
{
    const Timer timer { 1000 };
    Move movebest { };
    VL vlbest { -INF };
    ab_nodes_total_ = 0;
    for (DEPTH depth = 1; !timer.time_up_3xless(); depth++) {
        if (depth > g_maxdepth) break;
        Move depth_best { };
        VL depth_vl { -INF };
        VL a = -INF, b = INF;
        ab_nodes_ = 0;
        MovePicker mp { depth };
        ab_nodes_ = 1; // root
        for (Move m = mp.next(); m; m = mp.next()) {
            position_move(m), test_ply_ = 1;
            const VL vl = -alphabeta_vl_(depth - 1, -b, -a);
            position_undo(), test_ply_ = 0;
            if (vl > depth_vl) {
                depth_vl = vl;
                depth_best = m;
                a = std::max(a, vl);
            }
        }
        ab_nodes_total_ += ab_nodes_;
        if (depth_best) {
            killer_set(depth_best, depth);
            history_set(depth_best, g_team, depth);
            movebest = depth_best;
            vlbest = depth_vl;
        }
        std::cout << "alphabeta depth: " << int(depth)
                  << " nodes: " << ab_nodes_
                  << " total: " << ab_nodes_total_
                  << " time_ms: " << timer.duration()
                  << std::endl;
    }
    std::cout << "alphabeta total nodes: " << ab_nodes_total_
              << " nps: " << (timer.duration() > 0 ? ab_nodes_total_ * 1000 / timer.duration() : 0)
              << std::endl;
    return { movebest, vlbest };
}

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
