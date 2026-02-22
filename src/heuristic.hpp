#pragma once
#include "base.hpp"

namespace history {

namespace {
    std::array<std::array<unsigned int, 90>, 90> r_table {};
    std::array<std::array<unsigned int, 90>, 90> b_table {};
}

void init()
{
    r_table = b_table = {};
}

void update(Move move, DEPTH d)
{
    (team_now() == R ? r_table : b_table)[move.beg][move.end] += d * d;
}

void sort(MOVES& moves, TEAM team)
{
    const auto& table = team == R ? r_table : b_table;
    std::sort(moves.begin(), moves.end(), [&](const Move& a, const Move& b) {
        return table[a.beg][a.end] > table[b.beg][b.end];
    });
}

}

namespace killer {

namespace {
    std::array<std::array<Move, 2>, 128> table {};
}

void init()
{
    table = {};
}

void update(Move move, DEPTH d)
{
    table[d][1] = table[d][0];
    table[d][0] = move;
}

std::array<Move, 2> get(DEPTH d)
{
    return table[d];
}

}

namespace tt {

// TODO: implement tt

}
