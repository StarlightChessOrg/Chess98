#pragma once
#include "position.hpp"

// king moves
std::vector<Move> gen_king_(POS pos)
{
    std::vector<Move> ret {};
    ret.reserve(4);
    const bool condition1 = (2 < pos && pos < 15) || (65 < pos && pos < 78);
    if (condition1 && no_same_team(pos + 9)) {
        ret.emplace_back(pos, pos + 9);
    }
    const bool condition2 = (11 < pos && pos < 24) || (74 < pos && pos < 87);
    if (condition2 && no_same_team(pos - 9)) {
        ret.emplace_back(pos, pos - 9);
    }
    if (pos % 9 == 4 && no_same_team(pos - 1)) {
        ret.emplace_back(pos, pos - 1);
    }
    if (pos % 9 == 4 && no_same_team(pos + 1)) {
        ret.emplace_back(pos, pos + 1);
    }
    return ret;
}

// advisor moves
std::vector<Move> gen_advisor_(POS pos)
{
    if (pos == 13 || pos == 76) { // black center
        std::vector<Move> ret {};
        ret.reserve(4);
        if (no_same_team(pos - 10)) {
            ret.emplace_back(pos, pos - 10);
        }
        if (no_same_team(pos - 8)) {
            ret.emplace_back(pos, pos - 8);
        }
        if (no_same_team(pos + 10)) {
            ret.emplace_back(pos, pos + 10);
        }
        if (no_same_team(pos + 8)) {
            ret.emplace_back(pos, pos + 8);
        }
        return ret;
    } else if (pos < 24 && no_same_team(13)) { // black corner
        return { Move(pos, 13) };
    } else if (no_same_team(76)) { // red corner
        return { Move(pos, 76) };
    }
    return {};
}

// bishop moves
std::vector<Move> gen_bishop_(POS pos)
{
    std::vector<Move> ret {};
    ret.reserve(4);
    if (pos / 9 == 0 || pos / 9 == 5 || pos / 9 == 7 || pos / 9 == 3) {
        if (!piece_on(pos + 10) && no_same_team(pos + 16)) {
            ret.emplace_back(pos, pos + 16);
        }
        if (!piece_on(pos + 12) && no_same_team(pos + 20)) {
            ret.emplace_back(pos, pos + 20);
        }
    }
    if (pos / 9 == 4 || pos / 9 == 9 || pos / 9 == 7 || pos / 9 == 3) {
        if (!piece_on(pos - 10) && no_same_team(pos - 16)) {
            ret.emplace_back(pos, pos - 16);
        }
        if (!piece_on(pos - 12) && no_same_team(pos - 20)) {
            ret.emplace_back(pos, pos - 20);
        }
    }
    return ret;
}

// knight moves
template <bool GEN_CAPTURE>
std::vector<Move> gen_knight_(POS pos)
{
    std::vector<Move> ret {};
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

// rook moves
template <bool GEN_CAPTURE>
std::vector<Move> gen_rook_(POS pos)
{
    std::vector<Move> ret {};
    ret.reserve(17);
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

template <bool GEN_CAPTURE>
std::vector<Move> gen_cannon_(POS pos)
{
    std::vector<Move> ret {};
    ret.reserve(17);
    for (int p = pos - 9; p >= 0; p -= 9) {
        if (!piece_on(p)) {
            if constexpr (!GEN_CAPTURE) {
                ret.emplace_back(pos, p);
            }
        } else {
            for (p -= 9; p >= 0 && !piece_on(p); p -= 9) { }
            if (p >= 0 && opposite(p)) {
                ret.emplace_back(pos, p);
            }
            break;
        }
    }
    for (int p = pos + 9; p < 90; p += 9) {
        if (!piece_on(p)) {
            if constexpr (!GEN_CAPTURE) {
                ret.emplace_back(pos, p);
            }
        } else {
            for (p += 9; p < 90 && !piece_on(p); p += 9) { }
            if (p < 90 && opposite(p)) {
                ret.emplace_back(pos, p);
            }
            break;
        }
    }
    for (int p = pos + 1; p / 9 == pos / 9; p += 1) {
        if (!piece_on(p)) {
            if constexpr (!GEN_CAPTURE) {
                ret.emplace_back(pos, p);
            }
        } else {
            for (p += 1; p / 9 == pos / 9 && !piece_on(p); p += 1) { }
            if (p / 9 == pos / 9 && opposite(p)) {
                ret.emplace_back(pos, p);
            }
            break;
        }
    }
    for (int p = pos - 1; p / 9 == pos / 9; p -= 1) {
        if (!piece_on(p)) {
            if constexpr (!GEN_CAPTURE) {
                ret.emplace_back(pos, p);
            }
        } else {
            for (p -= 1; p / 9 == pos / 9 && !piece_on(p); p -= 1) { }
            if (p / 9 == pos / 9 && opposite(p)) {
                ret.emplace_back(pos, p);
            }
            break;
        }
    }
    return ret;
}

// pawn moves
template <bool GEN_CAPTURE>
std::vector<Move> gen_pawn_(POS pos)
{
    std::vector<Move> ret {};
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
