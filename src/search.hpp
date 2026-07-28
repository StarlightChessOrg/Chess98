#pragma once
#include "evaluate.hpp"
#include "moves.hpp"

STATE g_searchstop { 0 };
DEPTH g_maxdepth { 20 };
int g_searchduration { 1000 };
DEPTH distance_ { 0 };

SEARCH_RET search();
VL search_vl_(DEPTH depth, VL a, VL b, bool is_cut, bool ban_null, bool checking);
VL search_q_(VL a, VL b, DEPTH depth, bool checking);
void mark_checking_move_(bool checking);
bool null_okay_();
VL q_capture_gain_(Move move);
bool left_in_check_();

// search for best move and vl
SEARCH_RET search()
{
    const Timer timer { g_searchduration };
    VL vl { -INF };
    for (DEPTH depth = 1; !timer.time_up_2xless(); depth++) {
        vl = search_vl_(depth, -INF, INF, NODE_PV, false, in_check());
        if (depth > g_maxdepth || (g_searchstop ? g_searchstop-- : 0)) break;
        std::cout << int(depth) << " " << timer.duration() << std::endl;
    }
    const Move move = tt_get_move();
    return { move, vl };
}

// search for the best vl
VL search_vl_(DEPTH depth, VL a, VL b, bool is_cut, bool ban_null, bool checking)
{
    if (depth == 0) return search_q_(a, b, Q_MAX_DISTANCE, checking);
    const VL original_alpha = a;
    VL vlbest { -INF };
    Move movebest { };

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
            const VL vlnull = -search_vl_(nd, -b, -b + 1, NODE_CUT, true, in_check());
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
    int move_num { 0 };
    for (Move move = mp.next(); move; move = mp.next(), move_num++) {
        assert(std::abs(g_board[move.end]) != R_KING && g_board[move.beg] != 0);
        const PTYPE capture = piece_on(move.end);

        position_move(move), distance_++;

        // checking validation
        const bool gives_check = in_check();
        mark_checking_move_(gives_check);

        // check extension temporarily disabled
        const DEPTH normal_depth = DEPTH(depth - 1);
        DEPTH reduction { 0 };
        const bool c1 = !checking && !capture && !gives_check;
        const bool c2 = depth >= LMR_MIN_DEPTH && move_num >= LMR_MIN_MOVES;
        if (c1 && c2) {
            reduction = LMR_BASE;
            if (depth >= 6) reduction++;
            if (move_num >= 6) reduction++;
            if (is_cut) reduction++;
            if (reduction > normal_depth) reduction = normal_depth;
        }
        const DEPTH lmr_depth = normal_depth - reduction;

        // pvs
        VL vl { -INF };
        if (!is_cut && vlbest == -INF) {
            vl = -search_vl_(normal_depth, -b, -a, NODE_PV, false, gives_check);
        } else if (is_cut) {
            vl = -search_vl_(lmr_depth, -b, -b + 1, NODE_CUT, ban_null, gives_check);
            if (reduction && vl >= b)
                vl = -search_vl_(normal_depth, -b, -b + 1, NODE_CUT, ban_null, gives_check);
        } else {
            vl = -search_vl_(lmr_depth, -a - 1, -a, NODE_CUT, false, gives_check);
            if (reduction && vl > a)
                vl = -search_vl_(normal_depth, -a - 1, -a, NODE_CUT, false, gives_check);
            if (a < vl && vl < b)
                vl = -search_vl_(normal_depth, -b, -a, NODE_PV, false, gives_check);
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

// search quiescence (search-side reductions only; evaluate left as-is)
VL search_q_(VL a, VL b, DEPTH depth, bool checking)
{
    if (distance_ >= Q_MAX_DISTANCE || depth <= 0) return evaluate();
    VL vlbest { -INF };

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
        const bool gives_check = in_check();
        mark_checking_move_(gives_check);
        const VL vl = -search_q_(-b, -a, depth - 1, gives_check);
        position_undo(), distance_--;
        if (vl > vlbest) {
            vlbest = vl;
            a = std::max(a, vl);
            if (vl >= b) break;
        }
    }

    return vlbest != -INF ? vlbest : vlbest + distance_;
}

// mark the previous move as checking move or not
void mark_checking_move_(bool checking)
{
    if (checking && !g_history_checkings.empty()) {
        g_history_checkings.back() = true;
    }
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

// rough capture gain for delta pruning (SEE weights scaled toward eval units)
VL q_capture_gain_(Move move)
{
    return VL(WEIGHTS[std::size_t(std::abs(piece_on(move.end)))] * 12);
}

// true if the side that just moved left their king in check
bool left_in_check_()
{
    g_team = -g_team;
    const bool bad = in_check();
    g_team = -g_team;
    return bad;
}
