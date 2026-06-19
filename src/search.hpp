#pragma once
#include "evaluate.hpp"
#include "moves.hpp"

// const defines
constexpr DEPTH Q_MAX_DISTANCE { 64 };
constexpr DEPTH Q_CHECKING_DEPTH { 8 };

// global variables
STATE g_searchstop { 0 };
DEPTH g_maxdepth = { 20 };

// local variables
DEPTH distance_ { 0 };

// search
VL search_q_(VL a, VL b, DEPTH depth)
{
    if (distance_ == Q_MAX_DISTANCE || depth == 0) return evaluate();
    // mdp todo
    const bool checking = in_check();
    if (checking) {
        history_checkings.emplace_back(true);
        depth = std::min(depth, Q_CHECKING_DEPTH);
    } else {
        // ndp todo
    }
    // repeat status validation todo
    // search
    VL vlbest { -INF };
    std::vector<Move> moves { };
    if (checking) {
        moves = gen_all_quiet_moves();
        history_sort(moves, g_team);
    } else {
        moves = gen_all_capture_moves();
        mvvlva_sort(moves);
    }
    for (const Move m : moves) {
        position_move(m), distance_++;
        const VL vl = -search_q_(-b, -a, depth - 1);
        position_undo(), distance_--, history_checkings.pop_back();
        if (vl > vlbest) {
            if (vl > b) return vl;
            vlbest = vl;
            a = std::max(a, vl);
        }
    }
    return vlbest != -INF ? vlbest : vlbest + distance_;
}

template <bool CUT>
VL search_vl_(DEPTH depth, VL a, VL b)
{
    if (depth == 0) return search_q_(a, b, Q_MAX_DISTANCE);
    const VL vlhash = tt_get_vl(g_hashkey, depth, a, b);
    if (vlhash > b) return vlhash;
    // mdp todo
    const bool checking = in_check();
    if (checking) {
        history_checkings.emplace_back(checking);
    } else {
        // fp todo
        // nmp todo
    }
    // repeat status validation todo
    // search
    VL vlbest { -INF };
    HASH_FLAG movetype { EXACT };
    MovePicker mp { depth };
    for (Move m = mp.next(); m; m = mp.next()) {
        position_move(m), distance_++;
        VL vl { -INF };
        if constexpr (CUT) {
            if (vlbest == -INF) {
                vl = -search_vl_<false>(depth - 1, -b, -a);
            } else {
                vl = -search_vl_<true>(depth - 1, -INF, -a);
                if (a < vl && vl < b) {
                    vl = -search_vl_<false>(depth - 1, -b, -a);
                }
            }
        } else {
            vl = -search_vl_<true>(depth - 1, -INF, -a);
        }
        position_undo(), distance_--, history_captures_.pop_back();
        if (vl > vlbest) {
            if (vl > b) break;
            a = std::max(a, vl);
        }
    }

    return vlbest != -INF ? vlbest : vlbest + distance_;
}

SEARCH_RET search()
{
    const Timer timer { 1000 };
    VL vl { -INF };
    for (DEPTH depth = 0; !timer.time_up_3xless(); depth++) {
        if (depth > g_maxdepth || (g_searchstop ? g_searchstop-- : 0)) break;
        vl = search_vl_<false>(depth, -INF, INF);
    }
    const Move move = tt_get_move(g_hashkey);
    return { move, vl };
}
