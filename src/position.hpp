#pragma once
#include "pregen.hpp"

// global variables
MATRIX g_board { };
TEAM g_team { };
HASH g_hashkey { };

// some local variables and methods

// history moves and maintaining all pieces on board
std::vector<Move> history_moves_ { };
std::vector<PTYPE> history_captures_ { };
std::vector<HASH> history_hashkeys_ { };
std::vector<POS> pos_list_r_ { };
std::vector<POS> pos_list_b_ { };
std::array<PID, 90> pos_pid_r_ { };
std::array<PID, 90> pos_pid_b_ { };
std::array<UINT16, 9> bl10_container { };
std::array<UINT16, 10> bl9_container { };

// get all live pieces of current team
std::vector<POS> get_pos_list()
{
    return g_team == R ? pos_list_r_ : pos_list_b_;
}

// return team differences based on wheter you want to gen captures or not
template <bool G>
bool team_diff(POS p) { return G ? g_team * g_board[p] < 0 : !g_board[p]; }

// get piece on pos
PTYPE piece_on(POS p) { return p < 90 ? g_board[p] : 0; }

// get bl10 from the table
UINT16 get_bl10(POS pos) { return bl10_container[pos % 9]; }

// get_bl9 from the table
UINT16 get_bl9(POS pos) { return bl9_container[pos / 9]; }

// judge face-kings
bool face_king_()
{
    if (pos_list_r_[0] % 9 == pos_list_b_[0] % 9) {
        switch (get_bl10(pos_list_r_[0])) {
        case 0b1000000001:
        case 0b1000000010:
        case 0b1000000100:
        case 0b0100000001:
        case 0b0100000010:
        case 0b0100000100:
        case 0b0010000001:
        case 0b0010000010:
        case 0b0010000100:
            return true;
        }
    }
    return false;
}

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
            bl10_container[i % 9] |= 1 << (i / 9);
            bl9_container[i / 9] |= 1 << (i % 9);
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
            pos_pid_b_[pos_list_b_.back()] = pos_pid_b_[move.end];
            pos_list_b_.pop_back();
            pos_pid_b_[move.end] = 0;
        }
    } else {
        pos_list_b_[pos_pid_b_[move.beg]] = move.end;
        std::swap(pos_pid_b_[move.end], pos_pid_b_[move.beg]);
        if (g_board[move.end]) {
            pos_list_r_[pos_pid_r_[move.end]] = pos_list_r_.back();
            pos_pid_r_[pos_list_r_.back()] = pos_pid_r_[move.end];
            pos_list_r_.pop_back();
            pos_pid_r_[move.end] = 0;
        }
    }
    bl10_container[move.end % 9] |= 1 << (move.end / 9);
    bl9_container[move.end / 9] |= 1 << (move.end % 9);
    bl10_container[move.beg % 9] &= ~(1 << (move.beg / 9));
    bl9_container[move.beg / 9] &= ~(1 << (move.beg % 9));
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
    bl10_container[move.end % 9] &= captured ? ~(1 << (move.end / 9)) : 0xfff;
    bl9_container[move.end / 9] &= captured ? ~(1 << (move.end % 9)) : 0xfff;
    bl10_container[move.beg % 9] |= 1 << (move.beg / 9);
    bl9_container[move.beg / 9] |= 1 << (move.beg % 9);
    g_board[move.beg] = g_board[move.end];
    g_board[move.end] = captured;
    g_hashkey = hashkey;
    g_team = -g_team;
}

// judge whether current position is in check (include face-kings)
bool in_check()
{
    // get king pos
    const POS pos = g_team == R ? pos_list_r_[0] : pos_list_b_[0];
    // is attacked by a pawn (judging directly without validating team is ok)
    if (abs(piece_on(pos - 9 * g_team)) == R_PAWN) return true;
    if (abs(piece_on(pos - 1)) == R_PAWN) return true;
    if (abs(piece_on(pos + 1)) == R_PAWN) return true;
    // is attacked by a knight
    if (!piece_on(pos - 10)) {
        if (piece_on(pos - 19) * g_team == -R_KNIGHT) return true;
        if (piece_on(pos - 11) * g_team == -R_KNIGHT) return true;
    }
    if (!piece_on(pos + 10)) {
        if (piece_on(pos + 19) * g_team == -R_KNIGHT) return true;
        if (piece_on(pos + 11) * g_team == -R_KNIGHT) return true;
    }
    if (!piece_on(pos + 8)) {
        if (piece_on(pos + 17) * g_team == -R_KNIGHT) return true;
        if (piece_on(pos + 7) * g_team == -R_KNIGHT) return true;
    }
    if (!piece_on(pos - 8)) {
        if (piece_on(pos - 17) * g_team == -R_KNIGHT) return true;
        if (piece_on(pos - 7) * g_team == -R_KNIGHT) return true;
    }
    // is attacked by a rook or cannon
    const auto [left, right] = rook_9(get_bl9(pos), pos);
    const auto [top, bottom] = rook_10(get_bl10(pos), pos);
    if (piece_on(left) * g_team == -R_ROOK) return true;
    if (piece_on(right) * g_team == -R_ROOK) return true;
    if (piece_on(top) * g_team == -R_ROOK) return true;
    if (piece_on(bottom) * g_team == -R_ROOK) return true;
    const auto [left2, right2] = cannon_9(get_bl9(pos), pos);
    const auto [top2, bottom2] = cannon_10(get_bl10(pos), pos);
    if (left < 90 && piece_on(left) * g_team == -R_CANNON)
        return true;
    if (right2 < 90 && piece_on(right2) * g_team == -R_CANNON)
        return true;
    if (top2 < 90 && piece_on(top2) * g_team == -R_CANNON)
        return true;
    if (bottom2 < 90 && piece_on(bottom2) * g_team == -R_CANNON)
        return true;
    // is faced by the opposite king
    return face_king_();
}

