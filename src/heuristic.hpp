#pragma once
#include "base.hpp"

// history

static std::array<std::array<unsigned int, 90>, 90> history_table_r_ {};
static std::array<std::array<unsigned int, 90>, 90> history_table_b_ {};

void history_init()
{
    history_table_r_.fill({});
    history_table_b_.fill({});
}

void history_set(Move move, TEAM team, DEPTH depth)
{
    if (team == R) {
        history_table_r_[move.beg][move.end] += depth * depth;
    } else {
        history_table_b_[move.beg][move.end] += depth * depth;
    }
}

void history_sort(std::vector<Move>& moves, TEAM team)
{
    const auto& t = team == R ? history_table_r_ : history_table_b_;
    std::sort(moves.begin(), moves.end(), [&](Move a, Move b) {
        return t[a.beg][a.end] > t[b.beg][b.end];
    });
}

// killer

static std::array<std::array<Move, 2>, 128> killer_table_ {};

void killer_init()
{
    killer_table_.fill({});
}

void update(Move move, DEPTH d)
{
    killer_table_[d][1] = killer_table_[d][0];
    killer_table_[d][0] = move;
}

std::array<Move, 2> get(DEPTH d)
{
    return killer_table_[d];
}

// tt

static std::vector<TTEntry> tt_table {};
static int size { 0 };
static int mask { 0 };

void tt_init(int _size = 8)
{
    tt_table.clear();
    tt_table.resize(1ll << _size);
    size = _size;
    mask = (1 << _size) - 1;
}

void tt_set(HASH hashkey, FLAG flag, DEPTH depth, Move move, VL vl)
{
    TTEntry& e = tt_table[hashkey & mask];
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

VL tt_get_vl(HASH hashkey, DEPTH depth, VL alpha, VL beta)
{
    const TTEntry& e = tt_table[hashkey & mask];
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

Move tt_get_move(HASH hashkey)
{
    return tt_table[hashkey & mask].move;
}

// capture


