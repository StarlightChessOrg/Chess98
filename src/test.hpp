#pragma once
#include "search.hpp"

// plain alpha-beta baseline: optional TT only (no PVS/qsearch/killer/history/MovePicker)
DEPTH test_ply_ { 0 };
bool ab_use_tt_ { false };

VL alphabeta_vl_(DEPTH depth, VL a, VL b)
{
    if (depth == 0) return evaluate();

    if (ab_use_tt_) {
        const VL vlhash = tt_get_vl(g_hashkey, depth, a, b);
        if (vlhash != INVALID_VL) return vlhash;
    }

    const VL original_alpha = a;
    VL vlbest { -INF };
    Move movebest { };
    for (const Move m : gen_all_moves()) {
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

    if (vlbest == -INF) vlbest = VL(-INF + test_ply_);
    if (ab_use_tt_ && movebest) {
        HASH_FLAG flag = EXACT;
        if (vlbest >= b) flag = BETA;
        else if (vlbest <= original_alpha) flag = ALPHA;
        tt_set(g_hashkey, flag, depth, movebest, vlbest);
    }
    return vlbest;
}

SEARCH_RET search_alphabeta()
{
    const Timer timer { 1000 };
    Move movebest { };
    VL vlbest { -INF };
    for (DEPTH depth = 1; !timer.time_up_2xless(); depth++) {
        if (depth > g_maxdepth) break;
        Move depth_best { };
        VL depth_vl { -INF };
        VL a = -INF, b = INF;
        for (const Move m : gen_all_moves()) {
            position_move(m), test_ply_ = 1;
            const VL vl = -alphabeta_vl_(depth - 1, -b, -a);
            position_undo(), test_ply_ = 0;
            if (vl > depth_vl) {
                depth_vl = vl;
                depth_best = m;
                a = std::max(a, vl);
            }
        }
        if (depth_best) {
            movebest = depth_best;
            vlbest = depth_vl;
            if (ab_use_tt_) tt_set(g_hashkey, EXACT, depth, movebest, vlbest);
        }
    }
    return { movebest, vlbest };
}

void alphabeta_tt_compare()
{
    for (const bool use_tt : { false, true }) {
        ab_use_tt_ = use_tt;
        position_init(
            {
                B_ROOK, B_KNIGHT, B_BISHOP, B_ADVISOR, B_KING,
                B_ADVISOR, B_BISHOP, B_KNIGHT, B_ROOK,
                0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, B_CANNON, 0, 0, 0, 0, 0, B_CANNON, 0,
                B_PAWN, 0, B_PAWN, 0, B_PAWN, 0, B_PAWN, 0, B_PAWN,
                0, 0, 0, 0, 0, 0, 0, 0, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0,
                R_PAWN, 0, R_PAWN, 0, R_PAWN, 0, R_PAWN, 0, R_PAWN,
                0, R_CANNON, 0, 0, 0, 0, 0, R_CANNON, 0,
                0, 0, 0, 0, 0, 0, 0, 0, 0,
                R_ROOK, R_KNIGHT, R_BISHOP, R_ADVISOR, R_KING,
                R_ADVISOR, R_BISHOP, R_KNIGHT, R_ROOK
            },
            R);
        history_init();
        killer_init();
        tt_init();
        (void)search_alphabeta();
    }
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
    std::cout << "[Normal Generation 1 second]";
    std::cout << " iterations: " << iterations;
    std::cout << " num: " << num;
    std::cout << " id: " << id;
    std::cout << std::endl;
}

void search_test()
{
    search();
}
