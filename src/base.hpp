#pragma once
#include <algorithm>
#include <array>
#include <cassert>
#include <chrono>
#include <iostream>
#include <random>
#include <string>
#include <utility>
#include <vector>

struct Move;
struct Piece;
struct Timer;
using POS = unsigned char;
using PTYPE = char;
using PID = char;
using TEAM = char;
using DEPTH = unsigned char;
using VL = short;
using GAME_TYPE = unsigned char;
using HASH = long long;
using SEARCH_RET = std::pair<Move, VL>;
using TRICK_RET = std::pair<bool, VL>;
using MATRIX = std::array<PTYPE, 90>;
using MOVE_LIST = std::vector<Move>;
using PTYPE_LIST = std::vector<PTYPE>;
using PIDPOS_LIST = std::vector<POS>;
using PTYPEPOS_LIST = std::vector<std::vector<POS>>;
HASH gen_random();
HASH hashkey_on(PTYPE ptype, POS pos);
constexpr PTYPE R_KING = 1;
constexpr PTYPE R_ADVISOR = 2;
constexpr PTYPE R_BISHOP = 3;
constexpr PTYPE R_KNIGHT = 4;
constexpr PTYPE R_ROOK = 5;
constexpr PTYPE R_CANNON = 6;
constexpr PTYPE R_PAWN = 7;
constexpr PTYPE B_KING = -1;
constexpr PTYPE B_ADVISOR = -2;
constexpr PTYPE B_BISHOP = -3;
constexpr PTYPE B_KNIGHT = -4;
constexpr PTYPE B_ROOK = -5;
constexpr PTYPE B_CANNON = -6;
constexpr PTYPE B_PAWN = -7;
constexpr TEAM R = 1;
constexpr TEAM B = -1;
constexpr VL INF = 30000;
constexpr VL BAN = 20000;
constexpr VL INVALID_VL = -31000;
const HASH SIDE_KEY = gen_random();

struct Move {
    POS beg { 0 };
    POS end { 0 };

    Move() = default;
    Move(int beg, int end)
        : beg(static_cast<POS>(beg))
        , end(static_cast<POS>(end))
    {
        assert(beg != end && beg < 90 && end < 90);
    }
    bool operator==(Move m) const
    {
        return beg == m.beg && end == m.end;
    }
    bool operator!=(Move m) const
    {
        return !(*this == m);
    }
    operator bool() const
    {
        return beg != end;
    }
};

struct Piece {
    PTYPE ptype { 0 };
    POS pos { 0 };
    PID pid { 0 };

    Piece() = default;
    Piece(int ptype, int pos, int pid)
        : ptype(static_cast<PTYPE>(ptype))
        , pos(static_cast<POS>(pos))
        , pid(static_cast<PID>(pid))
    {
        assert(-7 <= ptype && ptype <= 7 && ptype != 0 && pos < 90);
    }
};

struct Timer {
    std::chrono::steady_clock::time_point beg;
    std::chrono::milliseconds limit;

    Timer() = default;
    Timer(int _limit)
        : beg(std::chrono::steady_clock::now())
        , limit(std::chrono::milliseconds(_limit))
    {
        assert(_limit > 0);
    }
    bool time_up() const
    {
        return std::chrono::steady_clock::now() - beg >= limit;
    }
    int duration() const
    {
        using namespace std::chrono;
        const auto duration = steady_clock::now() - beg;
        const auto ms = duration_cast<milliseconds>(duration).count();
        return static_cast<int>(ms);
    }
};

HASH gen_random()
{
    static std::mt19937_64 rand_engine(2820795095);
    static std::uniform_int_distribution<long long> gen_rand {
        std::numeric_limits<long long>::min(),
        std::numeric_limits<long long>::max()
    };
    return gen_rand(rand_engine);
}

HASH hashkey_on(PTYPE ptype, POS pos)
{
    assert(-7 <= ptype && ptype <= 7 && pos < 90);
    static const std::array<std::array<HASH, 90>, 15> HASH_KEYS = []() {
        std::array<std::array<HASH, 90>, 15> ret {};
        for (std::array<HASH, 90>& m : ret) {
            for (HASH& h : m) {
                h = gen_random();
            }
        }
        ret[7].fill(0);
        return ret;
    }();
    ptype += 7;
    return HASH_KEYS[ptype][pos];
}
