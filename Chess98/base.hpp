#pragma once
#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <cstring>
#include <ctime>
#include <iostream>
#include <map>
#include <random>
#include <string>
#include <vector>
#include <functional>
#include <chrono>
#include <unordered_map>
#include <fstream>
#ifdef _WIN32
#include <windows.h>
#elif __unix__
#include <unistd.h>
#endif

class Piece;
class Move;
class Result;
class TrickResult;
class TransItem;
void wait(int ms);
void command(std::string str);
void readFile(std::string filename, std::string &content);
void writeFile(std::string filename, std::string content);
enum MOVE_TYPE;
enum NODE_TYPE;
enum SEARCH_TYPE;
using uint64 = unsigned long long;
using uint32 = unsigned int;
using int32 = int;
using HASH_KEY_MAP = const std::array<std::array<int32, 10>, 9>;
using PIECE_INDEX = int;
using PIECEID = int;
using TEAM = int;
using PIECEID_MAP = std::array<std::array<PIECEID, 10>, 9>;
using PIECE_TARGET_MAP = std::array<std::array<bool, 10>, 9>;
using PIECES = std::vector<Piece>;
using MOVES = std::vector<Move>;
const int INF = 1000000;
const int BAN = INF - 2000;
const int ILLEGAL_VAL = INF * 2;
const PIECE_INDEX EMPTY_INDEX = -1;
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
const TEAM EMPTY_TEAM = 0;
const TEAM RED = 1;
const TEAM BLACK = -1;
const TEAM OVERFLOW_TEAM = 2;

enum MOVE_TYPE
{
    NORMAL = 0,
    HISTORY = 1,
    CAPTURE = 2,
    KILLER = 3,
    HASH = 4
};

enum NODE_TYPE
{
    NONE_TYPE = 0,
    ALPHA_TYPE = 1,
    BETA_TYPE = 2,
    EXACT_TYPE = 3,
};

enum SEARCH_TYPE
{
    ROOT = 0,
    PV = 1,
    CUT = 2,
    QUIESC = 3
};

class Piece
{
public:
    Piece() = default;
    Piece(PIECEID pieceid) : pieceid(pieceid) {}
    Piece(PIECEID pieceid, int x, int y, PIECE_INDEX pieceIndex)
        : pieceid(pieceid),
          x(x),
          y(y),
          pieceIndex(pieceIndex)
    {
        if (this->pieceid == EMPTY_PIECEID)
        {
            this->team = EMPTY_TEAM;
        }
        else if (this->pieceid == OVERFLOW_PIECEID)
        {
            this->team = OVERFLOW_TEAM;
        }
        else if (this->pieceid > 0)
        {
            this->team = RED;
        }
        else
        {
            this->team = BLACK;
        }
        this->isLive = true;
    }

public:
    PIECEID pieceid = EMPTY_PIECEID;
    int x = -1;
    int y = -1;
    PIECE_INDEX pieceIndex = -1;
    int team = EMPTY_TEAM;
    bool isLive = false;
};

class Move
{
public:
    Move() = default;

    Move(int x1, int y1, int x2, int y2, int val = 0, MOVE_TYPE moveType = NORMAL)
        : x1(x1),
          y1(y1),
          x2(x2),
          y2(y2),
          id(x1 * 1000 + y1 * 100 + x2 * 10 + y2),
          startpos(x1 * 10 + y1),
          endpos(x2 * 10 + y2),
          val(val),
          moveType(moveType) {};

    constexpr bool operator==(const Move &move) const
    {
        return this->id == move.id;
    }

    constexpr bool operator!=(const Move &move) const
    {
        return this->id != move.id;
    }

public:
    int id = -1;
    int startpos = -1;
    int endpos = -1;
    int x1 = -1;
    int y1 = -1;
    int x2 = -1;
    int y2 = -1;
    int val = 0;
    MOVE_TYPE moveType = NORMAL;
    bool isCheckingMove = false;
    Piece starter{};
    Piece captured{};
};

class Result
{
public:
    Result(Move move, int score)
        : move(move),
          val(score) {};

public:
    Move move{};
    int val = 0;
};

class TrickResult
{
public:
    TrickResult(bool isSuccess, std::vector<int> data)
        : isSuccess(isSuccess),
          data(std::move(data)) {};

public:
    bool isSuccess = false;
    std::vector<int> data;
};

class TransItem
{
public:
    TransItem() = default;

public:
    int32 hashLock = 0;
    int32 vlExact = -INF;
    int32 vlBeta = -INF;
    int32 vlAlpha = -INF;
    int32 exactDepth = 0;
    int32 betaDepth = 0;
    int32 alphaDepth = 0;
    Move exactMove{};
    Move betaMove{};
    Move alphaMove{};
};

void wait(int ms)
{
#ifdef _WIN32
    Sleep(ms);
#elif __unix__
    sleep(ms / 1000);
#endif
}

void command(std::string str)
{
    int res = system(str.c_str());
}

void readFile(std::string filename, std::string &content)
{
    std::ifstream file(filename, std::ios::in | std::ios::binary);
    if (!file)
    {
        content = "";
    }
    std::string result((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    content = result;
}

void writeFile(std::string filename, std::string content)
{
    std::ofstream file(filename, std::ios::out | std::ios::binary);
    if (!file)
    {
        std::cerr << "Failed to open file for writing: " << filename << std::endl;
        return;
    }
    file.write(content.c_str(), content.size());
}
