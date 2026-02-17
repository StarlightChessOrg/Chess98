#include "hash.hpp"

namespace {

MATRIX board {};
TEAM team { 0 };
// pindex
PIECES pieces {};
MATRIX pos_pindex {};
// pid_pindeces const
std::array<std::vector<PINDEX>, 15> pid_pindeces {};
// history
MOVES history_moves {};
PINDECES history_captured_pindeces {};
// hash
std::vector<int> history_hashkey {};
std::vector<int> history_hashlock {};
int hashkey { 0 };
int hashlock { 0 };

}

namespace position {

void init(const MATRIX& matrix, TEAM team)
{
    // board and team
    board = matrix;
    team = team;
    // history_moves and history_captured_pindeces
    history_moves.clear();
    history_moves.reserve(256);
    history_captured_pindeces.clear();
    history_captured_pindeces.reserve(256);
    for (POS i = 0; i < 90; i++) {
        if (matrix[i] != 0) {
            // pindex and ptypes
            const PINDEX& size = pieces.size();
            pid_pindeces[matrix[i] + 7].emplace_back(size);
            pos_pindex[i] = size;
            pieces.emplace_back(Piece { matrix[i], size, i });
            // hash
            hashkey ^= hash::key_on(matrix[i], i);
            hashlock ^= hash::lock_on(matrix[i], i);
        }
    }
}

void do_move(Move move)
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
    team = -team;
    // hash update
    hashkey ^= hash::key_on(board[move.beg], move.beg);
    hashlock ^= hash::lock_on(board[move.beg], move.beg);
}

void undo_move()
{
    const Move& move = history_moves.back();
    const PINDEX& captured_pindex = history_captured_pindeces.back();
    // pindex update
    pieces[pos_pindex[move.end]].pos = move.beg;
    pieces[captured_pindex].live = true;
    pos_pindex[move.beg] = pos_pindex[move.end];
    pos_pindex[move.end] = captured_pindex;
    // board update
    board[move.beg] = board[move.end];
    board[move.end] = pieces[captured_pindex].pid;
    team = -team;
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
