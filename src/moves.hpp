#pragma once
#include "heuristic.hpp"
#include "position.hpp"

// king moves
template <GENTYPE G>
void gen_king_(POS pos, MoveList& out)
{
    if ((2 < pos && pos < 15 || 65 < pos && pos < 78) && targetchk<G>(pos + 9))
        out.push(Move(pos, pos + 9));
    if ((11 < pos && pos < 24 || 74 < pos && pos < 87) && targetchk<G>(pos - 9))
        out.push(Move(pos, pos - 9));
    if ((pos % 9 == 4 || pos % 9 == 5) && targetchk<G>(pos - 1))
        out.push(Move(pos, pos - 1));
    if ((pos % 9 == 4 || pos % 9 == 3) && targetchk<G>(pos + 1))
        out.push(Move(pos, pos + 1));
}

// advisor moves
template <GENTYPE G>
void gen_advisor_(POS pos, MoveList& out)
{
    if (pos == 13 || pos == 76) { // center
        if (targetchk<G>(pos - 10))
            out.push(Move(pos, pos - 10));
        if (targetchk<G>(pos - 8))
            out.push(Move(pos, pos - 8));
        if (targetchk<G>(pos + 10))
            out.push(Move(pos, pos + 10));
        if (targetchk<G>(pos + 8))
            out.push(Move(pos, pos + 8));
    } else if (pos < 24 && targetchk<G>(13)) { // black corner
        out.push(Move(pos, 13));
    } else if (pos > 65 && targetchk<G>(76)) { // red corner
        out.push(Move(pos, 76));
    }
}

// bishop moves
template <GENTYPE G>
void gen_bishop_(POS pos, MoveList& out)
{
    if (pos / 9 == 0 || pos / 9 == 5 || pos / 9 == 7 || pos / 9 == 3) {
        if (!piece_on(pos + 10) && targetchk<G>(pos + 16))
            out.push(Move(pos, pos + 16));
        if (!piece_on(pos + 12) && targetchk<G>(pos + 20))
            out.push(Move(pos, pos + 20));
    }
    if (pos / 9 == 4 || pos / 9 == 9 || pos / 9 == 7 || pos / 9 == 3) {
        if (!piece_on(pos - 10) && targetchk<G>(pos - 16))
            out.push(Move(pos, pos - 16));
        if (!piece_on(pos - 12) && targetchk<G>(pos - 20))
            out.push(Move(pos, pos - 20));
    }
}

// knight moves
template <GENTYPE G>
void gen_knight_(POS pos, MoveList& out)
{
    if (pos > 17 && !piece_on(pos - 9)) {
        if (pos % 9 != 0 && targetchk<G>(pos - 19)) {
            out.push(Move(pos, pos - 19));
        }
        if (pos % 9 != 8 && targetchk<G>(pos - 17)) {
            out.push(Move(pos, pos - 17));
        }
    }
    if (pos < 72 && !piece_on(pos + 9)) {
        if (pos % 9 != 8 && targetchk<G>(pos + 19)) {
            out.push(Move(pos, pos + 19));
        }
        if (pos % 9 != 0 && targetchk<G>(pos + 17)) {
            out.push(Move(pos, pos + 17));
        }
    }
    if (pos % 9 > 1 && !piece_on(pos - 1)) {
        if (pos / 9 != 0 && targetchk<G>(pos - 11)) {
            out.push(Move(pos, pos - 11));
        }
        if (pos / 9 != 9 && targetchk<G>(pos + 7)) {
            out.push(Move(pos, pos + 7));
        }
    }
    if (pos % 9 < 7 && !piece_on(pos + 1)) {
        if (pos / 9 != 9 && targetchk<G>(pos + 11)) {
            out.push(Move(pos, pos + 11));
        }
        if (pos / 9 != 0 && targetchk<G>(pos - 7)) {
            out.push(Move(pos, pos - 7));
        }
    }
}

// rook moves
template <GENTYPE G>
void gen_rook_(POS pos, MoveList& out)
{
    const auto [left, right] = rook_9(get_bl9(pos), pos);
    const auto [top, bottom] = rook_10(get_bl10(pos), pos);
    if constexpr (G != CAPTURE) {
        for (int p = int(pos) - 1; p > int(left); --p)
            out.push(Move(pos, POS(p)));
        if (!piece_on(left)) out.push(Move(pos, left));

        for (int p = int(pos) + 1; p < int(right); ++p)
            out.push(Move(pos, POS(p)));
        if (!piece_on(right)) out.push(Move(pos, right));

        for (int p = int(pos) - 9; p > int(top); p -= 9)
            out.push(Move(pos, POS(p)));
        if (!piece_on(top)) out.push(Move(pos, top));

        for (int p = int(pos) + 9; p < int(bottom); p += 9)
            out.push(Move(pos, POS(p)));
        if (!piece_on(bottom)) out.push(Move(pos, bottom));
    }
    if constexpr (G != QUIET) {
        if (targetchk<CAPTURE>(left)) out.push(Move(pos, left));
        if (targetchk<CAPTURE>(right)) out.push(Move(pos, right));
        if (targetchk<CAPTURE>(top)) out.push(Move(pos, top));
        if (targetchk<CAPTURE>(bottom)) out.push(Move(pos, bottom));
    }
}

