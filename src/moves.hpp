#pragma once
#include "heuristic.hpp"
#include "position.hpp"

// king moves
std::vector<Move> gen_king_(POS pos)
{
    std::vector<Move> ret {};
    ret.reserve(4);
    const bool condition1 = (2 < pos && pos < 15) || (65 < pos && pos < 78);
    if (condition1 && diffteam(pos + 9)) {
        ret.emplace_back(pos, pos + 9);
    }
    const bool condition2 = (11 < pos && pos < 24) || (74 < pos && pos < 87);
    if (condition2 && diffteam(pos - 9)) {
        ret.emplace_back(pos, pos - 9);
    }
    if (pos % 9 == 4 && diffteam(pos - 1)) {
        ret.emplace_back(pos, pos - 1);
    }
    if (pos % 9 == 4 && diffteam(pos + 1)) {
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
        if (diffteam(pos - 10)) {
            ret.emplace_back(pos, pos - 10);
        }
        if (diffteam(pos - 8)) {
            ret.emplace_back(pos, pos - 8);
        }
        if (diffteam(pos + 10)) {
            ret.emplace_back(pos, pos + 10);
        }
        if (diffteam(pos + 8)) {
            ret.emplace_back(pos, pos + 8);
        }
        return ret;
    } else if (pos < 24 && diffteam(13)) { // black corner
        return { Move(pos, 13) };
    } else if (diffteam(76)) { // red corner
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
        if (!piece_on(pos + 10) && diffteam(pos + 16)) {
            ret.emplace_back(pos, pos + 16);
        }
        if (!piece_on(pos + 12) && diffteam(pos + 20)) {
            ret.emplace_back(pos, pos + 20);
        }
    }
    if (pos / 9 == 4 || pos / 9 == 9 || pos / 9 == 7 || pos / 9 == 3) {
        if (!piece_on(pos - 10) && diffteam(pos - 16)) {
            ret.emplace_back(pos, pos - 16);
        }
        if (!piece_on(pos - 12) && diffteam(pos - 20)) {
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
        if (pos % 9 != 0 && opposite<GEN_CAPTURE>(pos - 19)) {
            ret.emplace_back(pos, pos - 19);
        }
        if (pos % 9 != 8 && opposite<GEN_CAPTURE>(pos - 17)) {
            ret.emplace_back(pos, pos - 17);
        }
    }
    if (pos < 72 && !piece_on(pos + 9)) {
        if (pos % 9 != 0 && opposite<GEN_CAPTURE>(pos + 19)) {
            ret.emplace_back(pos, pos + 19);
        }
        if (pos % 9 != 8 && opposite<GEN_CAPTURE>(pos + 17)) {
            ret.emplace_back(pos, pos + 17);
        }
    }
    if (pos % 9 > 1 && !piece_on(pos - 1)) {
        if (pos / 9 != 0 && opposite<GEN_CAPTURE>(pos - 11)) {
            ret.emplace_back(pos, pos - 11);
        }
        if (pos / 9 != 9 && opposite<GEN_CAPTURE>(pos + 7)) {
            ret.emplace_back(pos, pos + 7);
        }
    }
    if (pos % 9 < 7 && !piece_on(pos + 1)) {
        if (pos / 9 != 0 && opposite<GEN_CAPTURE>(pos + 11)) {
            ret.emplace_back(pos, pos + 11);
        }
        if (pos / 9 != 9 && opposite<GEN_CAPTURE>(pos - 7)) {
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
}

// cannon moves
template <bool GEN_CAPTURE>
std::vector<Move> gen_cannon_(POS pos)
{
    std::vector<Move> ret {};
    ret.reserve(17);
}

// pawn moves
template <bool GEN_CAPTURE>
std::vector<Move> gen_pawn(POS pos)
{
    std::vector<Move> ret {};
    ret.reserve(3);
    const int target = pos - 9 * g_team;
    if (0 <= target && target < 90 && opposite<GEN_CAPTURE>(target)) {
        ret.emplace_back(pos, target);
    }
    if ((pos / 9 < 5 && g_team == R) || (pos / 9 > 4 && g_team == B)) {
        if (pos % 9 != 0 && opposite<GEN_CAPTURE>(pos - 1)) {
            ret.emplace_back(pos, pos - 1);
        }
        if (pos % 9 != 8 && opposite<GEN_CAPTURE>(pos + 1)) {
            ret.emplace_back(pos, pos + 1);
        }
    }
    return ret;
}
