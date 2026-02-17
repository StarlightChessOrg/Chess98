#pragma once
#include "base.hpp"

namespace history {

namespace {
    std::array<std::array<unsigned int, 90>, 90> r_table {};
    std::array<std::array<unsigned int, 90>, 90> b_table {};
    constexpr unsigned int strategy(DEPTH depth)
    {
        return depth * depth;
    }
}

void init()
{
    r_table = {};
    b_table = {};
}

void update(Move move, TEAM team, DEPTH depth)
{
    if (team == R) {
        r_table[move.beg][move.end] += strategy(depth);
    } else {
        b_table[move.beg][move.end] += strategy(depth);
    }
}

void sort(MOVES& moves, TEAM team)
{
    if (team == R) {
        std::sort(moves.begin(), moves.end(), [](Move a, Move b) {
            return r_table[a.beg][a.end] > r_table[b.beg][b.end];
        });
    } else {
        std::sort(moves.begin(), moves.end(), [](Move a, Move b) {
            return b_table[a.beg][a.end] > b_table[b.beg][b.end];
        });
    }
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

void update(Move move, DEPTH distance)
{
    table[distance][1] = table[distance][0];
    table[distance][0] = move;
}

std::array<Move, 2> get(DEPTH distance)
{
    return table[distance];
}

}

namespace tt {
    
// TODO: implement tt

}
