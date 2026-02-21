#pragma once
#include "evaluate.hpp"
#include "movepicker.hpp"

namespace search {

SEARCH_RETS rets {};
int node_number { 0 };

void init(const MATRIX& matrix, TEAM team)
{
    position::init(matrix, team);
    history::init();
    killer::init();
    evaluate::init();
}

void search(int time_limit_ms)
{
    Timer timer {time_limit_ms};
    for (DEPTH depth = 0; !timer.time_up(); depth++) {
        rets.emplace_back(search_root(depth));
    }
}

SEARCH_RET search_root(DEPTH depth)
{
    Move move_best {};
    VL vl_best { -INF };
    MovePicker mp {};
    for (Move m = mp.next(); m != Move {}; m = mp.next()) {
        position::move(m);
        evaluate::update(m);
        VL vl { -INF };
        if (vl_best == -INF) {
            vl = -search_pv(depth - 1, -INF, INF);
        } else {
            vl = -search_cut(depth - 1, -vl_best);
            if (vl > vl_best) {
                vl = -search_pv(depth - 1, -INF, -vl_best);
            }
        }
        if (vl > vl_best)
        {
            vl_best = vl;
            move_best = m;
        }
        position::undo_move();
        evaluate::undo_update();
    }
    return { move_best, vl_best };
}

VL search_pv(DEPTH depth, VL a, VL b)
{
    
}

VL search_cut(DEPTH depth, VL b)
{

}

VL search_q(DEPTH q_depth, VL a, VL b)
{

}

}
