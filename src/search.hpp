#pragma once
#include "evaluate.hpp"
#include "moves.hpp"

VL search_vl_(DEPTH depth, VL a, VL b)
{
    VL vl { -INF };
    return vl;
}

VL search_q_(VL b)
{
    VL vl { -INF };
    return vl;
}

SEARCH_RET search()
{
    const Timer timer { 1000 };
    VL vl { -INF };
    for (DEPTH depth = 0; !timer.time_up_3xlesser(); depth++) {
        vl = search_vl_(depth, -INF, INF);
    }
    const Move move = tt_get_move(g_hashkey);
    return { move, vl };
}
