#pragma once
#include "position.hpp"

// king moves
std::vector<Move> gen_king_(POS pos)
{
    std::vector<Move> ret { };
    ret.reserve(4);
    if ((2 < pos && pos < 15 || 65 < pos && pos < 78) && !opposite(pos + 9))
        ret.emplace_back(pos, pos + 9);
    if ((11 < pos && pos < 24 || 74 < pos && pos < 87) && !opposite(pos - 9))
        ret.emplace_back(pos, pos - 9);
    if (pos % 9 == 4 && !opposite(pos - 1)) ret.emplace_back(pos, pos - 1);
    if (pos % 9 == 4 && !opposite(pos + 1)) ret.emplace_back(pos, pos + 1);
    return ret;
}

// advisor moves
std::vector<Move> gen_advisor_(POS pos)
{
    if (pos == 13 || pos == 76) { // black center
        std::vector<Move> ret { };
        ret.reserve(4);
        if (!opposite(pos - 10)) ret.emplace_back(pos, pos - 10);
        if (!opposite(pos - 8)) ret.emplace_back(pos, pos - 8);
        if (!opposite(pos + 10)) ret.emplace_back(pos, pos + 10);
        if (!opposite(pos + 8)) ret.emplace_back(pos, pos + 8);
        return ret;
    } else if (pos < 24 && !opposite(13)) { // black corner
        return { Move(pos, 13) };
    } else if (!opposite(76)) { // red corner
        return { Move(pos, 76) };
    }
    return { };
}

// bishop moves
std::vector<Move> gen_bishop_(POS pos)
{
    std::vector<Move> ret { };
    ret.reserve(4);
    if (pos / 9 == 0 || pos / 9 == 5 || pos / 9 == 7 || pos / 9 == 3) {
        if (!piece_on(pos + 10) && !opposite(pos + 16)) {
            ret.emplace_back(pos, pos + 16);
        }
        if (!piece_on(pos + 12) && !opposite(pos + 20)) {
            ret.emplace_back(pos, pos + 20);
        }
    }
    if (pos / 9 == 4 || pos / 9 == 9 || pos / 9 == 7 || pos / 9 == 3) {
        if (!piece_on(pos - 10) && !opposite(pos - 16)) {
            ret.emplace_back(pos, pos - 16);
        }
        if (!piece_on(pos - 12) && !opposite(pos - 20)) {
            ret.emplace_back(pos, pos - 20);
        }
    }
    return ret;
}

// knight moves
template <bool GEN_CAPTURE>
std::vector<Move> gen_knight_(POS pos)
{
    std::vector<Move> ret { };
    ret.reserve(8);
    if (pos > 17 && !piece_on(pos - 9)) {
        if (pos % 9 != 0 && team_diff<GEN_CAPTURE>(pos - 19)) {
            ret.emplace_back(pos, pos - 19);
        }
        if (pos % 9 != 8 && team_diff<GEN_CAPTURE>(pos - 17)) {
            ret.emplace_back(pos, pos - 17);
        }
    }
    if (pos < 72 && !piece_on(pos + 9)) {
        if (pos % 9 != 0 && team_diff<GEN_CAPTURE>(pos + 19)) {
            ret.emplace_back(pos, pos + 19);
        }
        if (pos % 9 != 8 && team_diff<GEN_CAPTURE>(pos + 17)) {
            ret.emplace_back(pos, pos + 17);
        }
    }
    if (pos % 9 > 1 && !piece_on(pos - 1)) {
        if (pos / 9 != 0 && team_diff<GEN_CAPTURE>(pos - 11)) {
            ret.emplace_back(pos, pos - 11);
        }
        if (pos / 9 != 9 && team_diff<GEN_CAPTURE>(pos + 7)) {
            ret.emplace_back(pos, pos + 7);
        }
    }
    if (pos % 9 < 7 && !piece_on(pos + 1)) {
        if (pos / 9 != 0 && team_diff<GEN_CAPTURE>(pos + 11)) {
            ret.emplace_back(pos, pos + 11);
        }
        if (pos / 9 != 9 && team_diff<GEN_CAPTURE>(pos - 7)) {
            ret.emplace_back(pos, pos - 7);
        }
    }
    return ret;
}

// rook moves (legacy)
template <bool GEN_CAPTURE>
std::vector<Move> gen_rook_legacy_(POS pos)
{
    std::vector<Move> ret { };
    ret.reserve(GEN_CAPTURE ? 4 : 17);
    for (int p = pos - 9; p >= 0; p -= 9) {
        if (team_diff<GEN_CAPTURE>(p)) {
            ret.emplace_back(pos, p);
        }
        if (piece_on(p)) {
            break;
        }
    }
    for (int p = pos + 9; p < 90; p += 9) {
        if (team_diff<GEN_CAPTURE>(p)) {
            ret.emplace_back(pos, p);
        }
        if (piece_on(p)) {
            break;
        }
    }
    for (int p = pos + 1; p / 9 == pos / 9; p += 1) {
        if (team_diff<GEN_CAPTURE>(p)) {
            ret.emplace_back(pos, p);
        }
        if (piece_on(p)) {
            break;
        }
    }
    for (int p = pos - 1; p / 9 == pos / 9; p -= 1) {
        if (team_diff<GEN_CAPTURE>(p)) {
            ret.emplace_back(pos, p);
        }
        if (piece_on(p)) {
            break;
        }
    }
    return ret;
}

