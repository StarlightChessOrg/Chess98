#pragma once
#include "position.hpp"

// king moves
template <bool G>
std::vector<Move> gen_king_(POS pos)
{
    std::vector<Move> ret {};
    ret.reserve(4);
    if ((2 < pos && pos < 15 || 65 < pos && pos < 78) && team_diff<G>(pos + 9))
        ret.emplace_back(pos, pos + 9);
    if ((11 < pos && pos < 24 || 74 < pos && pos < 87) && team_diff<G>(pos - 9))
        ret.emplace_back(pos, pos - 9);
    if ((pos % 9 == 4 || pos % 9 == 5) && team_diff<G>(pos - 1))
        ret.emplace_back(pos, pos - 1);
    if ((pos % 9 == 4 || pos % 9 == 3) && team_diff<G>(pos + 1))
        ret.emplace_back(pos, pos + 1);
    return ret;
}

// advisor moves
template <bool G>
std::vector<Move> gen_advisor_(POS pos)
{
    if (pos == 13 || pos == 76) { // black center
        std::vector<Move> ret {};
        ret.reserve(4);
        if (team_diff<G>(pos - 10)) ret.emplace_back(pos, pos - 10);
        if (team_diff<G>(pos - 8)) ret.emplace_back(pos, pos - 8);
        if (team_diff<G>(pos + 10)) ret.emplace_back(pos, pos + 10);
        if (team_diff<G>(pos + 8)) ret.emplace_back(pos, pos + 8);
        return ret;
    } else if (pos < 24 && team_diff<G>(13)) { // black corner
        return { Move(pos, 13) };
    } else if (team_diff<G>(76)) { // red corner
        return { Move(pos, 76) };
    }
    return {};
}

// bishop moves
template <bool G>
std::vector<Move> gen_bishop_(POS pos)
{
    std::vector<Move> ret {};
    ret.reserve(4);
    if (pos / 9 == 0 || pos / 9 == 5 || pos / 9 == 7 || pos / 9 == 3) {
        if (!piece_on(pos + 10) && team_diff<G>(pos + 16))
            ret.emplace_back(pos, pos + 16);
        if (!piece_on(pos + 12) && team_diff<G>(pos + 20))
            ret.emplace_back(pos, pos + 20);
    }
    if (pos / 9 == 4 || pos / 9 == 9 || pos / 9 == 7 || pos / 9 == 3) {
        if (!piece_on(pos - 10) && team_diff<G>(pos - 16))
            ret.emplace_back(pos, pos - 16);
        if (!piece_on(pos - 12) && team_diff<G>(pos - 20))
            ret.emplace_back(pos, pos - 20);
    }
    return ret;
}

// knight moves
template <bool G>
std::vector<Move> gen_knight_(POS pos)
{
    std::vector<Move> ret {};
    ret.reserve(8);
    if (pos > 17 && !piece_on(pos - 9)) {
        if (pos % 9 != 0 && team_diff<G>(pos - 19)) {
            ret.emplace_back(pos, pos - 19);
        }
        if (pos % 9 != 8 && team_diff<G>(pos - 17)) {
            ret.emplace_back(pos, pos - 17);
        }
    }
    if (pos < 72 && !piece_on(pos + 9)) {
        if (pos % 9 != 8 && team_diff<G>(pos + 19)) {
            ret.emplace_back(pos, pos + 19);
        }
        if (pos % 9 != 0 && team_diff<G>(pos + 17)) {
            ret.emplace_back(pos, pos + 17);
        }
    }
    if (pos % 9 > 1 && !piece_on(pos - 1)) {
        if (pos / 9 != 0 && team_diff<G>(pos - 11)) {
            ret.emplace_back(pos, pos - 11);
        }
        if (pos / 9 != 9 && team_diff<G>(pos + 7)) {
            ret.emplace_back(pos, pos + 7);
        }
    }
    if (pos % 9 < 7 && !piece_on(pos + 1)) {
        if (pos / 9 != 9 && team_diff<G>(pos + 11)) {
            ret.emplace_back(pos, pos + 11);
        }
        if (pos / 9 != 0 && team_diff<G>(pos - 7)) {
            ret.emplace_back(pos, pos - 7);
        }
    }
    return ret;
}

// rook moves (legacy)
template <bool G>
std::vector<Move> gen_rook_legacy_(POS pos)
{
    std::vector<Move> ret {};
    ret.reserve(G ? 4 : 17);
    for (int p = pos - 9; p >= 0; p -= 9) {
        if (team_diff<G>(p)) ret.emplace_back(pos, p);
        if (piece_on(p)) break;
    }
    for (int p = pos + 9; p < 90; p += 9) {
        if (team_diff<G>(p)) ret.emplace_back(pos, p);
        if (piece_on(p)) break;
    }
    for (int p = pos + 1; p / 9 == pos / 9; p += 1) {
        if (team_diff<G>(p)) ret.emplace_back(pos, p);
        if (piece_on(p)) break;
    }
    for (int p = pos - 1; p / 9 == pos / 9; p -= 1) {
        if (team_diff<G>(p)) ret.emplace_back(pos, p);
        if (piece_on(p)) break;
    }
    return ret;
}

