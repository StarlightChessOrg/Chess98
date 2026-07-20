#include "moves.hpp"

// generate all capture moves
std::vector<Move> gen_all_capture_moves()
{
    std::vector<Move> ret { };
    ret.reserve(16);
    for (const POS p : pos_list()) {
        const PTYPE t = abs(piece_on(p));
        if (t == R_KING) {
            const auto moves = gen_king_<true>(p);
            ret.insert(ret.end(), moves.begin(), moves.end());
        } else if (t == R_ADVISOR) {
            const auto moves = gen_advisor_<true>(p);
            ret.insert(ret.end(), moves.begin(), moves.end());
        } else if (t == R_BISHOP) {
            const auto moves = gen_bishop_<true>(p);
            ret.insert(ret.end(), moves.begin(), moves.end());
        } else if (t == R_KNIGHT) {
            const auto moves = gen_knight_<true>(p);
            ret.insert(ret.end(), moves.begin(), moves.end());
        } else if (t == R_ROOK) {
            const auto moves = gen_rook_<true>(p);
            ret.insert(ret.end(), moves.begin(), moves.end());
        } else if (t == R_CANNON) {
            const auto moves = gen_cannon_<true>(p);
            ret.insert(ret.end(), moves.begin(), moves.end());
        } else if (t == R_PAWN) {
            const auto moves = gen_pawn_<true>(p);
            ret.insert(ret.end(), moves.begin(), moves.end());
        }
    }
    return ret;
}

// generate all quiet moves
std::vector<Move> gen_all_quiet_moves()
{
    std::vector<Move> ret { };
    ret.reserve(64);
    for (const POS p : pos_list()) {
        const PTYPE t = abs(piece_on(p));
        if (t == R_KING) {
            const auto moves = gen_king_<false>(p);
            ret.insert(ret.end(), moves.begin(), moves.end());
        } else if (t == R_ADVISOR) {
            const auto moves = gen_advisor_<false>(p);
            ret.insert(ret.end(), moves.begin(), moves.end());
        } else if (t == R_BISHOP) {
            const auto moves = gen_bishop_<false>(p);
            ret.insert(ret.end(), moves.begin(), moves.end());
        } else if (t == R_KNIGHT) {
            const auto moves = gen_knight_<false>(p);
            ret.insert(ret.end(), moves.begin(), moves.end());
        } else if (t == R_ROOK) {
            const auto moves = gen_rook_<false>(p);
            ret.insert(ret.end(), moves.begin(), moves.end());
        } else if (t == R_CANNON) {
            const auto moves = gen_cannon_<false>(p);
            ret.insert(ret.end(), moves.begin(), moves.end());
        } else if (t == R_PAWN) {
            const auto moves = gen_pawn_<false>(p);
            ret.insert(ret.end(), moves.begin(), moves.end());
        }
    }
    return ret;
}

Move MovePicker::next()
{
    if (status == STATUS_TT) {
        tt_move = tt_get_move(g_hashkey), status++;
        return tt_move ? tt_move : next();
    } else if (status == STATUS_GOOD_CAPTURES) {
        if (!generated) {
            moves = gen_all_capture_moves();
            mvvlva_sort(moves), generated = true;
        }
        if (i < moves.size()) {
            if (moves[i] == tt_move) return next();
            if (see_ge(moves[i], 0)) {
                return moves[i++];
            } else {
                bad_captures.emplace_back(moves[i++]);
                return next();
            }
        } else {
            generated = i = 0, status++;
            return next();
        }
    } else if (status == STATUS_KILLER) {
        if (!generated) {
            killers = killer_get(depth), generated = true;
        }
        if (i < killers.size()) {
            if (legal_move(killers[i++])) {
                return killers[static_cast<size_t>(i - 1)];
            } else {
                return next();
            }
        } else {
            generated = i = 0, status++;
            return next();
        }
    } else if (status == STATUS_QUIET) {
        if (!generated) {
            moves = gen_all_quiet_moves(), generated = true;
            history_sort(moves, g_team);
        }
        if (i < moves.size()) {
            Move m = moves[i++];
            bool c = m == tt_move || m == killers[0] || m == killers[1];
            return c ? next() : m;
        } else {
            generated = i = 0, status++;
            return next();
        }
    }
    return i < bad_captures.size() ? bad_captures[i++] : Move { };
}
