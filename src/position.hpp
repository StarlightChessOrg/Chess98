#pragma once
#include "base.hpp"

MATRIX g_board { };
TEAM g_team { };
HASH g_hashkey { };
VL g_evaluation { };
std::vector<bool> g_history_checkings { };
std::vector<Move> history_moves_ { };
std::vector<PTYPE> history_captures_ { };
std::vector<HASH> history_hashkeys_ { };
std::vector<POS> pos_list_r_ { };
std::vector<POS> pos_list_b_ { };
std::array<PID, 90> pos_pid_r_ { };
std::array<PID, 90> pos_pid_b_ { };
std::array<UINT16, 9> bl10_items_ { };
std::array<UINT16, 10> bl9_items_ { };

void position_init(const MATRIX& board, TEAM team);
std::vector<POS> get_pos_list();
template <GENTYPE G>
bool targetchk(POS p);
PTYPE piece_on(POS p);
UINT16 get_bl10(POS pos);
UINT16 get_bl9(POS pos);
bool face_king_();
void position_move(Move move);
void position_undo();
void position_do_null();
void position_undo_null();
bool in_check();
bool legal_move(Move move);
POS get_protector(POS pos);
bool is_repeat();

// reset the evaluation accumulators
void eval_reset_() { g_evaluation = 0; }

void eval_add_piece_(TEAM team, PTYPE p, POS pos)
{
    g_evaluation = VL(g_evaluation + team * pst_of_(team, std::abs(p), pos));
}

void eval_remove_piece_(TEAM team, PTYPE p, POS pos)
{
    g_evaluation = VL(g_evaluation - team * pst_of_(team, std::abs(p), pos));
}

void eval_slide_piece_(TEAM team, PTYPE p, POS from, POS to)
{
    g_evaluation = VL(g_evaluation
        + team * (pst_of_(team, std::abs(p), to) - pst_of_(team, std::abs(p), from)));
}

// init global position
void position_init(const MATRIX& board, TEAM team)
{
    // variables init
    history_moves_.clear();
    history_captures_.clear();
    history_hashkeys_.clear();
    g_history_checkings.clear();
    pos_list_r_ = pos_list_b_ = { };
    pos_pid_r_ = pos_pid_b_ = { };
    bl10_items_ = { };
    bl9_items_ = { };
    g_hashkey = 0;
    g_board = board;
    g_team = team;
    if (team == B) g_hashkey ^= SIDE_KEY;
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
        g_hashkey ^= HASH_KEYS[size_t(board[i] + 7)][i];
        if (board[i] != 0) {
            bl10_items_[i % 9] |= 1 << (i / 9);
            bl9_items_[i / 9] |= 1 << (i % 9);
        }
        if (board[i] > 0 && board[i] != R_KING) {
            pos_pid_r_[i] = PID(pos_list_r_.size());
            pos_list_r_.emplace_back(i);
        }
        if (board[i] < 0 && board[i] != B_KING) {
            pos_pid_b_[i] = PID(pos_list_b_.size());
            pos_list_b_.emplace_back(i);
        }
    }
    // vector reservations
    history_moves_.reserve(256);
    history_captures_.reserve(256);
    history_hashkeys_.reserve(256);
    g_history_checkings.reserve(256);
    // evaluation accumulators from scratch
    eval_reset_();
    for (POS i = 0; i < 90; i++) {
        if (board[i]) eval_add_piece_(board[i] > 0 ? R : B, board[i], i);
    }
}

// get all live pieces of current team
std::vector<POS> get_pos_list()
{
    return g_team == R ? pos_list_r_ : pos_list_b_;
}

// return team differences based on wheter you want to gen captures or not
template <GENTYPE G>
bool targetchk(POS p)
{
    if (p >= 90) return false;
    const PTYPE target = g_board[p];
    if (std::abs(target) == R_KING) return false;
    if constexpr (G == CAPTURE) {
        return target * g_team < 0;
    } else if constexpr (G == QUIET) {
        return target == 0;
    } else {
        return target * g_team <= 0;
    }
}

// get piece on pos
PTYPE piece_on(POS p) { 
    return p < 90 ? g_board[p] : 0; }

// get bl10 from the table
UINT16 get_bl10(POS pos) { 
    return bl10_items_[pos % 9];
 }

// get_bl9 from the table
UINT16 get_bl9(POS pos) { 
    return bl9_items_[pos / 9];
 }

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

