#pragma once
#include "base.hpp"

namespace history {

static std::array<std::array<unsigned int, 90>, 90> r_table {};
static std::array<std::array<unsigned int, 90>, 90> b_table {};

void init()
{
    r_table.fill({});
    b_table.fill({});
}

void update(Move move, TEAM team, DEPTH depth)
{
    if (team == R) {
        r_table[move.beg][move.end] += depth * depth;
    } else {
        b_table[move.beg][move.end] += depth * depth;
    }
}

void sort_moves(MOVE_LIST& moves, TEAM team)
{
    const auto& table = team == R ? r_table : b_table;
    std::sort(moves.begin(), moves.end(), [&](Move a, Move b) {
        return table[a.beg][a.end] > table[b.beg][b.end];
    });
}

}

namespace killer {

std::array<std::array<Move, 2>, 128> table {};

void init()
{
    table.fill({});
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

using FLAG = char;
constexpr FLAG EXACT = 0;
constexpr FLAG ALPHA = 1;
constexpr FLAG BETA = 2;

struct Entry {
    HASH key { 0 };
    FLAG flag { 0 };
    VL vl { 0 };
    DEPTH depth { 0 };
    Move move {};
};

std::vector<Entry> table {};
int size { 0 };
int mask { 0 };

void init(int _size = 8)
{
    table.clear();
    table.resize(1ll << _size);
    size = _size;
    mask = (1 << _size) - 1;
}

void set(HASH hashkey, FLAG flag, DEPTH depth, Move move, VL vl)
{
    Entry& e = table[hashkey & mask];
    if (e.key == 0) { // empty set
        e.key = hashkey;
        e.flag = flag;
        e.depth = depth;
        e.move = move;
        e.vl = vl;
    } else if (e.key == hashkey) { // same position replace
        if (depth >= e.depth) { // only deeper
            e.flag = flag;
            e.depth = depth;
            e.move = move;
            e.vl = vl;
        }
    } else { // collision
        e.key = hashkey;
        e.flag = flag;
        e.depth = depth;
        e.move = move;
        e.vl = vl;
    }
}

VL get_vl(HASH hashkey, DEPTH depth, VL alpha, VL beta)
{
    const Entry& e = table[hashkey & mask];
    if (e.key != hashkey || e.depth < depth) {
        return INVALID_VL;
    } else if (e.flag == EXACT) {
        return e.vl;
    } else if (e.flag == ALPHA && e.vl <= alpha) {
        return e.vl;
    } else if (e.flag == BETA && e.vl >= beta) {
        return e.vl;
    }
    return INVALID_VL;
}

Move get_move(HASH hashkey)
{
    return table[hashkey & mask].move;
}

}
