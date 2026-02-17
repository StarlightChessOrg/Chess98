#pragma once
#include <algorithm>
#include <array>
#include <cassert>
#include <chrono>
#include <string>
#include <utility>
#include <vector>

struct Move;
struct Piece;
struct Timer;
using POS = char;
using PID = char;
using PINDEX = char;
using TEAM = char;
using DEPTH = unsigned char;
using VL = short;
using SEARCH_RET = std::pair<Move, VL>;
using TRICK_RET = std::pair<bool, VL>;
using MATRIX = std::array<PID, 90>;
using MOVES = std::vector<Move>;
using PIECES = std::vector<Piece>;
using PINDECES = std::vector<PINDEX>;
constexpr PID R_KING = 1;
constexpr PID R_ADVISOR = 2;
constexpr PID R_BISHOP = 3;
constexpr PID R_KNIGHT = 4;
constexpr PID R_ROOK = 5;
constexpr PID R_CANNON = 6;
constexpr PID R_PAWN = 7;
constexpr PID B_KING = -1;
constexpr PID B_ADVISOR = -2;
constexpr PID B_BISHOP = -3;
constexpr PID B_KNIGHT = -4;
constexpr PID B_ROOK = -5;
constexpr PID B_CANNON = -6;
constexpr PID B_PAWN = -7;
constexpr TEAM R = 1;
constexpr TEAM B = -1;
constexpr VL INF = 30000;
constexpr VL BAN = 20000;

struct Move {
    POS beg { 0 };
    POS end { 0 };

    Move() = default;
    Move(POS beg, POS end)
        : beg(beg)
        , end(end)
    {
        assert(beg != end);
        assert(0 <= beg && beg <= 89);
        assert(0 <= end && end <= 89);
    }
    bool operator==(Move m) const
    {
        return beg == m.beg && end == m.end;
    }
};

struct Piece {
    PID pid { 0 };
    PINDEX pindex { 0 };
    POS pos { 0 };
    bool live { false };

    Piece() = default;
    Piece(PID p, PINDEX i, POS s)
        : pid(p)
        , pindex(i)
        , pos(s)
        , live(true)
    {
        assert(-7 <= pid && pid <= 7 && pid != 0);
        assert(0 <= pindex && pindex <= 33);
        assert(0 <= pos && pos <= 89);
    }
};

struct Timer {
    std::chrono::steady_clock::time_point beg;
    std::chrono::milliseconds limit;

    Timer() = default;
    Timer(int limit)
        : beg(std::chrono::steady_clock::now())
        , limit(std::chrono::milliseconds(limit))
    {
        assert(limit > 0);
    }
    bool time_up() const
    {
        return std::chrono::steady_clock::now() - beg >= limit;
    }
};
