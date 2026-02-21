#pragma once
#include "evaluate.hpp"
#include "movepicker.hpp"

namespace search {

SEARCH_RETS rets {};
std::vector<int> durations {};
int node_number { 0 };

void init(const MATRIX& matrix, TEAM team)
{
    position::init(matrix, team);
    history::init();
    killer::init();
    evaluate::init();
}

void search_main(int time_limit_ms)
{
    Timer timer {time_limit_ms};
    for (DEPTH depth = 0; !timer.time_up(); depth++) {
        rets.emplace_back(search_root(depth));
    }
}

SEARCH_RET search_root(DEPTH depth)
{
    Move best_move {};
    VL best_vl {};
    MovePicker mp {};
    for (Move m = mp.next(); m != Move {}; m = mp.next()) {
        position::do_move(m);
        evaluate::update(m);
        
    }
}

}
