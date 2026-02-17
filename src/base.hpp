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
using POS = std::uint8_t;
using PID = std::int8_t;
using PINDEX = std::uint8_t;
using TEAM = std::int8_t;
using DEPTH = std::uint8_t;
using VL = std::int16_t;
using SEARCH_RET = std::pair<Move, VL>;
using TRICK_RET = std::pair<bool, VL>;
using MATRIX = std::array<PID, 90>;
using MOVES = std::vector<Move>;
using PIECES = std::vector<Piece>;
using PINDECES = std::vector<PINDEX>;
using SEARCH_RETS = std::vector<SEARCH_RET>;
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
    Move(int beg, int end)
        : beg(static_cast<POS>(beg))
        , end(static_cast<POS>(end))
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
    Piece(int p, int i, int s)
        : pid(static_cast<PID>(p))
        , pindex(static_cast<PINDEX>(i))
        , pos(static_cast<POS>(s))
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
    int duration() const
    {
        const std::chrono::duration duration = std::chrono::steady_clock::now() - beg;
        const long long ms = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
        return static_cast<int>(ms);
    }
};
