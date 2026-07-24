#pragma once
#include "evaluate.hpp"
#include "moves.hpp"

STATE g_searchstop { 0 };
DEPTH g_maxdepth = { 20 };
DEPTH distance_ { 0 };

SEARCH_RET search();
VL search_vl_(DEPTH depth, VL a, VL b, bool CUT);
VL search_q_(VL a, VL b, DEPTH depth);

// search for best move and vl
SEARCH_RET search()
{
    const Timer timer { 1000 };
    VL vl { -INF };
    for (DEPTH depth = 1; !timer.time_up_3xless(); depth++) {
        if (depth > g_maxdepth || (g_searchstop ? g_searchstop-- : 0)) break;
        vl = search_vl_(depth, -INF, INF, NODE_PV);
        // DEBUG
        std::cout << "depth: " << int(depth) << std::endl;
    }
    const Move move = tt_get_move();
    return { move, vl };
}

// search for the best vl
VL search_vl_(DEPTH depth, VL a, VL b, bool is_cut)
{
    if (depth == 0) return search_q_(a, b, Q_MAX_DISTANCE);
    const VL original_alpha = a;
    VL vlbest { -INF };
    Move movebest { };

    // tt vl
    // DEBUG
    // const VL vlhash = tt_get_vl(g_hashkey, depth, a, b);
    // if (vlhash != INVALID_VL) return vlhash;

    // checking validation
    const bool checking = in_check();
    g_history_checkings.emplace_back(checking);
    if (!checking) {
        // futility pruning
        const VL vl = evaluate();
        if (depth <= 2 && vl - FP_MARGIN * depth >= b) return vl;
        // TODO: null move pruning
    }

    // repeat status validation
    if (is_repeat()) {
        if (checking) g_history_checkings.pop_back();
        return -INF;
    }

    // search
    MovePicker mp { depth };
    for (Move move = mp.next(); move; move = mp.next()) {
        position_move(move), distance_++;
        VL vl { -INF };
        if (is_cut) {
            if (vlbest == -INF) {
                vl = -search_vl_(depth - 1, -b, -a, NODE_PV);
            } else {
                vl = -search_vl_(depth - 1, -INF, -a, NODE_CUT);
                if (a < vl && vl < b) {
                    vl = -search_vl_(depth - 1, -b, -a, NODE_PV);
                }
            }
        } else {
            vl = -search_vl_(depth - 1, -INF, -a, NODE_PV);
        }
        position_undo(), distance_--;
        if (vl > vlbest) {
            movebest = move;
            vlbest = vl;
            a = std::max(a, vl);
            if (vl > b) break;
        }
    }

    // caching the search informantion
    if (movebest) {
        HASH_FLAG movetype { EXACT };
        if (vlbest > b) {
            movetype = BETA;
        } else if (vlbest <= original_alpha) {
            movetype = ALPHA;
        }
        if (movetype != ALPHA) killer_set(movebest, depth);
        history_set(movebest, g_team, depth);
        // DEBUG
        // tt_set(g_hashkey, movetype, depth, movebest, vlbest);
    }

    // end
    g_history_checkings.pop_back();
    return vlbest != -INF ? vlbest : vlbest + distance_;
}

// search quiescence
VL search_q_(VL a, VL b, DEPTH depth)
{
    // DEBUG
    return evaluate();
    if (distance_ == Q_MAX_DISTANCE || depth == 0) return evaluate();
    VL vlbest { -INF };

    // checking validation
    const bool checking = in_check();
    g_history_checkings.emplace_back(true);
    if (checking) {
        depth = std::min(depth, Q_CHECKING_DEPTH);
    } else {
        // delta pruning
        const VL vl = evaluate();
        if (vl >= b) return vl;
        vlbest = vl;
        if (vl > a) a = vl;
    }

    // repeat status validation
    if (is_repeat()) {
        if (checking) g_history_checkings.pop_back();
        return -INF;
    }

    // search
    std::vector<Move> moves { };
    if (checking) {
        moves = gen_all_moves();
    } else {
        moves = gen_all_capture_moves();
        mvvlva_sort(moves);
    }
    for (const Move move : moves) {
        position_move(move), distance_++;
        const VL vl = -search_q_(-b, -a, depth - 1);
        position_undo(), distance_--;
        if (vl > vlbest) {
            vlbest = vl;
            a = std::max(a, vl);
            if (vl > b) break;
        }
    }

    // end
    g_history_checkings.pop_back();
    return vlbest != -INF ? vlbest : vlbest + distance_;
}
