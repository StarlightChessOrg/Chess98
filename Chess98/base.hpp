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
#include <memory>
#include <fstream>
#include <thread>
#ifdef _WIN32
#include <windows.h>
#elif __unix__
#include <unistd.h>
#endif

class Piece;
class Move;
class Result;
class Trick;
class TransItem;
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
using MOVE_TYPE = int;
const MOVE_TYPE NORMAL = 0;
const MOVE_TYPE HISTORY = 1;
const MOVE_TYPE CAPTURE = 2;
const MOVE_TYPE KILLER = 3;
const MOVE_TYPE HASH = 4;
using NODE_TYPE = int;
const NODE_TYPE NONE_TYPE = 0;
const NODE_TYPE ALPHA_TYPE = 1;
const NODE_TYPE BETA_TYPE = 2;
const NODE_TYPE EXACT_TYPE = 3;
using SEARCH_TYPE = int;
const SEARCH_TYPE ROOT = 0;
const SEARCH_TYPE PV = 1;
const SEARCH_TYPE CUT = 2;
const SEARCH_TYPE QUIESC = 3;
const std::vector<PIECEID> ALL_PIECEIDS = {
    R_KING, R_GUARD, R_BISHOP, R_KNIGHT, R_ROOK, R_CANNON, R_PAWN,
    B_KING, B_GUARD, B_BISHOP, B_KNIGHT, B_ROOK, B_CANNON, B_PAWN,
};
void wait(int ms);
void command(std::string str);
void readFile(std::string filename, std::string& content);
void writeFile(std::string filename, std::string content);
PIECEID_MAP fenToPieceidmap(std::string fenCode);
std::string pieceidmapToFen(PIECEID_MAP pieceidMap, TEAM team);
TEAM fenToTeam(std::string fenCode);

class Piece
{
public:
    Piece() = default;
    Piece(PIECEID pieceid) : pieceid(pieceid) {}
    Piece(PIECEID pieceid, int x, int y, PIECE_INDEX pieceIndex) : pieceid(pieceid), x(x), y(y), pieceIndex(pieceIndex)
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
        : x1(x1), y1(y1), x2(x2), y2(y2), val(val), moveType(moveType)
    {
        this->id = x1 * 1000 + y1 * 100 + x2 * 10 + y2;
        this->startpos = x1 * 10 + y1;
        this->endpos = x2 * 10 + y2;
    }

    constexpr bool operator==(const Move& move) const { return this->id == move.id; }

    constexpr bool operator!=(const Move& move) const { return this->id != move.id; }

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
    Piece attacker{};
    Piece captured{};
};

class Result
{
public:
    Result() = default;
    Result(Move move, int vl) : move(move), vl(vl) {}

public:
    Move move{};
    int vl = 0;
};

class Trick
{
public:
    Trick() = default;
    Trick(int result) : success(true), data(result) {}

public:
    bool success = false;
    int data = 0;
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

void readFile(std::string filename, std::string& content)
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

PIECEID_MAP fenToPieceidmap(std::string fenCode)
{
    PIECEID_MAP pieceidMap = PIECEID_MAP{};
    int colNum = 9;
    int rowNum = 0;
    std::map<char, PIECEID> pairs{{'R', R_ROOK},  {'N', R_KNIGHT}, {'H', R_KNIGHT}, {'B', R_BISHOP}, {'E', R_BISHOP},
                                  {'G', R_GUARD}, {'A', R_GUARD},  {'K', R_KING},   {'C', R_CANNON}, {'P', R_PAWN},
                                  {'r', B_ROOK},  {'n', B_KNIGHT}, {'h', B_KNIGHT}, {'b', B_BISHOP}, {'e', B_BISHOP},
                                  {'g', B_GUARD}, {'a', B_GUARD},  {'k', B_KING},   {'c', B_CANNON}, {'p', B_PAWN}};
    for (int i = 0; i < fenCode.size(); i++)
    {
        if (fenCode[i] >= '1' && fenCode[i] <= '9')
        {
            rowNum += fenCode[i] - '0';
            continue;
        }
        else if (fenCode[i] == '/')
        {
            rowNum = 0;
            colNum--;
            continue;
        }
        else if (fenCode[i] == ' ')
        {
            break;
        }
        else
        {
            pieceidMap[rowNum][colNum] = pairs.at(fenCode[i]);
        }
        rowNum++;
    }

    return pieceidMap;
}

std::string pieceidmapToFen(PIECEID_MAP pieceidMap, TEAM team)
{
    std::string result = "";
    int spaceCount = 0;
    std::map<PIECEID, char> pairs{{R_KING, 'K'},   {R_GUARD, 'A'}, {R_BISHOP, 'B'}, {R_KNIGHT, 'N'}, {R_ROOK, 'R'},
                                  {R_CANNON, 'C'}, {R_PAWN, 'P'},  {B_KING, 'k'},   {B_GUARD, 'a'},  {B_BISHOP, 'b'},
                                  {B_KNIGHT, 'n'}, {B_ROOK, 'r'},  {B_CANNON, 'c'}, {B_PAWN, 'p'}};
    for (int x = 9; x >= 0; x--)
    {
        for (int y = 0; y < 9; y++)
        {
            PIECEID pieceid = pieceidMap[y][x];
            if (pieceid == EMPTY_PIECEID)
            {
                spaceCount++;
            }
            else
            {
                if (spaceCount > 0)
                {
                    result += std::to_string(spaceCount);
                    spaceCount = 0;
                }
                result += pairs.at(pieceid);
            }
        }
        if (spaceCount > 0)
        {
            result += std::to_string(spaceCount);
            spaceCount = 0;
        }
        result += "/";
    }
    result.pop_back();
    result += team == RED ? " w" : " b";
    result += " - - 0 1";

    return result;
}

TEAM fenToTeam(std::string fen)
{
    return fen.find("w") != std::string::npos ? RED : BLACK;
}
