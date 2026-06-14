#pragma once
#include "heuristic.hpp"
#include "movegen.hpp"

enum MPStatus {
    STATUS_TT,
    STATUS_GOOD_CAPTURES,
    STATUS_KILLER,
    STATUS_QUIET,
    STATUS_BAD_CAPTURES,
};

class MovePicker {
    DEPTH depth { 0 };
    TEAM team { 0 };
    MPStatus status = STATUS_TT;
    Move tt_move { };
    std::array<Move, 2> killers { };
    std::vector<Move> moves { };
    std::vector<Move> bad_captures { };

public:
    MovePicker(DEPTH depth, TEAM team) : depth(depth), team(team) { }
    Move next()
    {
        static std::uint8_t generated = 0; // 0 is false, 1 is true
        static std::uint8_t i { 0 }; // the index
        // tt move
        if (status == STATUS_TT) {
            status = STATUS_GOOD_CAPTURES;
            tt_move = tt_get_move(g_hashkey);
            return tt_move ? tt_move : next();
        }
        // good captures
        else if (status == STATUS_GOOD_CAPTURES) {
            if (!(generated++)) moves = gen_all_capture_moves();
            if (i < moves.size()) {
                if (moves[i] == tt_move) return next();
                if (see_ge(moves[i], 0)) {
                    return moves[i++];
                } else {
                    bad_captures.emplace_back(moves[i++]);
                    return next();
                }
            } else {
                generated = i = 0;
                moves.clear();
                status = STATUS_KILLER;
                return next();
            }
        }
        // killer moves
        else if (status == STATUS_KILLER) {
            if (!(generated++)) killers = killer_get(depth);
            if (i < killers.size()) {
                if (legal_move(killers[i])) {
                    return killers[i++];
                } else {
                    i++;
                    return next();
                }
            } else {
                generated = i = 0;
                status = STATUS_QUIET;
                return next();
            }
        }
        // quiet moves
        else if (status == STATUS_QUIET) {
            if (!(generated++)) {
                moves = gen_all_quiet_moves();
                history_sort(moves, team);
            }
            if (i < moves.size()) {
                Move m = moves[i++];
                bool c = m == tt_move && m == killers[0] && m == killers[1];
                return c ? next() : m;
            } else {
                generated = i = 0;
                moves.clear();
                status = STATUS_BAD_CAPTURES;
                return next();
            }
        }
        // bad captures or end move
        return i < bad_captures.size() ? bad_captures[i++] : Move { };
    }
};
