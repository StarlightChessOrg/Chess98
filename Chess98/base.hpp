#pragma once
#include <algorithm>
#include <array>
#include <cassert>
#include <chrono>
#include <cmath>
#include <cstring>
#include <ctime>
#include <fstream>
#include <future>
#include <iostream>
#include <map>
#include <random>
#include <string>
#include <thread>
#include <unordered_map>
#include <vector>

class Piece;
class Move;
class Result;
class Trick;
class TransItem;
class Information;
using uint32 = unsigned int;
using PIECE_INDEX = size_t;
using PIECEID = int;
using TEAM = int;
using FEN = std::string;
using TRICK_RET = std::pair<bool, int>;
using SEARCH_RET = std::pair<Move, int>;
using PIECEID_MAP = std::array<std::array<PIECEID, 10>, 9>;
using PIECE_TARGET_MAP = std::array<std::array<bool, 10>, 9>;
using PIECES = std::vector<Piece>;
using MOVES = std::vector<Move>;
using MOVE_TYPE = int;
using NODE_TYPE = int;
using SEARCH_TYPE = int;
constexpr int INF = 1000000;
constexpr int BAN = INF - 2000;
constexpr int ILLEGAL_VAL = INF * 2;
constexpr int ENGINE_MAX_DEPTH = 64;
constexpr PIECEID EMPTY_PIECEID = 0;
constexpr PIECEID R_KING = 1;
constexpr PIECEID R_GUARD = 2;
constexpr PIECEID R_BISHOP = 3;
constexpr PIECEID R_KNIGHT = 4;
constexpr PIECEID R_ROOK = 5;
constexpr PIECEID R_CANNON = 6;
constexpr PIECEID R_PAWN = 7;
constexpr PIECEID B_KING = -1;
constexpr PIECEID B_GUARD = -2;
constexpr PIECEID B_BISHOP = -3;
constexpr PIECEID B_KNIGHT = -4;
constexpr PIECEID B_ROOK = -5;
constexpr PIECEID B_CANNON = -6;
constexpr PIECEID B_PAWN = -7;
constexpr PIECEID OVERFLOW_PIECEID = 8;
constexpr TEAM EMPTY_TEAM = 0;
constexpr TEAM RED = 1;
constexpr TEAM BLACK = -1;
constexpr TEAM OVERFLOW_TEAM = 2;
constexpr MOVE_TYPE NORMAL = 0;
constexpr MOVE_TYPE HISTORY = 1;
constexpr MOVE_TYPE CAPTURE = 2;
constexpr MOVE_TYPE KILLER = 3;
constexpr MOVE_TYPE HASH = 4;
constexpr NODE_TYPE NONE_TYPE = 0;
constexpr NODE_TYPE ALPHA_TYPE = 1;
constexpr NODE_TYPE BETA_TYPE = 2;
constexpr NODE_TYPE EXACT_TYPE = 3;
constexpr SEARCH_TYPE ROOT = 0;
constexpr SEARCH_TYPE PV = 1;
constexpr SEARCH_TYPE CUT = 2;
constexpr SEARCH_TYPE QUIESC = 3;
constexpr std::array<PIECEID, 14> ALL_PIECEIDS {
    R_KING, R_GUARD, R_BISHOP, R_KNIGHT, R_ROOK, R_CANNON, R_PAWN,
    B_KING, B_GUARD, B_BISHOP, B_KNIGHT, B_ROOK, B_CANNON, B_PAWN
};

class Piece {
public:
    Piece() = default;
    Piece(PIECEID pid, int x, int y, PIECE_INDEX index)
        : pieceid(pid)
        , x(x)
        , y(y)
        , pieceIndex(index)
        , team(pid > 0 ? RED : BLACK)
        , isLive(true)
    {
        assert(pid != 0);
        assert(x < 9 && x > -1 && y < 10 && y > -1);
        assert(index < 33);
    }

public:
    PIECEID pieceid { 0 };
    int x { 0 };
    int y { 0 };
    PIECE_INDEX pieceIndex { 0 };
    TEAM team { EMPTY_TEAM };
    bool isLive { false };
};

class Move {
public:
    Move() = default;
    Move(int x1, int y1, int x2, int y2)
        : x1(x1)
        , y1(y1)
        , x2(x2)
        , y2(y2)
    {
    }

