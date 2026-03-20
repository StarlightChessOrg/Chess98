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
std::array<unsigned short, 9> bitline8_ {};
std::array<unsigned short, 10> bitline9_ {};

// init global position
void position_init(const MATRIX& board, TEAM team)
{
    // variables init
    g_board = board;
    g_team = team;
    // init king pos first in order to keep it in the 1st position
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
        if (board[i] != 0) {
            bitline8_[i % 9] |= 1 << (i / 9);
            bitline9_[i / 9] |= 1 << (i % 9);
        }
        if (board[i] > 0 && board[i] != R_KING) {
            pos_pid_r_[i] = static_cast<PID>(pos_list_r_.size());
            pos_list_r_.emplace_back(i);
        }
        if (board[i] < 0 && board[i] != B_KING) {
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
    bitline8_[move.end % 9] |= 1 << (move.end / 9);
    bitline9_[move.end / 9] |= 1 << (move.end % 9);
    bitline8_[move.beg % 9] &= ~(1 << (move.beg / 9));
    bitline9_[move.beg / 9] &= ~(1 << (move.beg % 9));
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
    bitline8_[move.end % 9] &= captured ? ~(1 << (move.end / 9)) : 0xfff;
    bitline9_[move.end / 9] &= captured ? ~(1 << (move.end % 9)) : 0xfff;
    bitline8_[move.beg % 9] |= 1 << (move.beg / 9);
    bitline9_[move.beg / 9] |= 1 << (move.beg % 9);
    g_board[move.beg] = g_board[move.end];
    g_board[move.end] = captured;
    g_hashkey = hashkey;
    g_team = -g_team;
}

// position util not_same_team
bool not_same_team(POS p)
{
    return g_team * g_board[p] <= 0;
}

// position util opposite
bool opposite(POS p)
{
    return g_team * g_board[p] < 0;
}

// position util team_diff
template <bool GEN_CAPTURE>
bool team_diff(POS p)
{
    return GEN_CAPTURE ? opposite(p) : not_same_team(p);
}

// position util piece_on
PTYPE piece_on(POS p)
{
    return g_board[p];
}

// position util get_bl8 from bitlines
unsigned short get_bl8(POS pos)
{
    return bitline8_[pos % 9];
}

// position util get_bl9
unsigned short get_bl9(POS pos)
{
    return bitline9_[pos / 9];
}

// judge whether a move is valid or not in situation
bool legal_move(Move move)
{
    const PTYPE p = piece_on(move.beg);
    // piece not exists or opposite, or same-team attack
    if (g_team * piece_on(move.end) > 0 || p * g_team <= 0) return false;
    // specific piece legal judge
    if (abs(p) == R_BISHOP) { // elephant eyes
        const int d = move.end - move.beg;
        if (d == 20) {
            if (piece_on(move.beg + 10)) return false;
        } else if (d == 16) {
            if (piece_on(move.beg + 8)) return false;
        } else if (d == -20) {
            if (piece_on(move.beg - 10)) return false;
        } else if (piece_on(move.beg - 8)) {
            return false;
        }
    } else if (abs(p) == R_KNIGHT) { // knight legs
        const int d = move.end - move.beg;
        if (d == 17 || d == 15) {
            if (piece_on(move.beg + 9)) return false;
        } else if (d == -17 || d == -15) {
            if (piece_on(move.beg - 9)) return false;
        } else if (d == 10 || d == -6) {
            if (piece_on(move.beg + 1)) return false;
        } else if (d == 6 || d == -10) {
            if (piece_on(move.beg - 1)) return false;
        }
    } else if (abs(p) == R_CANNON) { // cannon moves

    } else if (abs(p) == R_ROOK) { // rook moves
    }
    // in check after this move

    return true;
}

// judge whether a position is attacked by enemy
bool pos_attacked_by_enemy(POS pos)
{
    return false;
}