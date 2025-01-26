#pragma once
#include <windows.h>
#include <cstring>
#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <array>
#include <chrono>
#include <map>
#include <cassert>

using PIECE_INDEX = int;
using U64 = unsigned long long;

using PIECEID = int;
const PIECEID EMPTY_PIECEID = 0;
const PIECEID R_KING = 1;
const PIECEID R_GUARD = 2;
const PIECEID R_BISHOP = 3;
const PIECEID R_KNIGHT = 4;
const PIECEID R_ROOK = 5;
const PIECEID R_CANNON = 6;
const PIECEID R_PAWN = 7;
const PIECEID B_KING = -1;
const PIECEID B_GUARD = -2;
const PIECEID B_BISHOP = -3;
const PIECEID B_KNIGHT = -4;
const PIECEID B_ROOK = -5;
const PIECEID B_CANNON = -6;
const PIECEID B_PAWN = -7;
const PIECEID OVERFLOW_PIECEID = 8;

using TEAM = int;
const TEAM EMPTY_TEAM = 0;
const TEAM RED = 1;
const TEAM BLACK = -1;
const TEAM OVERFLOW_TEAM = 2;

using PIECEID_MAP = std::array<std::array<PIECEID, 10>, 9>;
PIECEID_MAP DEFAULT_MAP{
    {{R_ROOK, 0, 0, R_PAWN, 0, 0, B_PAWN, 0, 0, B_ROOK},
     {R_KNIGHT, 0, R_CANNON, 0, 0, 0, 0, B_CANNON, 0, B_KNIGHT},
     {R_BISHOP, 0, 0, R_PAWN, 0, 0, B_PAWN, 0, 0, B_BISHOP},
     {R_GUARD, 0, 0, 0, 0, 0, 0, 0, 0, B_GUARD},
     {R_KING, 0, 0, R_PAWN, 0, 0, B_PAWN, 0, 0, B_KING},
     {R_GUARD, 0, 0, 0, 0, 0, 0, 0, 0, B_GUARD},
     {R_BISHOP, 0, 0, R_PAWN, 0, 0, B_PAWN, 0, 0, B_BISHOP},
     {R_KNIGHT, 0, R_CANNON, 0, 0, 0, 0, B_CANNON, 0, B_KNIGHT},
     {R_ROOK, 0, 0, R_PAWN, 0, 0, B_PAWN, 0, 0, B_ROOK}}};
     
const int INF = 1000000;
const int BAN = INF - 2000;

/// @brief 棋子类
class Piece
{
public:
    Piece(PIECEID pieceid, int x, int y, PIECE_INDEX pieceIndex)
        : pieceid(pieceid),
          x(x),
          y(y),
          pieceIndex(pieceIndex) {}

    TEAM getTeam() const
    {
        if (this->pieceid == EMPTY_PIECEID)
        {
            return EMPTY_TEAM;
        }
        else if (this->pieceid == OVERFLOW_PIECEID)
        {
            return OVERFLOW_TEAM;
        }
        else if (this->pieceid > 0)
        {
            return RED;
        }
        else
        {
            return BLACK;
        }
    }

    PIECEID pieceid = EMPTY_PIECEID;
    int x = -1;
    int y = -1;
    PIECE_INDEX pieceIndex = -1;
    bool isLive = true;
};

/// @brief 着法类
class Move
{
public:
    Move() {}
    Move(int x1, int y1, int x2, int y2, int val = 0)
        : x1(x1),
          y1(y1),
          x2(x2),
          y2(y2),
          id(x1 * 1000 + y1 * 100 + x2 * 10 + y2),
          val(val) {}

    int x1 = -1;
    int y1 = -1;
    int x2 = -1;
    int y2 = -1;
    int id = -1;
    int val = 0;

    bool operator==(Move move)
    {
        return this->id == move.id;
    }

    bool operator!=(Move move)
    {
        return this->id != move.id;
    }
};

using MOVES = std::vector<Move>;

/// @brief 获取棋子名称
/// @param pieceid
/// @return
std::string getPieceName(PIECEID pieceid)
{
    if (pieceid == R_KING)
        return "RK";
    else if (pieceid == R_GUARD)
        return "RG";
    else if (pieceid == R_ROOK)
        return "RR";
    else if (pieceid == R_BISHOP)
        return "RB";
    else if (pieceid == R_KNIGHT)
        return "RN";
    else if (pieceid == R_CANNON)
        return "RC";
    else if (pieceid == R_PAWN)
        return "RP";
    else if (pieceid == B_KING)
        return "BK";
    else if (pieceid == B_GUARD)
        return "BG";
    else if (pieceid == B_ROOK)
        return "BR";
    else if (pieceid == B_BISHOP)
        return "BB";
    else if (pieceid == B_KNIGHT)
        return "BN";
    else if (pieceid == B_CANNON)
        return "BC";
    else if (pieceid == B_PAWN)
        return "BP";
    else
        return "  ";
}

using TIME_T = long long;

/// @brief 获取当前时间戳（毫秒）
/// @return
TIME_T getCurrentTimeWithMS()
{
    // 获取当前时间戳
    auto now = std::chrono::system_clock::now();

    // 将时间戳转换为毫秒数
    auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
    auto value = now_ms.time_since_epoch().count();
    return TIME_T(value);
}

/// @brief 获取一个64位的随机整数
/// @return
U64 rand64()
{
    return rand() ^ ((U64)rand() << 15) ^ ((U64)rand() << 30) ^ ((U64)rand() << 45) ^ ((U64)rand() << 60);
}

U64 zobristMap[7][2][9][10]{};

/// @brief 初始化
void initZobrist()
{
    memset(zobristMap, static_cast<U64>(0), sizeof(U64) * 7 * 2 * 9 * 10);
}
