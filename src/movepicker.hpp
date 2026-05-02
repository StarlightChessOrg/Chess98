#pragma once
#include "heuristic.hpp"
#include "movegen.hpp"

enum {
    STATUS_TT,
    STATUS_GOOD_CAPTURES,
    STATUS_KILLER,
    STATUS_QUIET,
    STATUS_BAD_CAPTURES,
    STATUS_END,
};

class MovePicker {
    std::vector<Move> moves { };
    std::vector<Move> bad_captures { };
    Move tt_move { };
    Move killer_move_1 { };
    Move killer_move_2 { };
    DEPTH depth { 0 };
    char status { STATUS_TT };
    char i = -1;

public:
    MovePicker(DEPTH d) : depth(d) { }
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
                mvv_lva(moves);
            } else if (status == STATUS_KILLER) {
                moves.clear();
                for (const Move m : killer_get(depth)) {
                    if (legal_move(m)) moves.emplace_back(m);
                }
            } else if (status == STATUS_QUIET) {
                moves = gen_all_quiet_moves();
                history_sort(moves, g_team);
            }
        }
        i++;
        if (i < moves.size()) {
            const bool exp = moves[i] != killer_move_1;
            const bool exp2 = moves[i] != killer_move_2;
            const bool exp3 = moves[i] != tt_move;
            if (exp && exp2 && exp3)
                return moves[i];
            else
                return next();
        } else {
            i = -1;
            status++;
            return next();
        }
    }
};
