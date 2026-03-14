#pragma once
#include "heuristic.hpp"
#include "movegen.hpp"

constexpr char STATUS_TT = 0;
constexpr char STATUS_GOOD_CAPTURES = 1;
constexpr char STATUS_KILLER = 2;
constexpr char STATUS_QUIET = 3;
constexpr char STATUS_BAD_CAPTURES = 4;

class MovePicker {
    std::vector<Move> moves {};
    char status { STATUS_TT };
    char i = -1;

public:
    Move next()
    {
        if (status == STATUS_TT) {
            status++;
            Move ret = tt_get_move(g_hashkey);
            return ret ? ret : next();
        } else if (status == STATUS_GOOD_CAPTURES) {
            status++;
            
        }
    }

protected:
};