// do move
void position_move(Move move)
{
    assert(move && g_board[move.beg] * g_team > 0);
    assert(g_board[move.end] * g_team <= 0 && abs(g_board[move.end]) != R_KING);
    assert(!pos_list_r_.empty() && !pos_list_b_.empty());
    // maintain the history (checkings aligned with moves; set by search if move gives check)
    history_moves_.emplace_back(move);
    history_captures_.emplace_back(g_board[move.end]);
    history_hashkeys_.emplace_back(g_hashkey);
    g_history_checkings.emplace_back(false);
    // evaluation incremental tracking
    eval_slide_piece_(g_team, g_board[move.beg], move.beg, move.end);
    if (g_board[move.end]) eval_remove_piece_(TEAM(-g_team), g_board[move.end], move.end);
    // hash
    g_hashkey ^= HASH_KEYS[size_t(g_board[move.beg] + 7)][move.beg];
    g_hashkey ^= HASH_KEYS[size_t(g_board[move.end] + 7)][move.end];
    g_hashkey ^= HASH_KEYS[size_t(g_board[move.beg] + 7)][move.end];
    g_hashkey ^= SIDE_KEY;
    // pos list tracking (core part to avoid 90-square scanning)
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
    // bitline
    bl10_items_[move.end % 9] |= 1 << (move.end / 9);
    bl9_items_[move.end / 9] |= 1 << (move.end % 9);
    bl10_items_[move.beg % 9] &= ~(1 << (move.beg / 9));
    bl9_items_[move.beg / 9] &= ~(1 << (move.beg % 9));
    // global updates
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
    // undo the history
    history_moves_.pop_back();
    history_captures_.pop_back();
    history_hashkeys_.pop_back();
    g_history_checkings.pop_back();
    // evaluation incremental undo (g_team is still the victim side here)
    eval_slide_piece_(TEAM(-g_team), g_board[move.end], move.end, move.beg);
    if (captured) eval_add_piece_(g_team, captured, move.end);
    // hash
    g_hashkey = hashkey;
    // maintain the tracking
    if (g_team == R) {
        pos_list_b_[pos_pid_b_[move.end]] = move.beg;
        std::swap(pos_pid_b_[move.end], pos_pid_b_[move.beg]);
        if (captured) {
            pos_pid_r_[move.end] = PID(pos_list_r_.size());
            pos_list_r_.emplace_back(move.end);
        }
    } else {
        pos_list_r_[pos_pid_r_[move.end]] = move.beg;
        std::swap(pos_pid_r_[move.end], pos_pid_r_[move.beg]);
        if (captured) {
            pos_pid_b_[move.end] = PID(pos_list_b_.size());
            pos_list_b_.emplace_back(move.end);
        }
    }
    // bitline undo
    bl10_items_[move.end % 9] &= (!captured) ? ~(1 << (move.end / 9)) : 0xfff;
    bl9_items_[move.end / 9] &= (!captured) ? ~(1 << (move.end % 9)) : 0xfff;
    bl10_items_[move.beg % 9] |= 1 << (move.beg / 9);
    bl9_items_[move.beg / 9] |= 1 << (move.beg % 9);
    // global updates undo
    g_board[move.beg] = g_board[move.end];
    g_board[move.end] = captured;
    g_team = -g_team;
}

// null move: pass the turn (no piece change; hash side-bit only)
void position_do_null()
{
    g_hashkey ^= SIDE_KEY;
    g_team = -g_team;
}

void position_undo_null()
{
    g_team = -g_team;
    g_hashkey ^= SIDE_KEY;
}