// judge whether a move is valid or not in situation (include in-check)
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
        } else if (piece_on(move.beg - 1)) {
            return false;
        }
    } else if (abs(p) == R_CANNON) { // cannon moves
        const int d = move.end - move.beg;
        if (move.beg % 9 != move.end % 9 && move.beg / 9 != move.end / 9)
            return false; // not in the same col or same row
        if (-9 < d && d < 9) { // horizontal move
            const auto [left, right] = cannon_9(get_bl9(move.beg), move.beg);
            const auto [left2, right2] = rook_9(get_bl9(move.end), move.end);
            if (!(left2 < move.end && move.end < right2)) {
                if (move.end != left && move.end != right) return false;
            }
        } else { // vertical move
            const auto [up, down] = cannon_10(get_bl10(move.beg), move.beg);
            const auto [up2, down2] = rook_10(get_bl10(move.end), move.end);
            if (!(up2 < move.end && move.end < down2)) {
                if (move.end != up && move.end != down) return false;
            }
        }
    } else if (abs(p) == R_ROOK) { // rook moves
        const int d = move.end - move.beg;
        if (move.beg % 9 != move.end % 9 || move.beg / 9 != move.end / 9)
            return false; // not in the same col or same row
        if (-9 < d && d < 9) { // horizontal move
            const auto [left, right] = rook_9(get_bl9(move.beg), move.beg);
            if (!(left < move.end && move.end < right)) return false;
        } else { // vertical move
            const auto [up, down] = rook_10(get_bl10(move.beg), move.beg);
            if (!(up < move.end && move.end < down)) return false;
        }
    }
    return true;
}

// calculate whether a pos can be attacked by a current team piece
POS get_protector(POS pos)
{
    // pawn protector
    if (piece_on(pos - 9 * g_team) * g_team == R_PAWN) return pos - 9 * g_team;
    if ((pos / 9 < 5 && g_team == R) || (pos / 9 > 4 && g_team == B)) {
        if (piece_on(pos - 1) * g_team == R_PAWN) return pos - 1;
        if (piece_on(pos + 1) * g_team == R_PAWN) return pos + 1;
    }
    // knight protector
    if (!piece_on(pos - 10)) {
        if (piece_on(pos - 19) * g_team == R_KNIGHT) return pos - 19;
        if (piece_on(pos - 11) * g_team == R_KNIGHT) return pos - 11;
    }
    if (!piece_on(pos + 10)) {
        if (piece_on(pos + 19) * g_team == R_KNIGHT) return pos + 19;
        if (piece_on(pos + 11) * g_team == R_KNIGHT) return pos + 11;
    }
    if (!piece_on(pos + 8)) {
        if (piece_on(pos + 17) * g_team == R_KNIGHT) return pos + 17;
        if (piece_on(pos + 7) * g_team == R_KNIGHT) return pos + 7;
    }
    if (!piece_on(pos - 8)) {
        if (piece_on(pos - 17) * g_team == R_KNIGHT) return pos - 17;
        if (piece_on(pos - 7) * g_team == R_KNIGHT) return pos - 7;
    }
    // rook or cannon protector
    const auto [left, right] = rook_9(get_bl9(pos), pos);
    const auto [top, bottom] = rook_10(get_bl10(pos), pos);
    if (piece_on(left) * g_team == R_ROOK) return left;
    if (piece_on(right) * g_team == R_ROOK) return right;
    if (piece_on(top) * g_team == R_ROOK) return top;
    if (piece_on(bottom) * g_team == R_ROOK) return bottom;
    const auto [left2, right2] = cannon_9(get_bl9(pos), pos);
    const auto [top2, bottom2] = cannon_10(get_bl10(pos), pos);
    if (left < 90 && piece_on(left2) * g_team == R_CANNON)
        return left2;
    if (right2 < 90 && piece_on(right2) * g_team == R_CANNON)
        return right2;
    if (top2 < 90 && piece_on(top2) * g_team == R_CANNON)
        return top2;
    if (bottom2 < 90 && piece_on(bottom2) * g_team == R_CANNON)
        return bottom2;
    // bishop protector
    if (g_team == R && pos / 9 > 4 || g_team == B && pos / 9 < 5) {
        if (!piece_on(pos - 10) && piece_on(pos - 20) * g_team == R_BISHOP)
            return pos - 20;
        if (!piece_on(pos - 8) && piece_on(pos - 16) * g_team == R_BISHOP)
            return pos - 16;
        if (piece_on(pos + 20) * g_team == R_BISHOP) return pos + 20;
        if (piece_on(pos + 16) * g_team == R_BISHOP) return pos + 16;
    }
    // king or advisor protector
    const bool c1 = g_team == R && pos / 9 > 6 && pos % 9 > 2 && pos % 9 < 5;
    const bool c2 = g_team == B && pos / 9 < 3 && pos % 9 > 2 && pos % 9 < 5;
    if ((c1) || (c2)) {
        if (piece_on(pos - 9) * g_team == R_KING) return pos - 9;
        if (piece_on(pos + 9) * g_team == R_KING) return pos + 9;
        if (piece_on(pos - 1) * g_team == R_KING) return pos - 1;
        if (piece_on(pos + 1) * g_team == R_KING) return pos + 1;
        if (piece_on(pos - 10) * g_team == R_ADVISOR) return pos - 10;
        if (piece_on(pos - 8) * g_team == R_ADVISOR) return pos - 8;
        if (piece_on(pos + 10) * g_team == R_ADVISOR) return pos + 10;
        if (piece_on(pos + 8) * g_team == R_ADVISOR) return pos + 8;
    }
    return INVALID_POS;
}
