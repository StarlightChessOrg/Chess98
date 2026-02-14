#pragma once
#include <algorithm>
#include <array>
#include <cassert>
#include <chrono>
#include <cmath>
#include <cstring>
#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <thread>
#include <vector>
#include <numeric>

using TRICK_RET = std::pair<bool, int>;
using SEARCH_RET = std::pair<Move, int>;
using POS = unsigned char;
using INDEX = int;
using PIECEID = int;
using TEAM = int;
using MOVE_TYPE = int;
using NODE_TYPE = int;
using SEARCH_TYPE = int;
using FEN = std::string;
using PID_MATRIX = std::array<int, 256>;
using PIECES = std::vector<Piece>;
using MOVES = std::vector<Move>;
constexpr int INF = 1000000;
constexpr int BAN = INF - 2000;
constexpr int POS_BEG = 0x33;
constexpr int POS_END = 0xcb;
constexpr PIECEID R_KING = 1;
constexpr PIECEID R_ADVISOR = 2;
constexpr PIECEID R_BISHOP = 3;
constexpr PIECEID R_KNIGHT = 4;
constexpr PIECEID R_ROOK = 5;
constexpr PIECEID R_CANNON = 6;
constexpr PIECEID R_PAWN = 7;
constexpr PIECEID B_KING = -1;
constexpr PIECEID B_ADVISOR = -2;
constexpr PIECEID B_BISHOP = -3;
constexpr PIECEID B_KNIGHT = -4;
constexpr PIECEID B_ROOK = -5;
constexpr PIECEID B_CANNON = -6;
constexpr PIECEID B_PAWN = -7;
constexpr TEAM R = 1;
constexpr TEAM B = -1;
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

bool valid_pos(const POS& pos)
{
    assert(POS_BEG <= pos && pos <= POS_END);
    constexpr PID_MATRIX AVAIABLE {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        1, 1, 1, 1, 1, 1, 1, 1, 1,
        0, 0, 0, 0, 0, 0, 0,
        1, 1, 1, 1, 1, 1, 1, 1, 1,
        0, 0, 0, 0, 0, 0, 0,
        1, 1, 1, 1, 1, 1, 1, 1, 1,
        0, 0, 0, 0, 0, 0, 0,
        1, 1, 1, 1, 1, 1, 1, 1, 1,
        0, 0, 0, 0, 0, 0, 0,
        1, 1, 1, 1, 1, 1, 1, 1, 1,
        0, 0, 0, 0, 0, 0, 0,
        1, 1, 1, 1, 1, 1, 1, 1, 1,
        0, 0, 0, 0, 0, 0, 0,
        1, 1, 1, 1, 1, 1, 1, 1, 1,
        0, 0, 0, 0, 0, 0, 0,
        1, 1, 1, 1, 1, 1, 1, 1, 1,
        0, 0, 0, 0, 0, 0, 0,
        1, 1, 1, 1, 1, 1, 1, 1, 1,
        0, 0, 0, 0, 0, 0, 0,
        1, 1, 1, 1, 1, 1, 1, 1, 1,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
    };
    return AVAIABLE[pos] == 1;
}

bool valid_pid(const PIECEID& pid) {
    return -7 <= pid && pid <= 7 && pid != 0;
}

struct Piece {
    Piece() = default;
    Piece(PIECEID pid, POS pos, INDEX index)
        : pieceid(pid)
        , pos(pos)
        , index(index)
        , team(pid > 0 ? R : B)
        , live(true)
    {
        assert(pid != 0);
        assert(POS_BEG <= pos && pos <= POS_END);
        assert(index < 33);
    }
    PIECEID pieceid { 0 };
    POS pos { 0 };
    INDEX index { 0 };
    TEAM team { 0 };
    bool live { false };
};

struct Move {
    Move() = default;
    Move(POS beg, POS end)
        : beg(beg)
        , end(end)
    {
        assert(valid_pos(beg) && valid_pos(end));
    }
    POS beg { 0 };
    POS end { 0 };
};

namespace utils {

std::string to_ucci_move(const Move& m)
{
    return {
        char('a' + (m.beg & 15)), char('0' + (m.beg >> 4)),
        char('a' + (m.end & 15)), char('0' + (m.end >> 4))
    };
}

void wait(int ms)
{
    assert(ms > 0);
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

void command(std::string str)
{
    int res = system(str.c_str());
    assert(res == 0);
}

void readFile(std::string filename, std::string& content)
{
    std::ifstream file(filename, std::ios::in | std::ios::binary);
    assert(file);
    std::string result((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    content = result;
}

void writeFile(std::string filename, std::string content)
{
    std::ofstream file(filename, std::ios::out | std::ios::binary);
    assert(file);
    file.write(content.c_str(), content.size());
}

} // namespace utils

namespace fen {

static constexpr PIECEID letter_pid_table[58] = {
    R_ADVISOR, R_BISHOP, R_CANNON, 0, R_BISHOP, 0, R_ADVISOR, R_KNIGHT,
    0, 0, R_KING, 0, 0, R_KNIGHT, 0, R_PAWN,
    0, R_ROOK, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0,
    B_ADVISOR, B_BISHOP, B_CANNON, 0, B_BISHOP, 0, B_ADVISOR, B_KNIGHT,
    0, 0, B_KING, 0, 0, B_KNIGHT, 0, B_PAWN,
    0, B_ROOK, 0, 0, 0, 0, 0, 0, 0, 0
};
static constexpr char pid_letter_table[15] = {
    'p', 'c', 'r', 'n', 'b', 'a', 'k',
    '?', 'K', 'A', 'B', 'N', 'R', 'C', 'P'
};

PIECEID letter_pid(char& ch)
{
    assert(valid_pid(letter_pid_table[ch - 'A']));
    return letter_pid_table[ch - 'A'];
}

char pid_letter(PIECEID& pid)
{
    assert(valid_pid(pid));
    return pid_letter_table[pid + 7];
}

TEAM team(FEN& fen)
{
    assert(fen.find("w") != std::string::npos || fen.find("b") != std::string::npos);
    return fen.find("w") != std::string::npos ? R : B;
}

using FEN = std::string;
using PID_MATRIX = std::array<int, 256>;

PID_MATRIX to_matrix(FEN fen)
{
    PID_MATRIX pid_matrix {};
    int r = 0, c = 0;
    for (const char& ch : fen) {
        if (ch == ' ') {
            break;
        } else if (ch == '/') {
            r++;
            c = 0;
        } else if (ch >= '1' && ch <= '9') {
            c += ch - '0';
        } else {
            const int idx = ch - 'A';
            const int pid = letter_pid_table[idx];
            if (pid != 0) {
                int sq = ((r + 3) << 4) | (c + 3);
                pid_matrix[sq] = pid;
                c++;
            }
        }
    }
    return pid_matrix;
}

FEN to_fen(PID_MATRIX pid_matrix, TEAM team = R)
{
    std::string fen { "" };
    for (int r = 0; r < 10; r++) {
        int empty = 0;
        for (int c = 0; c < 9; c++) {
            const int sq = ((r + 3) << 4) | (c + 3);
            const int pid = pid_matrix[sq];
            if (!pid) {
                empty++;
            } else {
                if (empty)
                    fen += char('0' + empty), empty = 0;
                fen += pid_letter_table[pid + 7];
            }
        }
        if (empty) {
            fen += char('0' + empty);
        }
        if (r != 9) {
            fen += '/';
        }
    }
    fen += team == R ? " w" : " b";
    return fen;
}

} // namespace fen
