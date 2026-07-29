#pragma once
#include "heuristic.hpp"
#include "position.hpp"

// king moves
template <GENTYPE G>
std::vector<Move> gen_king_(POS pos)
{
    std::vector<Move> ret { };
    ret.reserve(4);
    if ((2 < pos && pos < 15 || 65 < pos && pos < 78) && targetchk<G>(pos + 9))
        ret.emplace_back(pos, pos + 9);
    if ((11 < pos && pos < 24 || 74 < pos && pos < 87) && targetchk<G>(pos - 9))
        ret.emplace_back(pos, pos - 9);
    if ((pos % 9 == 4 || pos % 9 == 5) && targetchk<G>(pos - 1))
        ret.emplace_back(pos, pos - 1);
    if ((pos % 9 == 4 || pos % 9 == 3) && targetchk<G>(pos + 1))
        ret.emplace_back(pos, pos + 1);
    return ret;
}

// advisor moves
template <GENTYPE G>
std::vector<Move> gen_advisor_(POS pos)
{
    if (pos == 13 || pos == 76) { // center
        std::vector<Move> ret { };
        ret.reserve(4);
        if (targetchk<G>(pos - 10))
            ret.emplace_back(pos, pos - 10);
        if (targetchk<G>(pos - 8))
            ret.emplace_back(pos, pos - 8);
        if (targetchk<G>(pos + 10))
            ret.emplace_back(pos, pos + 10);
        if (targetchk<G>(pos + 8))
            ret.emplace_back(pos, pos + 8);
        return ret;
    } else if (pos < 24 && targetchk<G>(13)) { // black corner
        return { Move(pos, 13) };
    } else if (pos > 65 && targetchk<G>(76)) { // red corner
        return { Move(pos, 76) };
    }
    return { };
}

// bishop moves
template <GENTYPE G>
std::vector<Move> gen_bishop_(POS pos)
{
    std::vector<Move> ret { };
    ret.reserve(4);
    if (pos / 9 == 0 || pos / 9 == 5 || pos / 9 == 7 || pos / 9 == 3) {
        if (!piece_on(pos + 10) && targetchk<G>(pos + 16))
            ret.emplace_back(pos, pos + 16);
        if (!piece_on(pos + 12) && targetchk<G>(pos + 20))
            ret.emplace_back(pos, pos + 20);
    }
    if (pos / 9 == 4 || pos / 9 == 9 || pos / 9 == 7 || pos / 9 == 3) {
        if (!piece_on(pos - 10) && targetchk<G>(pos - 16))
            ret.emplace_back(pos, pos - 16);
        if (!piece_on(pos - 12) && targetchk<G>(pos - 20))
            ret.emplace_back(pos, pos - 20);
    }
    return ret;
}

// knight moves
template <GENTYPE G>
std::vector<Move> gen_knight_(POS pos)
{
    std::vector<Move> ret { };
    ret.reserve(8);
    if (pos > 17 && !piece_on(pos - 9)) {
        if (pos % 9 != 0 && targetchk<G>(pos - 19)) {
            ret.emplace_back(pos, pos - 19);
        }
        if (pos % 9 != 8 && targetchk<G>(pos - 17)) {
            ret.emplace_back(pos, pos - 17);
        }
    }
    if (pos < 72 && !piece_on(pos + 9)) {
        if (pos % 9 != 8 && targetchk<G>(pos + 19)) {
            ret.emplace_back(pos, pos + 19);
        }
        if (pos % 9 != 0 && targetchk<G>(pos + 17)) {
            ret.emplace_back(pos, pos + 17);
        }
    }
    if (pos % 9 > 1 && !piece_on(pos - 1)) {
        if (pos / 9 != 0 && targetchk<G>(pos - 11)) {
            ret.emplace_back(pos, pos - 11);
        }
        if (pos / 9 != 9 && targetchk<G>(pos + 7)) {
            ret.emplace_back(pos, pos + 7);
        }
    }
    if (pos % 9 < 7 && !piece_on(pos + 1)) {
        if (pos / 9 != 9 && targetchk<G>(pos + 11)) {
            ret.emplace_back(pos, pos + 11);
        }
        if (pos / 9 != 0 && targetchk<G>(pos - 7)) {
            ret.emplace_back(pos, pos - 7);
        }
    }
    return ret;
}

// rook moves
template <GENTYPE G>
std::vector<Move> gen_rook_(POS pos)
{
    std::vector<Move> ret { };
    ret.reserve(24);
    const auto [left, right] = rook_9(get_bl9(pos), pos);
    const auto [top, bottom] = rook_10(get_bl10(pos), pos);
    if constexpr (G != CAPTURE) {
        for (int p = int(pos) - 1; p > int(left); --p)
            ret.emplace_back(pos, POS(p));
        if (!piece_on(left)) ret.emplace_back(pos, left);

        for (int p = int(pos) + 1; p < int(right); ++p)
            ret.emplace_back(pos, POS(p));
        if (!piece_on(right)) ret.emplace_back(pos, right);

        for (int p = int(pos) - 9; p > int(top); p -= 9)
            ret.emplace_back(pos, POS(p));
        if (!piece_on(top)) ret.emplace_back(pos, top);

        for (int p = int(pos) + 9; p < int(bottom); p += 9)
            ret.emplace_back(pos, POS(p));
        if (!piece_on(bottom)) ret.emplace_back(pos, bottom);
    }
    if constexpr (G != QUIET) {
        if (targetchk<CAPTURE>(left)) ret.emplace_back(pos, left);
        if (targetchk<CAPTURE>(right)) ret.emplace_back(pos, right);
        if (targetchk<CAPTURE>(top)) ret.emplace_back(pos, top);
        if (targetchk<CAPTURE>(bottom)) ret.emplace_back(pos, bottom);
    }
    return ret;
}