// cannon moves
template <GENTYPE G>
void gen_cannon_(POS pos, MoveList& out)
{
    if constexpr (G == QUIET) {
        gen_rook_<QUIET>(pos, out);
        return;
    }
    const auto bl9 = get_bl9(pos), bl10 = get_bl10(pos);
    const auto [left, right] = cannon_9(bl9, pos);
    const auto [top, bottom] = cannon_10(bl10, pos);
    if (left < INVALID_POS && targetchk<CAPTURE>(left))
        out.push(Move(pos, left));
    if (right < INVALID_POS && targetchk<CAPTURE>(right))
        out.push(Move(pos, right));
    if (top < INVALID_POS && targetchk<CAPTURE>(top))
        out.push(Move(pos, top));
    if (bottom < INVALID_POS && targetchk<CAPTURE>(bottom))
        out.push(Move(pos, bottom));
    if constexpr (G == ALL) {
        gen_rook_<QUIET>(pos, out);
    }
}

// pawn moves
template <GENTYPE G>
void gen_pawn_(POS pos, MoveList& out)
{
    const int target = pos - 9 * g_team;
    if (0 <= target && target < 90 && targetchk<G>(target))
        out.push(Move(pos, target));
    if ((pos / 9 < 5 && g_team == R) || (pos / 9 > 4 && g_team == B)) {
        if (pos % 9 != 0 && targetchk<G>(pos - 1))
            out.push(Move(pos, pos - 1));
        if (pos % 9 != 8 && targetchk<G>(pos + 1))
            out.push(Move(pos, pos + 1));
    }
}

// gen moves
template <GENTYPE G>
void gen_moves_(MoveList& out)
{
    for (const POS p : get_pos_list()) {
        const PTYPE t = std::abs(piece_on(p));
        if (t == R_KING) {
            gen_king_<G>(p, out);
        } else if (t == R_ADVISOR) {
            gen_advisor_<G>(p, out);
        } else if (t == R_BISHOP) {
            gen_bishop_<G>(p, out);
        } else if (t == R_KNIGHT) {
            gen_knight_<G>(p, out);
        } else if (t == R_ROOK) {
            gen_rook_<G>(p, out);
        } else if (t == R_CANNON) {
            gen_cannon_<G>(p, out);
        } else if (t == R_PAWN) {
            gen_pawn_<G>(p, out);
        }
    }
}

void gen_all_capture_moves(MoveList& out)
{
    gen_moves_<CAPTURE>(out);
}

void gen_all_quiet_moves(MoveList& out)
{
    gen_moves_<QUIET>(out);
}

void gen_all_moves(MoveList& out)
{
    gen_moves_<ALL>(out);
}

// move picker
// tt -> killer -> capture -> quiet
class MovePicker {
    std::array<Move, 3> starts { };
    MoveList moves { };
    int i { 0 };

    static void order_rest_(MoveList& moves)
    {
        auto mid = std::partition(moves.begin(), moves.end(), [](Move m) {
            return piece_on(m.end) != 0;
        });
        mvvlva_sort(moves.begin(), mid);
        history_sort(mid, moves.end(), g_team);
    }

public:
    MovePicker(DEPTH depth)
    {
        const auto killers = killer_get(depth);
        starts[0] = tt_get_move();
        starts[1] = killers[0] != starts[0] && legal_move(killers[0]) ? killers[0] : Move { };
        starts[2] = killers[1] != starts[0] && legal_move(killers[1]) ? killers[1] : Move { };
    }

    Move next()
    {
        if (i >= 0 && i < 3) {
            const Move m = starts[size_t(i++)];
            return m ? m : next();
        }
        if (i == 3) {
            moves.i = 0;
            gen_all_moves(moves);
            order_rest_(moves);
            i = -1;
            return next();
        }
        const int idx = -i - 1;
        if (idx < moves.size()) {
            const Move m = moves[idx];
            const bool c = m == starts[0] || m == starts[1] || m == starts[2];
            return (i--, c) ? next() : m;
        }
        return Move { };
    }
};