// cannon moves (legacy)
template <bool GEN_CAPTURE>
std::vector<Move> gen_cannon_legacy_(POS pos)
{
    if constexpr (GEN_CAPTURE) {
        std::vector<Move> ret { };
        ret.reserve(GEN_CAPTURE ? 4 : 17);
        bool t = false;
        for (int p = pos - 9; p >= 0; p -= 9) {
            if (t == false && piece_on(p)) {
                t = true;
                continue;
            } else if (t == true && piece_on(p)) {
                if (opposite(p)) ret.emplace_back(pos, p);
            }
        }
        for (int p = pos + 9; p < 90; p += 9) {
            if (t == false && piece_on(p)) {
                t = true;
                continue;
            } else if (t == true && piece_on(p)) {
                if (opposite(p)) ret.emplace_back(pos, p);
            }
        }
        for (int p = pos + 1; p / 9 == pos / 9; p += 1) {
            if (t == false && piece_on(p)) {
                t = true;
                continue;
            } else if (t == true && piece_on(p)) {
                if (opposite(p)) ret.emplace_back(pos, p);
            }
        }
        for (int p = pos - 1; p / 9 == pos / 9; p -= 1) {
            if (t == false && piece_on(p)) {
                t = true;
                continue;
            } else if (t == true && piece_on(p)) {
                if (opposite(p)) ret.emplace_back(pos, p);
            }
        }
        return ret;
    } else {
        return gen_rook_legacy_<false>(pos);
    }
}

// rook moves (bit)
template <bool GEN_CAPTURE>
std::vector<Move> gen_rook_bit_(POS pos)
{
    std::vector<Move> ret { };
    ret.reserve(4);
    const auto bl9 = get_bl9(pos), bl10 = get_bl10(pos);
    const auto [left, right] = rook_9(bl9, pos);
    const auto [top, bottom] = rook_10(bl10, pos);
    if constexpr (GEN_CAPTURE) {
        if (opposite(left))
            ret.emplace_back(pos, left);
        if (opposite(right))
            ret.emplace_back(pos, right);
        if (opposite(top))
            ret.emplace_back(pos, top);
        if (opposite(bottom))
            ret.emplace_back(pos, bottom);
    } else {
        for (POS p = pos - 1; p > left; p--)
            ret.emplace_back(pos, p);
        for (POS p = pos + 1; p < right; p++)
            ret.emplace_back(pos, p);
        for (POS p = pos - 9; p > top; p -= 9)
            ret.emplace_back(pos, p);
        for (POS p = pos + 9; p < bottom; p += 9)
            ret.emplace_back(pos, p);
        if (!piece_on(top))
            ret.emplace_back(pos, top);
        if (!piece_on(right))
            ret.emplace_back(pos, right);
        if (!piece_on(left))
            ret.emplace_back(pos, left);
        if (!piece_on(bottom))
            ret.emplace_back(pos, bottom);
    }
    return ret;
}

// cannon moves (bit)
template <bool GEN_CAPTURE>
std::vector<Move> gen_cannon_bit_(POS pos)
{
    if constexpr (GEN_CAPTURE) {
        std::vector<Move> ret { };
        ret.reserve(4);
        const auto bl9 = get_bl9(pos), bl10 = get_bl10(pos);
        const auto [left, right] = cannon_9(bl9, pos);
        const auto [top, bottom] = cannon_10(bl10, pos);
        if (team_diff<GEN_CAPTURE>(left))
            ret.emplace_back(pos, left);
        if (team_diff<GEN_CAPTURE>(right))
            ret.emplace_back(pos, right);
        if (team_diff<GEN_CAPTURE>(top))
            ret.emplace_back(pos, top);
        if (team_diff<GEN_CAPTURE>(bottom))
            ret.emplace_back(pos, bottom);
        return ret;
    } else {
        return gen_rook_bit_<false>(pos);
    }
}

// pawn moves
template <bool GEN_CAPTURE>
std::vector<Move> gen_pawn_(POS pos)
{
    std::vector<Move> ret { };
    ret.reserve(3);
    const int target = pos - 9 * g_team;
    if (0 <= target && target < 90 && team_diff<GEN_CAPTURE>(target)) {
        ret.emplace_back(pos, target);
    }
    if ((pos / 9 < 5 && g_team == R) || (pos / 9 > 4 && g_team == B)) {
        if (pos % 9 != 0 && team_diff<GEN_CAPTURE>(pos - 1)) {
            ret.emplace_back(pos, pos - 1);
        }
        if (pos % 9 != 8 && team_diff<GEN_CAPTURE>(pos + 1)) {
            ret.emplace_back(pos, pos + 1);
        }
    }
    return ret;
}
