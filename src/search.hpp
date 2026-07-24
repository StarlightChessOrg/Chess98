#pragma once
#include "evaluate.hpp"
#include "moves.hpp"

STATE g_searchstop { 0 };
DEPTH g_maxdepth = { 20 };
DEPTH distance_ { 0 };

SEARCH_RET search();
VL search_vl_(DEPTH depth, VL a, VL b, bool is_cut, bool ban_null = false);
VL search_q_(VL a, VL b, DEPTH depth);
void mark_checking_move_(bool checking);
bool null_okay_();

// search for best move and vl
SEARCH_RET search()
{
    const Timer timer { 1000 };
    VL vl { -INF };

    for (DEPTH depth = 1; !timer.time_up_3xless(); depth++) {
        // max depth and the stop command
        if (depth > g_maxdepth || (g_searchstop ? g_searchstop-- : 0)) break;

        vl = search_vl_(depth, -INF, INF, NODE_PV);
    }

    const Move move = tt_get_move();
    return { move, vl };
}

// check if the null move pruning can be used
// TODO: implement a safer detection
bool null_okay_()
{
    for (const POS p : get_pos_list()) {
        const int t = std::abs(piece_on(p));
        if (t == R_ROOK || t == R_CANNON || t == R_KNIGHT || t == R_PAWN) {
            return true;
        }
    }
    return false;
}

// search for the best vl
VL search_vl_(DEPTH depth, VL a, VL b, bool is_cut, bool ban_null)
{
    if (depth == 0) return search_q_(a, b, Q_MAX_DISTANCE);
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
        const VL vl = evaluate();
        if (depth <= 2 && vl - FP_MARGIN * depth >= b) return vl;

        // null move pruning (CUT nodes only; no consecutive nulls)
        if (is_cut && !ban_null && depth >= NULL_MOVE_MIN_DEPTH && null_okay_()) {
            const DEPTH r = DEPTH(NULL_MOVE_R + depth / 6);
            const DEPTH nd = depth > r + 1 ? DEPTH(depth - 1 - r) : DEPTH(0);
            position_do_null();
            distance_++;
            const VL vlnull = -search_vl_(nd, -b, -b + 1, NODE_CUT, true);
            distance_--;
            position_undo_null();
            if (vlnull >= b) {
                tt_set(g_hashkey, BETA, depth, Move { }, vlnull);
                return vlnull;
            }
        }
    }

    // repeat status validation
    if (is_repeat()) return -INF;

    // search
    MovePicker mp { depth };
    for (Move move = mp.next(); move; move = mp.next()) {
        position_move(move), distance_++;
        VL vl { -INF };
        if (is_cut) {
            vl = -search_vl_(depth - 1, -b, -b + 1, NODE_CUT, ban_null);
        } else if (vlbest == -INF) {
            vl = -search_vl_(depth - 1, -b, -a, NODE_PV, false);
        } else {
            vl = -search_vl_(depth - 1, -a - 1, -a, NODE_CUT, false);
            if (a < vl && vl < b) {
                vl = -search_vl_(depth - 1, -b, -a, NODE_PV, false);
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
