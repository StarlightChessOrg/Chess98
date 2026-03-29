#pragma once
#include <algorithm>
#include <array>
#include <bitset>
#include <cassert>
#include <chrono>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <random>
#include <string>
#include <utility>
#include <vector>

// defines

struct Move;
struct Timer;
struct TTEntry;
using POS = unsigned char;
using PTYPE = char;
using PID = unsigned char;
using TEAM = char;
using DEPTH = unsigned char;
using VL = short;
using HASH = long long;
using HASH_FLAG = char;
using SEARCH_RET = std::pair<Move, VL>;
using TRICK_RET = std::pair<bool, VL>;
using MATRIX = std::array<PTYPE, 90>;
constexpr POS INVALID_POS = 100;
constexpr HASH_FLAG EXACT = 0;
constexpr HASH_FLAG ALPHA = 1;
constexpr HASH_FLAG BETA = 2;
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

// move

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
    bool operator==(Move m) const noexcept
    {
        return beg == m.beg && end == m.end;
    }
    bool operator!=(Move m) const noexcept
    {
        return beg != m.beg || end != m.end;
    }
    operator bool() const noexcept
    {
        return beg != end;
    }
    operator int() const noexcept
    {
        return beg * 100 + end;
    }
};

// tt entry

struct TTEntry {
    HASH key { 0 };
    HASH_FLAG flag { 0 };
    VL vl { 0 };
    DEPTH depth { 0 };
    Move move {};

    TTEntry() = default;
};

// timer

struct Timer {
    std::chrono::steady_clock::time_point beg {};
    std::chrono::milliseconds limit {};

    Timer()
        : beg(std::chrono::steady_clock::now())
    {
    }
    Timer(int _limit)
        : beg(std::chrono::steady_clock::now())
        , limit(std::chrono::milliseconds(_limit))
    {
        assert(_limit > 0);
    }
    bool time_up() const noexcept
    {
        return std::chrono::steady_clock::now() - beg >= limit;
    }
    int duration() const noexcept
    {
        using namespace std::chrono;
        const auto duration = steady_clock::now() - beg;
        const auto ms = duration_cast<milliseconds>(duration).count();
        return static_cast<int>(ms);
    }
};

// hash

long long gen_random_()
{
    static std::mt19937_64 rand_engine(2820795095);
    static std::uniform_int_distribution<long long> gen_rand {
        std::numeric_limits<long long>::min(),
        std::numeric_limits<long long>::max()
    };
    return gen_rand(rand_engine);
}

const std::array<std::array<HASH, 90>, 15> HASH_KEYS_ = []() {
    std::array<std::array<HASH, 90>, 15> ret {};
    for (std::array<HASH, 90>& m : ret) {
        for (HASH& h : m) {
            h = gen_random_();
        }
    }
    ret[7].fill(0);
    return ret;
}();

const HASH SIDE_KEY = gen_random_();

HASH hashkey_on(PTYPE ptype, POS pos)
{
    assert(-7 <= ptype && ptype <= 7 && pos < 90);
    ptype += 7;
    return HASH_KEYS_[ptype][pos];
}
