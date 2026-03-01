#pragma once
#include "heuristic.hpp"
#include "position.hpp"

// king moves
std::vector<Move> gen_king_(POS pos)
{
    std::vector<Move> ret {};
    ret.reserve(4);
    const bool condition1 = (2 < pos && pos < 15) || (65 < pos && pos < 78);
    if (condition1 && !same_team(piece_on(pos + 9))) {
        ret.emplace_back(pos, pos + 9);
    }
    const bool condition2 = (11 < pos && pos < 24) || (74 < pos && pos < 87);
    if (condition2 && !same_team(piece_on(pos - 9))) {
        ret.emplace_back(pos, pos - 9);
    }
    if (pos % 9 == 4 && !same_team(piece_on(pos - 1))) {
        ret.emplace_back(pos, pos - 1);
    }
    if (pos % 9 == 4 && !same_team(piece_on(pos + 1))) {
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
        if (!same_team(piece_on(pos - 10))) {
            ret.emplace_back(pos, pos - 10);
        }
        if (!same_team(piece_on(pos - 8))) {
            ret.emplace_back(pos, pos - 8);
        }
        if (!same_team(piece_on(pos + 10))) {
            ret.emplace_back(pos, pos + 10);
        }
        if (!same_team(piece_on(pos + 8))) {
            ret.emplace_back(pos, pos + 8);
        }
        return ret;
    } else if (pos < 24 && !same_team(piece_on(13))) { // black corner
        return { Move(pos, 13) };
    } else if (!same_team(piece_on(76))) { // red corner
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
        if (!same_team(piece_on(pos + 16))) {
            ret.emplace_back(pos, pos + 16);
        }
        if (!same_team(piece_on(pos + 20))) {
            ret.emplace_back(pos, pos + 20);
        }
    }
    if (pos / 9 == 4 || pos / 9 == 9 || pos / 9 == 7 || pos / 9 == 3) {
        if (!same_team(piece_on(pos - 16))) {
            ret.emplace_back(pos, pos - 16);
        }
        if (!same_team(piece_on(pos - 20))) {
            ret.emplace_back(pos, pos - 20);
        }
    }
    return ret;
}

std::pair<std::vector<Move>, std::vector<Move>> gen_knight(POS pos)
{
    return {};
}
