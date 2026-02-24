#pragma once
#include "evaluate.hpp"
#include "movepicker.hpp"

namespace search {

constexpr DEPTH Q_DEPTH = 32;
constexpr DEPTH Q_CHECKING_DEPTH = 8;

DEPTH distance { 0 };

int node_number { 0 };
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
    tt::init();
    evaluate::init(matrix);
}

void move(Move m)
{
    distance++;
    position::move(m);
    evaluate::update();
}

void undo_move()
{
    distance--;
    position::undo_move();
    evaluate::undo_update();
}

VL search(DEPTH depth, VL a, VL b, bool pv);
VL search_q(DEPTH q_depth, VL a, VL b);

VL search(DEPTH depth, VL a, VL b, bool pv)
{
    node_number++;
    VL tt_vl = tt::get_vl(position::hashkey, depth, a, b);
    if (tt_vl != INVALID_VL) {
        return tt_vl;
    }
    if (depth <= 0) {
        return search_q(Q_DEPTH, a, b);
    }
    VL vlbest = -INF;
    Move movebest {};
    MovePicker mp{};
    for (Move m = mp.next(); m; m = mp.next()) {
        move(m);
        VL vl { -INF };
        if (vlbest == -INF) {
            vl = -search(depth - 1, -b, -a, PV);
        } else {
            vl = -search(depth - 1, -a - 1, -a, CUT);
            if (a < vl && vl < b) {
                vl = -search(depth - 1, -b, -a, PV);
            }
        }
        undo_move();
        if (vl >= b && is_quiet(m) && !pv) {
            killer::update(m, depth);
            tt::set(position::hashkey, tt::BETA, depth, m, vl);
            return vl;
        }
        if (vl > vlbest) {
            vlbest = vl;
            movebest = m;
        }
    }
    if (movebest && is_quiet(movebest) && !pv) {
        history::update(movebest, depth);
    }
    if (vlbest <= a) {
        tt::set(position::hashkey, tt::ALPHA, depth, movebest, vlbest);
    } else if (vlbest >= b) {
        tt::set(position::hashkey, tt::BETA, depth, movebest, vlbest);
    } else {
        tt::set(position::hashkey, tt::EXACT, depth, movebest, vlbest);
    }
    return vlbest;
}

VL search_q(DEPTH q_depth, VL a, VL b)
{
    return evaluate::evaluate() ? a + q_depth : b;
}

}
