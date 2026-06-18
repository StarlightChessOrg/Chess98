#pragma once
#include "heuristic.hpp"
#include "position.hpp"

// king moves
template <bool G>
std::vector<Move> gen_king_(POS pos)
{
    std::vector<Move> ret { };
    ret.reserve(4);
    if ((2 < pos && pos < 15 || 65 < pos && pos < 78) && teamcheck<G>(pos + 9))
        ret.emplace_back(pos, pos + 9);
    if ((11 < pos && pos < 24 || 74 < pos && pos < 87) && teamcheck<G>(pos - 9))
        ret.emplace_back(pos, pos - 9);
    if ((pos % 9 == 4 || pos % 9 == 5) && teamcheck<G>(pos - 1))
        ret.emplace_back(pos, pos - 1);
    if ((pos % 9 == 4 || pos % 9 == 3) && teamcheck<G>(pos + 1))
        ret.emplace_back(pos, pos + 1);
    return ret;
}

// advisor moves
template <bool G>
std::vector<Move> gen_advisor_(POS pos)
{
    if (pos == 13 || pos == 76) { // black center
        std::vector<Move> ret { };
        ret.reserve(4);
        if (teamcheck<G>(pos - 10))
            ret.emplace_back(pos, pos - 10);
        if (teamcheck<G>(pos - 8))
            ret.emplace_back(pos, pos - 8);
        if (teamcheck<G>(pos + 10))
            ret.emplace_back(pos, pos + 10);
        if (teamcheck<G>(pos + 8))
            ret.emplace_back(pos, pos + 8);
        return ret;
    } else if (pos < 24 && teamcheck<G>(13)) { // black corner
        return { Move(pos, 13) };
    } else if (teamcheck<G>(76)) { // red corner
        return { Move(pos, 76) };
    }
    return { };
}

// bishop moves
template <bool G>
std::vector<Move> gen_bishop_(POS pos)
{
    std::vector<Move> ret { };
    ret.reserve(4);
    if (pos / 9 == 0 || pos / 9 == 5 || pos / 9 == 7 || pos / 9 == 3) {
        if (!piece_on(pos + 10) && teamcheck<G>(pos + 16))
            ret.emplace_back(pos, pos + 16);
        if (!piece_on(pos + 12) && teamcheck<G>(pos + 20))
            ret.emplace_back(pos, pos + 20);
    }
    if (pos / 9 == 4 || pos / 9 == 9 || pos / 9 == 7 || pos / 9 == 3) {
        if (!piece_on(pos - 10) && teamcheck<G>(pos - 16))
            ret.emplace_back(pos, pos - 16);
        if (!piece_on(pos - 12) && teamcheck<G>(pos - 20))
            ret.emplace_back(pos, pos - 20);
    }
    return ret;
}

// knight moves
template <bool G>
std::vector<Move> gen_knight_(POS pos)
{
    std::vector<Move> ret { };
    ret.reserve(8);
    if (pos > 17 && !piece_on(pos - 9)) {
        if (pos % 9 != 0 && teamcheck<G>(pos - 19)) {
            ret.emplace_back(pos, pos - 19);
        }
        if (pos % 9 != 8 && teamcheck<G>(pos - 17)) {
            ret.emplace_back(pos, pos - 17);
        }
    }
    if (pos < 72 && !piece_on(pos + 9)) {
        if (pos % 9 != 8 && teamcheck<G>(pos + 19)) {
            ret.emplace_back(pos, pos + 19);
        }
        if (pos % 9 != 0 && teamcheck<G>(pos + 17)) {
            ret.emplace_back(pos, pos + 17);
        }
    }
    if (pos % 9 > 1 && !piece_on(pos - 1)) {
        if (pos / 9 != 0 && teamcheck<G>(pos - 11)) {
            ret.emplace_back(pos, pos - 11);
        }
        if (pos / 9 != 9 && teamcheck<G>(pos + 7)) {
            ret.emplace_back(pos, pos + 7);
        }
    }
    if (pos % 9 < 7 && !piece_on(pos + 1)) {
        if (pos / 9 != 9 && teamcheck<G>(pos + 11)) {
            ret.emplace_back(pos, pos + 11);
        }
        if (pos / 9 != 0 && teamcheck<G>(pos - 7)) {
            ret.emplace_back(pos, pos - 7);
        }
    }
    return ret;
}

