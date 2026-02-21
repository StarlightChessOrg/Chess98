#pragma once
#include "hash.hpp"

namespace position {

// board and team
MATRIX board {};
TEAM board_team { 0 };
// pindex
PIECES pieces {};
MATRIX pos_pindex {};
// pid_pindeces const
std::array<std::vector<PINDEX>, 15> pid_pindeces_table {};
PINDEX r_king_index { 0 };
PINDEX b_king_index { 0 };
// history
MOVES history_moves {};
PINDECES history_captured_pindeces {};
// hash
std::vector<int> history_hashkey {};
std::vector<int> history_hashlock {};
int hashkey { 0 };
int hashlock { 0 };

void init(const MATRIX& matrix, TEAM t)
{
    assert(t == R || t == B);
    // board and team
    board = matrix;
    board_team = t;
    // history_moves and history_captured_pindeces
    history_moves.clear();
    history_moves.reserve(256);
    history_captured_pindeces.clear();
    history_captured_pindeces.reserve(256);
    pieces.clear();
    pieces.emplace_back(Piece {});
    for (POS i = 0; i < 90; i++) {
        assert(-7 <= matrix[i] && matrix[i] <= 7);
        // set king pindex
        if (matrix[i] == R_KING)
        {
            r_king_index = static_cast<PINDEX>(pieces.size());
        }
        else if (matrix[i] == B_KING)
        {
            b_king_index = static_cast<PINDEX>(pieces.size());
        }
        if (matrix[i] != 0) {
            // pindex and ptypes
            const PINDEX size = static_cast<PINDEX>(pieces.size());
            const size_t k = static_cast<size_t>(matrix[i] + 7);
            pid_pindeces_table[k].emplace_back(size);
            pos_pindex[i] = size;
            pieces.emplace_back(Piece { matrix[i], size, i });
            // hash
            hashkey ^= hash::key_on(matrix[i], i);
            hashlock ^= hash::lock_on(matrix[i], i);
        }
    }
}

void move(Move move)
{
    // history and hash update
    history_moves.emplace_back(move);
    history_captured_pindeces.emplace_back(pos_pindex[move.end]);
    history_hashkey.emplace_back(hashkey);
    history_hashlock.emplace_back(hashlock);
    // pindex update
    pieces[pos_pindex[move.end]].live = board[move.end] == 0;
    pieces[pos_pindex[move.beg]].pos = move.end;
    pos_pindex[move.end] = pos_pindex[move.beg];
    pos_pindex[move.beg] = 0;
    // board update
    board[move.end] = board[move.beg];
    board[move.beg] = 0;
    board_team = -board_team;
    // hash update
    hashkey ^= hash::key_on(board[move.beg], move.beg);
    hashlock ^= hash::lock_on(board[move.beg], move.beg);
}

void undo_move()
{
    assert(!history_moves.empty());
    const Move move = history_moves.back();
    const PINDEX captured_pindex = history_captured_pindeces.back();
    // pindex update
    pieces[pos_pindex[move.end]].pos = move.beg;
    pieces[captured_pindex].live = true;
    pos_pindex[move.beg] = pos_pindex[move.end];
    pos_pindex[move.end] = captured_pindex;
    // board update
    board[move.beg] = board[move.end];
    board[move.end] = pieces[captured_pindex].pid;
    board_team = -board_team;
    // hash update
    hashkey = history_hashkey.back();
    hashlock = history_hashlock.back();
    // history update
    history_moves.pop_back();
    history_captured_pindeces.pop_back();
    history_hashkey.pop_back();
    history_hashlock.pop_back();
}

}

TEAM team_now()
{
    return position::board_team;
}

PID pid_on(int pos)
{
    assert(0 <= pos && pos < 90);
    return position::board[static_cast<size_t>(pos)];
}

TEAM same_team(int t, int pos)
{
    assert(0 <= pos && pos < 90);
    assert(t == R || t == B || t == 0);
    return position::board[static_cast<size_t>(pos)] * t > 0;
}

PID pindex_pid(PINDEX pindex)
{
    assert(pindex < position::pieces.size());
    return position::pieces[static_cast<size_t>(pindex)].pid;
}

Piece pindex_piece(PINDEX pindex)
{
    assert(pindex < position::pieces.size());
    return position::pieces[static_cast<size_t>(pindex)];
}

PINDECES pid_pindeces(PID pid)
{
    return position::pid_pindeces_table[pid + 7];
}
