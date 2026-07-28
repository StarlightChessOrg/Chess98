#pragma once
#include "heuristic.hpp"
#include "position.hpp"

// king moves
template <bool G>
std::vector<Move> gen_king_(POS pos)
{
    std::vector<Move> ret { };
    ret.reserve(4);
    if ((2 < pos && pos < 15 || 65 < pos && pos < 78) && teamcheck(pos + 9, G))
        ret.emplace_back(pos, pos + 9);
    if ((11 < pos && pos < 24 || 74 < pos && pos < 87) && teamcheck(pos - 9, G))
        ret.emplace_back(pos, pos - 9);
    if ((pos % 9 == 4 || pos % 9 == 5) && teamcheck(pos - 1, G))
        ret.emplace_back(pos, pos - 1);
    if ((pos % 9 == 4 || pos % 9 == 3) && teamcheck(pos + 1, G))
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
        if (teamcheck(pos - 10, G))
            ret.emplace_back(pos, pos - 10);
        if (teamcheck(pos - 8, G))
            ret.emplace_back(pos, pos - 8);
        if (teamcheck(pos + 10, G))
            ret.emplace_back(pos, pos + 10);
        if (teamcheck(pos + 8, G))
            ret.emplace_back(pos, pos + 8);
        return ret;
    } else if (pos < 24 && teamcheck(13, G)) { // black corner
        return { Move(pos, 13) };
    } else if (teamcheck(76, G)) { // red corner
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
        if (!piece_on(pos + 10) && teamcheck(pos + 16, G))
            ret.emplace_back(pos, pos + 16);
        if (!piece_on(pos + 12) && teamcheck(pos + 20, G))
            ret.emplace_back(pos, pos + 20);
    }
    if (pos / 9 == 4 || pos / 9 == 9 || pos / 9 == 7 || pos / 9 == 3) {
        if (!piece_on(pos - 10) && teamcheck(pos - 16, G))
            ret.emplace_back(pos, pos - 16);
        if (!piece_on(pos - 12) && teamcheck(pos - 20, G))
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
        if (pos % 9 != 0 && teamcheck(pos - 19, G)) {
            ret.emplace_back(pos, pos - 19);
        }
        if (pos % 9 != 8 && teamcheck(pos - 17, G)) {
            ret.emplace_back(pos, pos - 17);
        }
    }
    if (pos < 72 && !piece_on(pos + 9)) {
        if (pos % 9 != 8 && teamcheck(pos + 19, G)) {
            ret.emplace_back(pos, pos + 19);
        }
        if (pos % 9 != 0 && teamcheck(pos + 17, G)) {
            ret.emplace_back(pos, pos + 17);
        }
    }
    if (pos % 9 > 1 && !piece_on(pos - 1)) {
        if (pos / 9 != 0 && teamcheck(pos - 11, G)) {
            ret.emplace_back(pos, pos - 11);
        }
        if (pos / 9 != 9 && teamcheck(pos + 7, G)) {
            ret.emplace_back(pos, pos + 7);
        }
    }
    if (pos % 9 < 7 && !piece_on(pos + 1)) {
        if (pos / 9 != 9 && teamcheck(pos + 11, G)) {
            ret.emplace_back(pos, pos + 11);
        }
        if (pos / 9 != 0 && teamcheck(pos - 7, G)) {
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
    if (G) {
        if (teamcheck(left, G))
            ret.emplace_back(pos, left);
        if (teamcheck(right, G))
            ret.emplace_back(pos, right);
        if (teamcheck(top, G))
            ret.emplace_back(pos, top);
        if (teamcheck(bottom, G))
            ret.emplace_back(pos, bottom);
    } else {
        for (int p = int(pos) - 1; p > left; --p)
            ret.emplace_back(pos, POS(p));
        if (!piece_on(left))
            ret.emplace_back(pos, left);
        for (int p = int(pos) + 1; p < right; ++p)
            ret.emplace_back(pos, POS(p));
        if (!piece_on(right))
            ret.emplace_back(pos, right);
        for (int p = int(pos) - 9; p > top; p -= 9)
            ret.emplace_back(pos, POS(p));
        if (!piece_on(top))
            ret.emplace_back(pos, top);
        for (int p = int(pos) + 9; p < bottom; p += 9)
            ret.emplace_back(pos, POS(p));
        if (!piece_on(bottom))
            ret.emplace_back(pos, bottom);
    }
    return ret;
}

// cannon moves
template <bool G>
std::vector<Move> gen_cannon_(POS pos)
{
    if (G) {
        std::vector<Move> ret { };
        ret.reserve(4);
        const auto bl9 = get_bl9(pos), bl10 = get_bl10(pos);
        const auto [left, right] = cannon_9(bl9, pos);
        const auto [top, bottom] = cannon_10(bl10, pos);
        if (left < INVALID_POS && teamcheck(left, G))
            ret.emplace_back(pos, left);
        if (right < INVALID_POS && teamcheck(right, G))
            ret.emplace_back(pos, right);
        if (top < INVALID_POS && teamcheck(top, G))
            ret.emplace_back(pos, top);
        if (bottom < INVALID_POS && teamcheck(bottom, G))
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
    if (0 <= target && target < 90 && teamcheck(target, G))
        ret.emplace_back(pos, target);
    if ((pos / 9 < 5 && g_team == R) || (pos / 9 > 4 && g_team == B)) {
        if (pos % 9 != 0 && teamcheck(pos - 1, G))
            ret.emplace_back(pos, pos - 1);
        if (pos % 9 != 8 && teamcheck(pos + 1, G))
            ret.emplace_back(pos, pos + 1);
    }
    return ret;
}

// generate all capture moves
std::vector<Move> gen_all_capture_moves()
{
    std::vector<Move> ret { };
    ret.reserve(16);
    for (const POS p : get_pos_list()) {
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
    for (const POS p : get_pos_list()) {
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

// generate all moves
// TODO: implement a high-performance gen_all_moves
std::vector<Move> gen_all_moves()
{
    std::vector<Move> quiets = gen_all_quiet_moves();
    std::vector<Move> captures = gen_all_capture_moves();
    quiets.insert(quiets.end(), captures.begin(), captures.end());
    return quiets;
}

// the move picker to generate moves step-by-step
class MovePicker {
    DEPTH depth { 0 };
    MovePickerStatus status = STATUS_TT;
    Move tt_move { }; // regester tt move
    std::array<Move, 2> killers { }; // and killer moves
    std::vector<Move> moves { };
    std::vector<Move> bad_captures { };
    bool generated = false; // 0 is false, others are true
    std::uint8_t i { 0 }; // the index

public:
    MovePicker(DEPTH depth) : depth(depth) { }
    Move next();
};

// get the next move in move picker
Move MovePicker::next()
{
    if (status == STATUS_TT) {
        tt_move = tt_get_move(), status++;
        if (!tt_move || abs(piece_on(tt_move.end)) == R_KING) return next();
        return tt_move;
    } else if (status == STATUS_GOOD_CAPTURES) {
        if (!generated) {
            moves = gen_all_capture_moves();
            mvvlva_sort(moves);
            generated = true;
        }
        if (i < moves.size()) {
            Move m = moves[i++];
            if (m == tt_move) return next();
            if (abs(piece_on(m.end)) == R_KING) return next();
            if (see_ge(m, 0)) return m;
            bad_captures.emplace_back(m);
            return next();
        } else {
            generated = i = 0, status++;
            return next();
        }
    } else if (status == STATUS_KILLER) {
        if (!generated) {
            killers = killer_get(depth), generated = true;
        }
        if (i < killers.size()) {
            if (legal_move(killers[i++]) && abs(piece_on(killers[size_t(i - 1)].end)) != R_KING) {
                return killers[size_t(i - 1)];
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
            const bool skip = m == tt_move || m == killers[0] || m == killers[1];
            return skip ? next() : m;
        } else {
            generated = i = 0, status++;
            return next();
        }
    }
    return i < bad_captures.size() ? bad_captures[i++] : Move { };
}