// rook moves
template <bool G>
std::vector<Move> gen_rook_(POS pos)
{
    std::vector<Move> ret { };
    ret.reserve(4);
    const auto bl9 = get_bl9(pos), bl10 = get_bl10(pos);
    const auto [left, right] = rook_9(bl9, pos);
    const auto [top, bottom] = rook_10(bl10, pos);
    if constexpr (G) {
        if (teamcheck<G>(left))
            ret.emplace_back(pos, left);
        if (teamcheck<G>(right))
            ret.emplace_back(pos, right);
        if (teamcheck<G>(top))
            ret.emplace_back(pos, top);
        if (teamcheck<G>(bottom))
            ret.emplace_back(pos, bottom);
    } else {
        for (POS p = pos - 1; p > left; p--) ret.emplace_back(pos, p);
        if (!piece_on(top))
            ret.emplace_back(pos, top);
        for (POS p = pos + 1; p < right; p++) ret.emplace_back(pos, p);
        if (!piece_on(right))
            ret.emplace_back(pos, right);
        for (POS p = pos - 9; p > top; p -= 9) ret.emplace_back(pos, p);
        if (!piece_on(left))
            ret.emplace_back(pos, left);
        for (POS p = pos + 9; p < bottom; p += 9) ret.emplace_back(pos, p);
        if (!piece_on(bottom))
            ret.emplace_back(pos, bottom);
    }
    return ret;
}

// cannon moves
template <bool G>
std::vector<Move> gen_cannon_(POS pos)
{
    if constexpr (G) {
        std::vector<Move> ret { };
        ret.reserve(4);
        const auto bl9 = get_bl9(pos), bl10 = get_bl10(pos);
        const auto [left, right] = cannon_9(bl9, pos);
        const auto [top, bottom] = cannon_10(bl10, pos);
        if (left < INVALID_POS && teamcheck<G>(left))
            ret.emplace_back(pos, left);
        if (right < INVALID_POS && teamcheck<G>(right))
            ret.emplace_back(pos, right);
        if (top < INVALID_POS && teamcheck<G>(top))
            ret.emplace_back(pos, top);
        if (bottom < INVALID_POS && teamcheck<G>(bottom))
            ret.emplace_back(pos, bottom);
        return ret;
    }
    return gen_rook_<false>(pos);
}

// pawn moves
template <bool G>
std::vector<Move> gen_pawn_(POS pos)
{
    std::vector<Move> ret { };
    ret.reserve(3);
    const int target = pos - 9 * g_team;
    if (0 <= target && target < 90 && teamcheck<G>(target))
        ret.emplace_back(pos, target);
    if ((pos / 9 < 5 && g_team == R) || (pos / 9 > 4 && g_team == B)) {
        if (pos % 9 != 0 && teamcheck<G>(pos - 1))
            ret.emplace_back(pos, pos - 1);
        if (pos % 9 != 8 && teamcheck<G>(pos + 1))
            ret.emplace_back(pos, pos + 1);
    }
    return ret;
}

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

class MovePicker {
    DEPTH depth { 0 };
    TEAM team { 0 };
    MovePickerStatus status = STATUS_TT;
    Move tt_move { }; // regester tt move
    std::array<Move, 2> killers { }; // and killer moves
    std::vector<Move> moves { };
    std::vector<Move> bad_captures { };
    bool generated = false; // 0 is false, others are true
    std::uint8_t i { 0 }; // the index

public:
    MovePicker(DEPTH depth, TEAM team) : depth(depth), team(team) { }
    Move next();
};

Move MovePicker::next()
{
    if (status == STATUS_TT) {
        tt_move = tt_get_move(g_hashkey), status++;
        return tt_move ? tt_move : next();
    } else if (status == STATUS_GOOD_CAPTURES) {
        if (!generated) {
            moves = gen_all_capture_moves();
            killers = killer_get(depth), generated = true;
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
            history_sort(moves, team);
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