// judge whether current position is in check (include face-kings)
bool in_check()
{
    // get king pos
    const POS pos = g_team == R ? pos_list_r_[0] : pos_list_b_[0];
    assert(pos % 9 >= 3 && pos % 9 <= 5);
    assert((g_team == R && pos / 9 >= 7) || (g_team == B && pos / 9 <= 2));
    assert(std::abs(piece_on(pos)) == R_KING);
    // is attacked by an enemy pawn
    if (piece_on(pos - 9 * g_team) * g_team == B_PAWN) return true;
    if (piece_on(pos - 1) * g_team == B_PAWN) return true;
    if (piece_on(pos + 1) * g_team == B_PAWN) return true;
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
    if (left < 90 && piece_on(left2) * g_team == -R_CANNON)
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
    // piece not exists or opposite, or same-team attack, or eat the king
    if (g_team * piece_on(move.end) > 0 || p * g_team <= 0) return false;
    if (std::abs(piece_on(move.end)) == R_KING) return false;
    // specific piece legal judge
    if (abs(p) == R_KING) {
        const int d = int(move.end) - int(move.beg);
        if (d != 1 && d != -1 && d != 9 && d != -9) return false;
        if (move.end % 9 < 3 || move.end % 9 > 5) return false;
        if (p == R_KING && move.end / 9 < 7) return false;
        if (p == B_KING && move.end / 9 > 2) return false;
    } else if (abs(p) == R_ADVISOR) {
        const int d = int(move.end) - int(move.beg);
        if (d != 10 && d != -10 && d != 8 && d != -8) return false;
        if (move.end != 13 && move.end != 76
            && !(move.end % 9 >= 3 && move.end % 9 <= 5
                && ((p == R_ADVISOR && move.end / 9 >= 7)
                    || (p == B_ADVISOR && move.end / 9 <= 2))))
            return false;
        // must stay in own palace (no black advisor to 76 / red to 13)
        if (p == R_ADVISOR && move.end / 9 < 7) return false;
        if (p == B_ADVISOR && move.end / 9 > 2) return false;
    } else if (abs(p) == R_BISHOP) { // elephant eyes
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
// if a piece has multi protectors, return the least valuable one (LVA order)
POS get_protector(POS pos)
{
    // pawn protector
    if (piece_on(pos - 9 * g_team) * g_team == R_PAWN) return pos - 9 * g_team;
    if ((pos / 9 < 5 && g_team == R) || (pos / 9 > 4 && g_team == B)) {
        if (piece_on(pos - 1) * g_team == R_PAWN) return pos - 1;
        if (piece_on(pos + 1) * g_team == R_PAWN) return pos + 1;
    }
    // advisor protector
    const bool c1 = g_team == R && pos / 9 > 6 && pos % 9 > 2 && pos % 9 < 5;
    const bool c2 = g_team == B && pos / 9 < 3 && pos % 9 > 2 && pos % 9 < 5;
    if ((c1) || (c2)) {
        if (piece_on(pos - 10) * g_team == R_ADVISOR) return pos - 10;
        if (piece_on(pos - 8) * g_team == R_ADVISOR) return pos - 8;
        if (piece_on(pos + 10) * g_team == R_ADVISOR) return pos + 10;
        if (piece_on(pos + 8) * g_team == R_ADVISOR) return pos + 8;
    }
    // bishop protector
    if (g_team == R && pos / 9 > 4 || g_team == B && pos / 9 < 5) {
        if (!piece_on(pos - 10) && piece_on(pos - 20) * g_team == R_BISHOP)
            return pos - 20;
        if (!piece_on(pos - 8) && piece_on(pos - 16) * g_team == R_BISHOP)
            return pos - 16;
        if (piece_on(pos + 20) * g_team == R_BISHOP) return pos + 20;
        if (piece_on(pos + 16) * g_team == R_BISHOP) return pos + 16;
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
    // cannon then rook (LVA: cannon cheaper than rook)
    const auto [left, right] = rook_9(get_bl9(pos), pos);
    const auto [top, bottom] = rook_10(get_bl10(pos), pos);
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
    if (piece_on(left) * g_team == R_ROOK) return left;
    if (piece_on(right) * g_team == R_ROOK) return right;
    if (piece_on(top) * g_team == R_ROOK) return top;
    if (piece_on(bottom) * g_team == R_ROOK) return bottom;
    // king protector (last)
    if ((c1) || (c2)) {
        if (piece_on(pos - 9) * g_team == R_KING) return pos - 9;
        if (piece_on(pos + 9) * g_team == R_KING) return pos + 9;
        if (piece_on(pos - 1) * g_team == R_KING) return pos - 1;
        if (piece_on(pos + 1) * g_team == R_KING) return pos + 1;
    }
    return INVALID_POS;
}

// repeat validation
bool is_repeat()
{
    const size_t n = history_hashkeys_.size();
    if (n < 4 || g_history_checkings.size() != n) return false;
    for (size_t i = n; i-- > 0; ) {
        if (history_hashkeys_[i] == g_hashkey) {
            if (n - i < 4) return false;
            for (size_t j = i; j < n; j += 2) // 己方着：i, i+2, ...
                if (!g_history_checkings[j]) return false;
            return true;
        }
        if (history_captures_[i]) break;
    }
    return false;
}
