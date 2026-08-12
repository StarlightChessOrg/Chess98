#pragma once
#include "base.hpp"
#include "position.hpp"

std::array<std::array<UINT32, 90>, 90> history_table_r_ { };
std::array<std::array<UINT32, 90>, 90> history_table_b_ { };
std::array<std::array<Move, 2>, 128> killer_table_ { };
std::vector<TTEntry> tt_table_ { };
std::uint32_t tt_size_ { 0 };
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

// sort moves via history table (stable tie-break on squares)
template <class It>
void history_sort(It first, It last, TEAM team)
{
    const auto& t = team == R ? history_table_r_ : history_table_b_;
    std::sort(first, last, [&](Move a, Move b) {
        const UINT32 ha = t[a.beg][a.end];
        const UINT32 hb = t[b.beg][b.end];
        if (ha != hb) return ha > hb;
        if (a.beg != b.beg) return a.beg < b.beg;
        return a.end < b.end;
    });
}

void history_sort(std::vector<Move>& moves, TEAM team)
{
    history_sort(moves.begin(), moves.end(), team);
}

// init the killer table
void killer_init()
{
    killer_table_.fill({ });
}

// set a killer move for a depth to the table
void killer_set(Move move, DEPTH d)
{
    if (move == killer_table_[d][0]) return;
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

// rough SEE: reject only if victim < attacker and the square is protected
bool see_ge(Move move, VL /*threshold*/)
{
    const VL victim = WEIGHTS[std::size_t(std::abs(piece_on(move.end)))];
    const VL attacker = WEIGHTS[std::size_t(std::abs(piece_on(move.beg)))];
    if (victim >= attacker) return true;
    position_move(move);
    const bool protected_ = get_protector(move.end) < 90;
    position_undo();
    return !protected_;
}

// sort the moves via MVV-LVA (stable tie-break on squares)
template <class It>
void mvvlva_sort(It first, It last)
{
    std::sort(first, last, [](Move a, Move b) {
        const VL a_beg = WEIGHTS[std::size_t(std::abs(piece_on(a.beg)))];
        const VL a_end = WEIGHTS[std::size_t(std::abs(piece_on(a.end)))];
        const VL b_beg = WEIGHTS[std::size_t(std::abs(piece_on(b.beg)))];
        const VL b_end = WEIGHTS[std::size_t(std::abs(piece_on(b.end)))];
        const VL sa = VL(a_beg - a_end);
        const VL sb = VL(b_beg - b_end);
        if (sa != sb) return sa < sb;
        if (a.beg != b.beg) return a.beg < b.beg;
        return a.end < b.end;
    });
}

void mvvlva_sort(std::vector<Move>& move_list)
{
    mvvlva_sort(move_list.begin(), move_list.end());
}
