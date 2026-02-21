#pragma once
#include "evaluate.hpp"
#include "movepicker.hpp"

namespace search {

constexpr DEPTH Q_DEPTH = 32;
constexpr DEPTH Q_CHECKING_DEPTH = 8;

DEPTH distance { 0 };

SEARCH_RETS rets {};
std::vector<int> durations {};

void init(const MATRIX& matrix, TEAM team)
{
    distance = 0;
    rets.clear();
    rets.reserve(24);
    durations.clear();
    durations.reserve(24);
    position::init(matrix, team);
    history::init();
    killer::init();
    evaluate::init(matrix);
}

void move(Move m) 
{
    distance++;
    position::move(m);
    evaluate::update(m);
}

void undo_move()
{
    distance--;
    position::undo_move();
    evaluate::undo_update();
}

void search(int time_limit_ms)
{
    Timer timer { time_limit_ms };
    for (DEPTH depth = 0; !timer.time_up(); depth++) {
        rets.emplace_back(search_root(depth));
        durations.emplace_back(timer.duration());
    }
}

SEARCH_RET search_root(DEPTH depth)
{
    Move move_best {};
    VL vl_best { -INF };
    MovePicker mp {};
    for (Move m = mp.next(); m != Move {}; m = mp.next()) {
        move(m);
        VL vl { -INF };
        if (vl_best == -INF) {
            vl = -search_pv(depth - 1, -INF, INF);
        } else {
            vl = -search_cut(depth - 1, -vl_best);
            if (vl > vl_best) {
                vl = -search_pv(depth - 1, -INF, -vl_best);
            }
        }
        if (vl > vl_best) {
            vl_best = vl;
            move_best = m;
        }
        undo_move();
    }
    return { move_best, vl_best };
}

VL search_pv(DEPTH depth, VL a, VL b)
{
    if (depth <= 0) {
        return search_q(Q_DEPTH, a, b);
    }
}

VL search_cut(DEPTH depth, VL b)
{
    if (depth <= 0) {
        return search_q(Q_DEPTH, -INF, b);
    }
}

VL search_q(DEPTH q_depth, VL a, VL b)
{
    if (q_depth <= 0) {
        return evaluate::evaluate();
    }
}

}
