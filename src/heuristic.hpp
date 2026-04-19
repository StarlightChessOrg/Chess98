#pragma once
#include "base.hpp"

// history

std::array<std::array<UINT32, 90>, 90> history_table_r_ {};
std::array<std::array<UINT32, 90>, 90> history_table_b_ {};

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

std::array<std::array<Move, 2>, 128> killer_table_ {};

void killer_init()
{
    killer_table_.fill({});
}

void update(Move move, DEPTH d)
{
    killer_table_[d][1] = killer_table_[d][0];
    killer_table_[d][0] = move;
}

std::array<Move, 2> killer_get(DEPTH d)
{
    return killer_table_[d];
}

// tt

std::vector<TTEntry> tt_table_ {};
UINT8 tt_size_ { 0 };
UINT8 tt_mask_ { 0 };

void tt_init(int _size = 8)
{
    tt_table_.clear();
    tt_table_.resize(1ll << _size);
    tt_size_ = _size;
    tt_mask_ = (1 << _size) - 1;
}

void tt_set(HASH hashkey, HASH_FLAG flag, DEPTH depth, Move move, VL vl)
{
    TTEntry& e = tt_table_[hashkey & tt_mask_];
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
    const TTEntry& e = tt_table_[hashkey & tt_mask_];
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
    return tt_table_[hashkey & tt_mask_].move;
}

// capture

bool see_ge(Move m, VL target_vl)
{
    // TODO
    return false;
}

bool mvvlva(std::vector<Move>& move_list)
{
    // TODO
    return false;
}