    bool operator==(const Move& move) const
    {
        return x1 == move.x1 && y1 == move.y1 && x2 == move.x2 && y2 == move.y2;
    }

    bool start_eq(const Move& move) const
    {
        return x1 == move.x1 && y1 == move.y1;
    }

public:
    int x1 { 0 };
    int y1 { 0 };
    int x2 { 0 };
    int y2 { 0 };
    MOVE_TYPE moveType { NORMAL };
    bool isCheckingMove { false };
    Piece attacker {};
    Piece captured {};
};

class TransItem {
public:
    TransItem() = default;

public:
    int hash_lock = 0;
    int vlExact = -INF;
    int vlBeta = -INF;
    int vlAlpha = -INF;
    int exactDepth = 0;
    int betaDepth = 0;
    int alphaDepth = 0;
    Move exact_move {};
    Move beta_move {};
    Move alpha_move {};
};

namespace utils {

inline bool is_over_board(int x, int y)
{
    return x > -1 && x < 9 && y > -1 && y < 10;
}

inline void wait(int ms)
{
    assert(ms > 0);
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

void command(std::string str)
{
    int res = system(str.c_str());
    assert(res == 0);
}

inline void readFile(std::string filename, std::string& content)
{
    std::ifstream file(filename, std::ios::in | std::ios::binary);
    assert(file);
    std::string result((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    content = result;
}

inline void writeFile(std::string filename, std::string content)
{
    std::ofstream file(filename, std::ios::out | std::ios::binary);
    assert(file);
    file.write(content.c_str(), content.size());
}

} // namespace utils

namespace fen {

static constexpr PIECEID letter_pid_table[58] = {
    R_GUARD, R_BISHOP, R_CANNON, 0, R_BISHOP, 0, R_GUARD, R_KNIGHT,
    0, 0, R_KING, 0, 0, R_KNIGHT, 0, R_PAWN,
    0, R_ROOK, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    B_GUARD, B_BISHOP, B_CANNON, 0, B_BISHOP, 0, B_GUARD, B_KNIGHT,
    0, 0, B_KING, 0, 0, B_KNIGHT, 0, B_PAWN,
    0, B_ROOK, 0, 0, 0, 0, 0, 0, 0, 0
};
static constexpr char pid_letter_table[15] = {
    'p', 'c', 'r', 'n', 'b', 'a', 'k',
    '?', 'K', 'A', 'B', 'N', 'R', 'C', 'P'
};

inline PIECEID letter_pid(char ch)
{
    assert(letter_pid_table[ch - 'A'] > -8 && letter_pid_table[ch - 'A'] < 8);
    return letter_pid_table[ch - 'A'];
}

inline char pid_letter(PIECEID pid)
{
    assert(pid > -8 && pid < 8);
    return pid_letter_table[pid + 7];
}

inline TEAM team(FEN fen)
{
    assert(fen.find("w") != std::string::npos || fen.find("b") != std::string::npos);
    return fen.find("w") != std::string::npos ? RED : BLACK;
}

PIECEID_MAP to_pid_matrix(FEN fen)
{
    PIECEID_MAP ret {};
    size_t col = 0;
    size_t row = 0;
    for (size_t i = 0; i < fen.size(); i++) {
        if (fen[i] >= '1' && fen[i] <= '9') {
            row += fen[i] - '0';
        } else if (fen[i] == '/') {
            row = 0;
            col += 1;
        } else if (fen[i] == ' ') {
            break;
        } else {
            ret[row][9 - col] = letter_pid(fen[i]);
            row += 1;
        }
    }
    return ret;
}

FEN to_fen(PIECEID_MAP pid_matrix, TEAM team)
{
    std::string result = "";
    for (size_t y = 0; y < 10; y++) {
        int spaceCount = 0;
        for (size_t x = 0; x < 9; x++) {
            PIECEID pieceid = pid_matrix[x][9 - y];
            if (pieceid == EMPTY_PIECEID) {
                spaceCount++;
            } else {
                if (spaceCount > 0) {
                    result += std::to_string(spaceCount);
                    spaceCount = 0;
                }
                result += pid_letter(pieceid);
            }
        }
        if (spaceCount > 0) {
            result += std::to_string(spaceCount);
        }
        result += "/";
    }
    result.pop_back();
    result += team == RED ? " w" : " b";
    return result;
}

} // namespace fen
