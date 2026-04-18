#pragma once
#include "heuristic.hpp"
#include "movegen.hpp"

constexpr char STATUS_TT = 0;
constexpr char STATUS_GOOD_CAPTURES = 1;
constexpr char STATUS_KILLER = 2;
constexpr char STATUS_QUIET = 3;
constexpr char STATUS_BAD_CAPTURES = 4;

class MovePicker {
    std::vector<Move> quiet_moves { };
    std::vector<Move> good_captures { };
    std::vector<Move> bad_captures { };
    std::array<bool, 8100> moves_picked { };
    char status { STATUS_TT };
    char i = -1;

public:
    Move next()
    {
        if (status == STATUS_TT) {
            status++;
            const Move m = tt_get_move(g_hashkey);
            quiet_moves[int(m)] = m;
            return m ? m : next();
        } else if (status == STATUS_GOOD_CAPTURES) {
            
        }
    }

protected:
};
