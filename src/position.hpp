#pragma once
#include "base.hpp"

extern MATRIX g_board;
extern TEAM g_team;
extern HASH g_hashkey;
extern std::vector<bool> history_checkings;
extern std::vector<Move> history_moves_;
extern std::vector<PTYPE> history_captures_;
extern std::vector<HASH> history_hashkeys_;
extern std::vector<POS> pos_list_r_;
extern std::vector<POS> pos_list_b_;
extern std::array<PID, 90> pos_pid_r_;
extern std::array<PID, 90> pos_pid_b_;
extern std::array<UINT16, 9> bl10_container;
extern std::array<UINT16, 10> bl9_container;

void position_init(const MATRIX& board, TEAM team);
std::vector<POS> pos_list();
template <bool G>
inline bool teamcheck(POS p) { return G ? g_team * g_board[p] < 0 : !g_board[p]; }
PTYPE piece_on(POS p);
UINT16 get_bl10(POS pos);
UINT16 get_bl9(POS pos);
bool face_king_();
void position_move(Move move);
void position_undo();
bool in_check();
bool legal_move(Move move);
POS get_protector(POS pos);
