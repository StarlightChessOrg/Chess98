#pragma once
#include "heuristic.hpp"
#include "movegen.hpp"

constexpr char STATUS_TT = 0;
constexpr char STATUS_GOOD_CAPTURES = 1;
constexpr char STATUS_KILLER = 2;
constexpr char STATUS_QUIET = 3;
constexpr char STATUS_BAD_CAPTURES = 4;
constexpr char STATUS_END = 5;

class MovePicker {
    std::vector<Move> moves { };
    DEPTH depth { };
    char status { STATUS_TT };
    char i = -1;

public:
    MovePicker(DEPTH d)
        : depth(d)
    {
    }
    Move next()
    {
        if (status == STATUS_TT) {
            status++;
            const Move m = tt_get_move(g_hashkey);
            return m ? m : next();
        }
        if (status == STATUS_END) return Move { };
        if (i == -1) {
            if (status == STATUS_GOOD_CAPTURES) {
                moves = gen_all_capture_moves();
                // TODO
            } else if (status == STATUS_KILLER) {
                for (const Move m : killer_get(depth)) {
                    if (legal_move(m)) moves.emplace_back(m);
                }
            } else if (status == STATUS_QUIET) {
                moves = gen_all_quiet_moves();
                history_sort(moves, g_team);
            } else if (status == STATUS_BAD_CAPTURES) {
                // TODO
            }
        }
        i++;
        if (i < moves.size()) {
            return moves[i];
        } else {
            i = -1;
            status++;
            return next();
        }
    }
};
