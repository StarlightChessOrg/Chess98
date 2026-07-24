#pragma once
#include "evaluate.hpp"
#include "moves.hpp"

STATE g_searchstop { 0 };
DEPTH g_maxdepth = { 20 };
DEPTH distance_ { 0 };
std::uint64_t nodes_ab_ { 0 };
std::uint64_t nodes_q_ { 0 };

SEARCH_RET search();
VL search_vl_(DEPTH depth, VL a, VL b, bool CUT);
VL search_q_(VL a, VL b, DEPTH depth);
void mark_checking_move_(bool checking);

// search for best move and vl
SEARCH_RET search()
{
    const Timer timer { 1000 };
    VL vl { -INF };
    std::uint64_t total_ab = 0, total_q = 0;
    for (DEPTH depth = 1; !timer.time_up_3xless(); depth++) {
        if (depth > g_maxdepth || (g_searchstop ? g_searchstop-- : 0)) break;
        nodes_ab_ = nodes_q_ = 0;
        vl = search_vl_(depth, -INF, INF, NODE_PV);
        total_ab += nodes_ab_;
        total_q += nodes_q_;
        // TODO: |DEBUG|
        std::cout << "depth: " << int(depth)
                  << " ab: " << nodes_ab_
                  << " q: " << nodes_q_
                  << " sum: " << (nodes_ab_ + nodes_q_)
                  << " total: " << (total_ab + total_q)
                  << " time_ms: " << timer.duration()
                  << std::endl;
    }
    const auto all = total_ab + total_q;
    std::cout << "total ab: " << total_ab << " q: " << total_q
              << " all: " << all
              << " nps: " << (timer.duration() > 0 ? all * 1000 / timer.duration() : 0)
              << std::endl;
    const Move move = tt_get_move();
    return { move, vl };
}

// search for the best vl
// PVS: PV = first full-window + scout null-window + research; CUT = null-window only
VL search_vl_(DEPTH depth, VL a, VL b, bool is_cut)
{
    nodes_ab_++;
    if (depth == 0) return search_q_(a, b, Q_MAX_DISTANCE);
    if (is_cut) a = b - 1; // force zero-width window (beta-1, beta)
    const VL original_alpha = a;
    VL vlbest { -INF };
    Move movebest { };

    // checking validation first (TT hit must not skip perpetual-check marking)
    const bool checking = in_check();
    mark_checking_move_(checking);

    // tt vl
    const VL vlhash = tt_get_vl(g_hashkey, depth, a, b);
    if (vlhash != INVALID_VL) return vlhash;
    if (!checking) {
        // futility pruning
        // TODO: |DEBUG|
        const VL vl = evaluate();
        if (depth <= 2 && vl - FP_MARGIN * depth >= b) return vl;
        // TODO: null move pruning
    }

    // repeat status validation
    if (is_repeat()) return -INF;

    // search
    MovePicker mp { depth };
    for (Move move = mp.next(); move; move = mp.next()) {
        position_move(move), distance_++;
        VL vl { -INF };
        if (is_cut) {
            vl = -search_vl_(depth - 1, -b, -b + 1, NODE_CUT);
        } else if (vlbest == -INF) {
            vl = -search_vl_(depth - 1, -b, -a, NODE_PV);
        } else {
            vl = -search_vl_(depth - 1, -a - 1, -a, NODE_CUT);
            if (a < vl && vl < b) {
                vl = -search_vl_(depth - 1, -b, -a, NODE_PV);
            }
        }
        position_undo(), distance_--;
        if (vl > vlbest) {
            movebest = move;
            vlbest = vl;
            a = std::max(a, vl);
            if (vl >= b) break;
        }
    }

    // caching the search informantion
    if (movebest) {
        HASH_FLAG movetype { EXACT };
        if (vlbest >= b) {
            movetype = BETA;
        } else if (vlbest <= original_alpha) {
            movetype = ALPHA;
        }
        if (movetype != ALPHA) killer_set(movebest, depth);
        history_set(movebest, g_team, depth);
        tt_set(g_hashkey, movetype, depth, movebest, vlbest);
    }

    return vlbest != -INF ? vlbest : vlbest + distance_;
}

// rough capture gain for delta pruning (SEE weights scaled toward eval units)
inline VL q_capture_gain_(Move move)
{
    return VL(WEIGHTS[std::size_t(std::abs(piece_on(move.end)))] * 12);
}

// true if the side that just moved left their king in check
inline bool left_in_check_()
{
    g_team = -g_team;
    const bool bad = in_check();
    g_team = -g_team;
    return bad;
}

// search quiescence (search-side reductions only; evaluate left as-is)
VL search_q_(VL a, VL b, DEPTH depth)
{
    // TODO: |DEBUG|
    return evaluate();
    nodes_q_++;
    if (distance_ >= Q_MAX_DISTANCE || depth <= 0) return evaluate();
    VL vlbest { -INF };

    // checking validation
    const bool checking = in_check();
    mark_checking_move_(checking);
    if (checking) {
        depth = std::min(depth, Q_CHECKING_DEPTH);
    } else {
        const VL vl = evaluate();
        if (vl >= b) return vl;
        vlbest = vl;
        if (vl > a) a = vl;
    }

    // repeat status validation
    if (is_repeat()) return -INF;

    // search: all moves if in check, else captures only
    std::vector<Move> moves { };
    if (checking) {
        moves = gen_all_moves();
    } else {
        moves = gen_all_capture_moves();
        mvvlva_sort(moves);
    }
    for (const Move move : moves) {
        if (!checking) {
            // delta: even winning the piece for free cannot raise alpha
            if (vlbest + q_capture_gain_(move) + Q_DELTA_MARGIN <= a) continue;
            // SEE: skip losing exchanges
            if (!see_ge(move, 0)) continue;
        }
        position_move(move), distance_++;
        if (left_in_check_()) { // illegal / self-check
            position_undo(), distance_--;
            continue;
        }
        const VL vl = -search_q_(-b, -a, depth - 1);
        position_undo(), distance_--;
        if (vl > vlbest) {
            vlbest = vl;
            a = std::max(a, vl);
            if (vl >= b) break;
        }
    }

    return vlbest != -INF ? vlbest : vlbest + distance_;
}

void mark_checking_move_(bool checking)
{
    if (checking && !g_history_checkings.empty()) {
        g_history_checkings.back() = true;
    }
}
