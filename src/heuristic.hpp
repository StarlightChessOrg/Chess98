#pragma once
#include "base.hpp"
#include "position.hpp"

void history_init();
void history_set(Move move, TEAM team, DEPTH depth);
void history_sort(std::vector<Move>& moves, TEAM team);
void killer_init();
void killer_set(Move move, DEPTH d);
std::array<Move, 2> killer_get(DEPTH d);
void tt_init(int _size = 8);
void tt_set(HASH hashkey, HASH_FLAG flag, DEPTH depth, Move move, VL vl);
VL tt_get_vl(HASH hashkey, DEPTH depth, VL alpha, VL beta);
Move tt_get_move(HASH hashkey);
bool see_ge(Move move, VL threshold);
void mvvlva_sort(std::vector<Move>& move_list);
