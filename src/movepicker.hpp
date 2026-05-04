#pragma once
#include "heuristic.hpp"
#include "movegen.hpp"

enum {
    STATUS_TT,
    STATUS_GOOD_CAPTURES,
    STATUS_KILLER,
    STATUS_QUIET,
    STATUS_BAD_CAPTURES,
};

class MovePicker {
    std::vector<Move> moves {};
    std::vector<Move> bad_captures {};
    Move tt_move {};
    Move killer_move_1 {};
    Move killer_move_2 {};
    DEPTH depth { 0 };
    char status { STATUS_TT };
    char i = -1;

public:
    MovePicker(DEPTH d)
        : depth(d)
    {
        bad_captures.reserve(8);
    }

    Move next()
    {
        if (status == STATUS_TT) {
            status++;
            tt_move = tt_get_move(g_hashkey);
            return tt_move ? valid(tt_move) : next();
        } else if (status == STATUS_KILLER) {
            return killer_move();
        } else if (status == STATUS_GOOD_CAPTURES) {
            return good_capture();
        } else if (status == STATUS_QUIET) {
            moves = gen_all_quiet_moves();
            i++;
            if (i < moves.size()) {
                return valid(moves[i]);
            } else {
                status++;
                i = -1;
                return next();
            }
        } else if (status == STATUS_BAD_CAPTURES) {
            i++;
            if (i < bad_captures.size()) {
                return valid(bad_captures[i]);
            } else {
                return Move {};
            }
        } else {
            return Move {};
        }
    }

protected:
    Move killer_move()
    {
        if (i == -1) {
            killer_move_1 = killer_get(depth)[0];
            killer_move_2 = killer_get(depth)[1];
            i++;
        }
         if (i == 0) {
            if (legal_move(killer_move_1)) {
                return killer_move_1;
            } else {
                i++;
                return next();
            }
        } else {
            i = -1;
            if (legal_move(killer_move_2)) {
                return killer_move_2;
            } else {
                status++;
                return next();
            }
        }
    }

    Move good_capture()
    {
        if (i == -1) {
            moves = gen_all_capture_moves();
            mvv_lva(moves);
            i++;
        }
        if (i < moves.size()) {
            i++;
            if (see_ge(moves[i - 1], 0)) {
                return valid(moves[i - 1]);
            } else {
                bad_captures.emplace_back(moves[i - 1]);
                return next();
            }
        } else {
            i = -1;
            status++;
            return next();
        }
    }

    Move valid(Move m)
    {
        position_move(m);
        if (in_check()) {
            position_undo();
            return next();
        } else {
            position_undo();
            return m;
        }
    }
};