// cannon moves
template <GENTYPE G>
std::vector<Move> gen_cannon_(POS pos)
{
    if constexpr (G == QUIET) {
        return gen_rook_<QUIET>(pos);
    }
    std::vector<Move> ret { };
    ret.reserve(4);
    const auto bl9 = get_bl9(pos), bl10 = get_bl10(pos);
    const auto [left, right] = cannon_9(bl9, pos);
    const auto [top, bottom] = cannon_10(bl10, pos);
    if (left < INVALID_POS && targetchk<CAPTURE>(left))
        ret.emplace_back(pos, left);
    if (right < INVALID_POS && targetchk<CAPTURE>(right))
        ret.emplace_back(pos, right);
    if (top < INVALID_POS && targetchk<CAPTURE>(top))
        ret.emplace_back(pos, top);
    if (bottom < INVALID_POS && targetchk<CAPTURE>(bottom))
        ret.emplace_back(pos, bottom);
    if constexpr (G == ALL) {
        const auto quiets = gen_rook_<QUIET>(pos);
        ret.insert(ret.end(), quiets.begin(), quiets.end());
    }
    return ret;
}

// pawn moves
template <GENTYPE G>
std::vector<Move> gen_pawn_(POS pos)
{
    std::vector<Move> ret { };
    ret.reserve(3);
    const int target = pos - 9 * g_team;
    if (0 <= target && target < 90 && targetchk<G>(target))
        ret.emplace_back(pos, target);
    if ((pos / 9 < 5 && g_team == R) || (pos / 9 > 4 && g_team == B)) {
        if (pos % 9 != 0 && targetchk<G>(pos - 1))
            ret.emplace_back(pos, pos - 1);
        if (pos % 9 != 8 && targetchk<G>(pos + 1))
            ret.emplace_back(pos, pos + 1);
    }
    return ret;
}

// gen moves
template <GENTYPE G>
std::vector<Move> gen_moves_()
{
    std::vector<Move> ret { };
    ret.reserve(16);
    for (const POS p : get_pos_list()) {
        const PTYPE t = std::abs(piece_on(p));
        if (t == R_KING) {
            const auto moves = gen_king_<G>(p);
            ret.insert(ret.end(), moves.begin(), moves.end());
        } else if (t == R_ADVISOR) {
            const auto moves = gen_advisor_<G>(p);
            ret.insert(ret.end(), moves.begin(), moves.end());
        } else if (t == R_BISHOP) {
            const auto moves = gen_bishop_<G>(p);
            ret.insert(ret.end(), moves.begin(), moves.end());
        } else if (t == R_KNIGHT) {
            const auto moves = gen_knight_<G>(p);
            ret.insert(ret.end(), moves.begin(), moves.end());
        } else if (t == R_ROOK) {
            const auto moves = gen_rook_<G>(p);
            ret.insert(ret.end(), moves.begin(), moves.end());
        } else if (t == R_CANNON) {
            const auto moves = gen_cannon_<G>(p);
            ret.insert(ret.end(), moves.begin(), moves.end());
        } else if (t == R_PAWN) {
            const auto moves = gen_pawn_<G>(p);
            ret.insert(ret.end(), moves.begin(), moves.end());
        }
    }
    return ret;
}

// generate all capture moves
std::vector<Move> gen_all_capture_moves()
{
    return gen_moves_<CAPTURE>();
}

// generate all quiet moves
std::vector<Move> gen_all_quiet_moves()
{
    return gen_moves_<QUIET>();
}

// generate all moves
std::vector<Move> gen_all_moves()
{
    return gen_moves_<ALL>();
}

// move picker
class MovePicker {
    DEPTH depth { 0 };
    MovePickerStatus status { STATUS_TT };
    Move tt_move { };
    std::array<Move, 2> killers { };
    std::vector<Move> moves { };
    std::size_t i { 0 };
    bool generated { false };

public:
    MovePicker(DEPTH d) : depth(d) { }
    Move next();
};

// get next move
Move MovePicker::next()
{
    if (status == STATUS_TT) {
        tt_move = tt_get_move();
        status = STATUS_KILLER;
        if (tt_move && legal_move(tt_move)) return tt_move;
        return next();
    } else if (status == STATUS_KILLER) {
        if (!generated) {
            killers = killer_get(depth);
            generated = true;
            i = 0;
        }
        while (i < killers.size()) {
            Move m = killers[i++];
            if (!m || m == tt_move) continue;
            if (piece_on(m.end)) continue;
            if (!legal_move(m)) continue;
            return m;
        }
        generated = false;
        i = 0;
        status = STATUS_MOVES;
        return next();
    } else if (status == STATUS_MOVES) {
        if (!generated) {
            moves = gen_all_capture_moves();
            mvvlva_sort(moves);
            std::vector<Move> quiets = gen_all_quiet_moves();
            history_sort(quiets, g_team);
            moves.insert(moves.end(), quiets.begin(), quiets.end());
            generated = true;
            i = 0;
        }
        while (i < moves.size()) {
            Move m = moves[i++];
            if (m == tt_move) continue;
            if (!piece_on(m.end) && (m == killers[0] || m == killers[1]))
                continue;
            return m;
        }
        return Move { };
    }
    return Move { };
}
