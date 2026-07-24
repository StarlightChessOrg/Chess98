#pragma once
#include "base.hpp"
#include "position.hpp"

std::array<std::array<UINT32, 90>, 90> history_table_r_ { };
std::array<std::array<UINT32, 90>, 90> history_table_b_ { };
std::array<std::array<Move, 2>, 128> killer_table_ { };
std::vector<TTEntry> tt_table_ { };
int tt_size_ { 0 };
std::uint32_t tt_mask_ { 0 };

// init the history table
void history_init()
{
    history_table_r_.fill({ });
    history_table_b_.fill({ });
}

// set a history weight to the table
void history_set(Move move, TEAM team, DEPTH depth)
{
    if (team == R) {
        history_table_r_[move.beg][move.end] += depth * depth;
    } else {
        history_table_b_[move.beg][move.end] += depth * depth;
    }
}

// sort moves via history table
void history_sort(std::vector<Move>& moves, TEAM team)
{
    const auto& t = team == R ? history_table_r_ : history_table_b_;
    std::sort(moves.begin(), moves.end(), [&](Move a, Move b) {
        return t[a.beg][a.end] > t[b.beg][b.end];
    });
}

// init the killer table
void killer_init()
{
    killer_table_.fill({ });
}

// set a killer move for a depth to the table
void killer_set(Move move, DEPTH d)
{
    killer_table_[d][1] = killer_table_[d][0];
    killer_table_[d][0] = move;
}

// get killer moves for a depth from the table
std::array<Move, 2> killer_get(DEPTH d)
{
    return killer_table_[d];
}

// init the tt (_size is log2 of entry count, e.g. 16 -> 65536)
void tt_init(int _size = 20)
{
    tt_table_.clear();
    tt_table_.resize(1ll << _size);
    tt_size_ = _size;
    tt_mask_ = (std::uint32_t(1) << _size) - 1;
}

// set a tt entry
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

// get a vl from the tt entry, return INVALID_VL if not found
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

// get a move from the tt entry, return an empty move if not found
Move tt_get_move()
{
    const TTEntry& entry = tt_table_[g_hashkey & tt_mask_];
    if (entry.key != g_hashkey) return { };
    return entry.move;
}

// culculate whether a move is good enough via SEE
bool see_ge(Move move, VL threshold)
{
    VL vl = 0;
    int count = 0;
    vl += WEIGHTS[abs(piece_on(move.end))];
    position_move(move);
    count++;
    for (POS p = get_protector(move.end); p < 90; p = get_protector(p)) {
        vl += WEIGHTS[abs(piece_on(move.end))] * ((count % 2) ? -1 : 1);
        position_move(Move(p, move.end));
        count++;
    }
    for (int i = 0; i < count; i++) position_undo();
    return vl >= threshold;
}

// sort the moves via MVV-LVA
void mvvlva_sort(std::vector<Move>& move_list)
{
    std::sort(move_list.begin(), move_list.end(), [](Move a, Move b) {
        const VL a_beg = WEIGHTS[abs(piece_on(a.beg))];
        const VL a_end = WEIGHTS[abs(piece_on(a.end))];
        const VL b_beg = WEIGHTS[abs(piece_on(b.beg))];
        const VL b_end = WEIGHTS[abs(piece_on(b.end))];
        return a_beg - a_end < b_beg - b_end;
    });
}
