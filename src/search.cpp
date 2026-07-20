#include "search.hpp"

SEARCH_RET search()
{
    const Timer timer { 1000 };
    VL vl { -INF };
    
    // search
    for (DEPTH depth = 1; !timer.time_up_3xless(); depth++) {
        if (depth > g_maxdepth || (g_searchstop ? g_searchstop-- : 0)) break;
        vl = search_vl_<NODE_PV>(depth, -INF, INF);
    }

    std::cout << search_vl_<NODE_PV>(1, -INF, INF) << std::endl;
    // end
    const Move move = tt_get_move(g_hashkey);
    return { move, vl };
}

template <bool CUT>
VL search_vl_(DEPTH depth, VL a, VL b)
{
    if (depth == 0) return search_q_(a, b, Q_MAX_DISTANCE);

    // tt
    const VL vlhash = tt_get_vl(g_hashkey, depth, a, b);
    if (vlhash > b) return vlhash;

    // checking validation
    VL vlbest { -INF };
    HASH_FLAG movetype { EXACT };
    const bool checking = in_check();
    if (checking) {
        history_checkings.emplace_back(checking);
    } else {
        // fultility pruning
        const VL vl = evaluate();
        if (depth <= 2 && vl - FP_MARGIN * depth >= b) return vl;
        // TODO: null and delta pruning
    }

    // repeat validation
    if (is_repeat()) {
        if (checking) history_checkings.pop_back();
        return 0;
    }

    // search
    Move movebest { };
    MovePicker mp { depth };
    for (Move m = mp.next(); m; m = mp.next()) {
        position_move(m), distance_++;
        VL vl { -INF };
        if constexpr (CUT) {
            vl = -search_vl_<NODE_CUT>(depth - 1, -INF, -a);
        } else {
            if (vlbest == -INF) {
                vl = -search_vl_<NODE_PV>(depth - 1, -b, -a);
            } else {
                vl = -search_vl_<NODE_CUT>(depth - 1, -INF, -a);
                if (a < vl && vl < b) {
                    vl = -search_vl_<NODE_PV>(depth - 1, -b, -a);
                }
            }
        }
        position_undo(), distance_--;
        if (vl > vlbest) {
            movebest = m;
            a = std::max(a, vl);
            if (vl > b) break;
        }
    }

    // caching information to tables
    if (movebest) {
        if (movetype != ALPHA) killer_set(movebest, depth);
        history_set(movebest, g_team, depth);
        tt_set(g_hashkey, movetype, depth, movebest, vlbest);
    }

    // end
    if (checking) history_checkings.pop_back();
    return vlbest != -INF ? vlbest : static_cast<VL>(vlbest + distance_);
}

VL search_q_(VL a, VL b, DEPTH depth)
{
    if (distance_ == Q_MAX_DISTANCE || depth == 0) return evaluate();
    VL vlbest { -INF };

    // checking validation
    const bool checking = in_check();
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

    // repeat validation
    if (is_repeat()) {
        if (checking) history_checkings.pop_back();
        return 0;
    }

    // move generation (checking extension or capture moves)
    std::vector<Move> moves { };
    if (checking) {
        moves = gen_all_quiet_moves();
        history_sort(moves, g_team);
    } else {
        moves = gen_all_capture_moves();
        mvvlva_sort(moves);
    }

    // search
    for (const Move m : moves) {
        position_move(m), distance_++;
        const VL vl = -search_q_(-b, -a, depth - 1);
        position_undo(), distance_--;
        if (vl > vlbest) {
            if (vl > b) {
                if (checking) history_checkings.pop_back();
                return vl;
            }
            vlbest = vl;
            a = std::max(a, vl);
        }
    }

    // end
    if (checking) history_checkings.pop_back();
    return vlbest != -INF ? vlbest : static_cast<VL>(vlbest + distance_);
}
