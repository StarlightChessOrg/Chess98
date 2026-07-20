#include "search.hpp"

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

template <bool CUT>
VL search_vl_(DEPTH depth, VL a, VL b)
{
    if (depth == 0) return search_q_(a, b, Q_MAX_DISTANCE);
    const VL vlhash = tt_get_vl(g_hashkey, depth, a, b);
    if (vlhash > b) return vlhash;
    const bool checking = in_check();
    if (checking) {
        history_checkings.emplace_back(checking);
    } else {
        // fp
        const VL vl = evaluate();
        if (depth <= 2 && vl - FP_MARGIN * depth >= b) return vl;
        // nmp todo
    }
    // repeat status validation todo
    VL vlbest { -INF };
    Move movebest { };
    HASH_FLAG movetype { EXACT };
    MovePicker mp { depth };
    for (Move m = mp.next(); m; m = mp.next()) {
        position_move(m), distance_++;
        VL vl { -INF };
        if (CUT) {
            if (vlbest == -INF) {
                vl = -search_vl_<NODE_PV>(depth - 1, -b, -a);
            } else {
                vl = -search_vl_<NODE_CUT>(depth - 1, -INF, -a);
                if (a < vl && vl < b) {
                    vl = -search_vl_<NODE_PV>(depth - 1, -b, -a);
                }
            }
        } else {
            vl = -search_vl_<NODE_CUT>(depth - 1, -INF, -a);
        }
        position_undo(), distance_--, history_captures_.pop_back();
        if (vl > vlbest) {
            movebest = m;
            a = std::max(a, vl);
            if (vl > b) break;
        }
    }
    if (movebest) {
        if (movetype != ALPHA) killer_set(movebest, depth);
        history_set(movebest, g_team, depth);
        tt_set(g_hashkey, movetype, depth, movebest, vlbest);
    }
    return vlbest != -INF ? vlbest : static_cast<VL>(vlbest + distance_);
}

VL search_q_(VL a, VL b, DEPTH depth)
{
    if (distance_ == Q_MAX_DISTANCE || depth == 0) return evaluate();
    const bool checking = in_check();
    VL vlbest { -INF };
    if (checking) {
        history_checkings.emplace_back(true);
        depth = std::min(depth, Q_CHECKING_DEPTH);
    } else {
        // delta pruning
        const VL vl = evaluate();
        if (vl >= b) return vl;
        vlbest = vl;
        if (vl > a) a = vl;
    }
    // repeat status validation todo
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
    return vlbest != -INF ? vlbest : static_cast<VL>(vlbest + distance_);
}