// cannon moves (legacy)
template <bool G>
std::vector<Move> gen_cannon_legacy_(POS pos)
{
    if constexpr (G) {
        std::vector<Move> ret {};
        ret.reserve(G ? 4 : 17);
        bool t = false;
        for (int p = pos - 9; p >= 0; p -= 9) {
            if (t == false && piece_on(p)) {
                t = true;
                continue;
            } else if (t == true && piece_on(p)) {
                if (team_diff<G>(p)) ret.emplace_back(pos, p);
                break;
            }
        }
        for (int p = pos + 9; p < 90; p += 9) {
            if (t == false && piece_on(p)) {
                t = true;
                continue;
            } else if (t == true && piece_on(p)) {
                if (team_diff<G>(p)) ret.emplace_back(pos, p);
                break;
            }
        }
        for (int p = pos + 1; p / 9 == pos / 9; p += 1) {
            if (t == false && piece_on(p)) {
                t = true;
                continue;
            } else if (t == true && piece_on(p)) {
                if (team_diff<G>(p)) ret.emplace_back(pos, p);
                break;
            }
        }
        for (int p = pos - 1; p / 9 == pos / 9; p -= 1) {
            if (t == false && piece_on(p)) {
                t = true;
                continue;
            } else if (t == true && piece_on(p)) {
                if (team_diff<G>(p)) ret.emplace_back(pos, p);
                break;
            }
        }
        return ret;
    }
    return gen_rook_legacy_<false>(pos);
}

// rook moves (bit)
template <bool G>
std::vector<Move> gen_rook_bit_(POS pos)
{
    std::vector<Move> ret {};
    ret.reserve(4);
    const auto bl9 = get_bl9(pos), bl10 = get_bl10(pos);
    const auto [left, right] = rook_9(bl9, pos);
    const auto [top, bottom] = rook_10(bl10, pos);
    if constexpr (G) {
        if (team_diff<G>(left)) ret.emplace_back(pos, left);
        if (team_diff<G>(right)) ret.emplace_back(pos, right);
        if (team_diff<G>(top)) ret.emplace_back(pos, top);
        if (team_diff<G>(bottom)) ret.emplace_back(pos, bottom);
    } else {
        for (POS p = pos - 1; p > left; p--) ret.emplace_back(pos, p);
        if (!piece_on(top)) ret.emplace_back(pos, top);
        for (POS p = pos + 1; p < right; p++) ret.emplace_back(pos, p);
        if (!piece_on(right)) ret.emplace_back(pos, right);
        for (POS p = pos - 9; p > top; p -= 9) ret.emplace_back(pos, p);
        if (!piece_on(left)) ret.emplace_back(pos, left);
        for (POS p = pos + 9; p < bottom; p += 9) ret.emplace_back(pos, p);
        if (!piece_on(bottom)) ret.emplace_back(pos, bottom);
    }
    return ret;
}

// cannon moves (bit)
template <bool G>
std::vector<Move> gen_cannon_bit_(POS pos)
{
    if constexpr (G) {
        std::vector<Move> ret {};
        ret.reserve(4);
        const auto bl9 = get_bl9(pos), bl10 = get_bl10(pos);
        const auto [left, right] = cannon_9(bl9, pos);
        const auto [top, bottom] = cannon_10(bl10, pos);
        if (left < INVALID_POS && team_diff<G>(left))
            ret.emplace_back(pos, left);
        if (right < INVALID_POS && team_diff<G>(right))
            ret.emplace_back(pos, right);
        if (top < INVALID_POS && team_diff<G>(top))
            ret.emplace_back(pos, top);
        if (bottom < INVALID_POS && team_diff<G>(bottom))
            ret.emplace_back(pos, bottom);
        return ret;
    }
    return gen_rook_bit_<false>(pos);
}

// pawn moves
template <bool G>
std::vector<Move> gen_pawn_(POS pos)
{
    std::vector<Move> ret {};
    ret.reserve(3);
    const int target = pos - 9 * g_team;
    if (0 <= target && target < 90 && team_diff<G>(target))
        ret.emplace_back(pos, target);
    if ((pos / 9 < 5 && g_team == R) || (pos / 9 > 4 && g_team == B)) {
        if (pos % 9 != 0 && team_diff<G>(pos - 1))
            ret.emplace_back(pos, pos - 1);
        if (pos % 9 != 8 && team_diff<G>(pos + 1))
            ret.emplace_back(pos, pos + 1);
    }
    return ret;
}

// generate all capture moves
std::vector<Move> gen_all_capture_moves()
{
    std::vector<Move> ret {};
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
            const auto moves = gen_rook_bit_<true>(p);
            ret.insert(ret.end(), moves.begin(), moves.end());
        } else if (t == R_CANNON) {
            const auto moves = gen_cannon_bit_<true>(p);
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
    std::vector<Move> ret {};
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
            const auto moves = gen_rook_bit_<false>(p);
            ret.insert(ret.end(), moves.begin(), moves.end());
        } else if (t == R_CANNON) {
            const auto moves = gen_cannon_bit_<false>(p);
            ret.insert(ret.end(), moves.begin(), moves.end());
        } else if (t == R_PAWN) {
            const auto moves = gen_pawn_<false>(p);
            ret.insert(ret.end(), moves.begin(), moves.end());
        }
    }
    return ret;
}
