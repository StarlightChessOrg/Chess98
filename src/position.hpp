#pragma once
#include "base.hpp"

// global variables
MATRIX g_board {};
TEAM g_team {};
HASH g_hashkey {};

// history moves and maintaining all pieces on board
std::vector<Move> history_moves_ {};
std::vector<PTYPE> history_captures_ {};
std::vector<HASH> history_hashkeys_ {};
std::vector<POS> pos_list_r_ {};
std::vector<POS> pos_list_b_ {};
std::array<PID, 90> pos_pid_r_ {};
std::array<PID, 90> pos_pid_b_ {};

// init global position
void position_init(const MATRIX& board, TEAM team)
{
    // variables init
    g_board = board;
    g_team = team;
    // init king pos first
    for (int i = 0; i < 90; i++) {
        if (board[i] == R_KING) {
            pos_pid_r_[i] = 0;
            pos_list_r_.emplace_back(i);
        } else if (board[i] == B_KING) {
            pos_pid_b_[i] = 0;
            pos_list_b_.emplace_back(i);
        }
    }
    // init pos list and hash
    for (int i = 0; i < 90; i++) {
        g_hashkey ^= hashkey_on(board[i], i);
        if (board[i] > 0 && board[i] != R_KING) {
            pos_pid_r_[i] = static_cast<PID>(pos_list_r_.size());
            pos_list_r_.emplace_back(i);
        } else if (board[i] < 0 && board[i] != B_KING){
            pos_pid_b_[i] = static_cast<PID>(pos_list_b_.size());
            pos_list_b_.emplace_back(i);
        }
    }
    // vector reservations
    history_moves_.reserve(96);
    history_captures_.reserve(96);
    history_hashkeys_.reserve(96);
}

// do move
void position_move(Move move)
{
    assert(move && g_board[move.beg] * g_team > 0);
    assert(!pos_list_r_.empty() && !pos_list_b_.empty());
    history_moves_.emplace_back(move);
    history_captures_.emplace_back(g_board[move.end]);
    history_hashkeys_.emplace_back(g_hashkey);
    if (g_team == R) {
        pos_list_r_[pos_pid_r_[move.beg]] = move.end;
        std::swap(pos_pid_r_[move.end], pos_pid_r_[move.beg]);
        if (g_board[move.end]) {
            pos_list_b_[pos_pid_b_[move.end]] = pos_list_b_.back();
            pos_list_b_.pop_back();
            pos_pid_b_[move.end] = 0;
        }
    } else {
        pos_list_b_[pos_pid_b_[move.beg]] = move.end;
        std::swap(pos_pid_b_[move.end], pos_pid_b_[move.beg]);
        if (g_board[move.end]) {
            pos_list_r_[pos_list_r_[move.end]] = pos_list_r_.back();
            pos_list_r_.pop_back();
            pos_pid_r_[move.end] = 0;
        }
    }
    g_hashkey ^= hashkey_on(g_board[move.beg], move.beg);
    g_hashkey ^= hashkey_on(g_board[move.end], move.end);
    g_hashkey ^= SIDE_KEY;
    g_board[move.end] = g_board[move.beg];
    g_board[move.beg] = 0;
    g_team = -g_team;
}

// undo move
void position_undo()
{
    assert(!history_moves_.empty());
    const Move move = history_moves_.back();
    const PTYPE captured = history_captures_.back();
    const HASH hashkey = history_hashkeys_.back();
    history_moves_.pop_back();
    history_captures_.pop_back();
    history_hashkeys_.pop_back();
    if (g_team == R) {
        pos_list_b_[pos_pid_b_[move.end]] = move.beg;
        std::swap(pos_pid_b_[move.end], pos_pid_b_[move.beg]);
        if (captured) {
            pos_pid_r_[move.end] = static_cast<PID>(pos_list_r_.size());
            pos_list_r_.emplace_back(move.end);
        }
    } else {
        pos_list_r_[pos_pid_r_[move.end]] = move.beg;
        std::swap(pos_pid_r_[move.end], pos_pid_r_[move.beg]);
        if (captured) {
            pos_pid_b_[move.end] = static_cast<PID>(pos_list_b_.size());
            pos_list_b_.emplace_back(move.end);
        }
    }
    g_board[move.beg] = g_board[move.end];
    g_board[move.end] = captured;
    g_hashkey = hashkey;
    g_team = -g_team;
}

// position util diffteam
bool diffteam(POS p)
{
    return g_team * g_board[p] <= 0;
}

// position util opposite
template <bool GEN_CAPTURE>
bool opposite(POS p)
{
    if constexpr (GEN_CAPTURE) {
        return g_team * g_board[p] < 0
    } else {
        return diffteam(p);
    }
}

// position util piece_on
PTYPE piece_on(POS p)
{
    return g_board[p];
}
