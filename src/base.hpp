#pragma once
#include <algorithm>
#include <array>
#include <bit>
#include <cassert>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <random>
#include <string>
#include <utility>
#include <vector>

struct Move {
    std::uint8_t beg { 0 };
    std::uint8_t end { 0 };
    Move() = default;
    Move(int beg, int end) : beg(beg), end(end) { }
    bool operator==(Move m) { return beg == m.beg && end == m.end; }
    bool operator!=(Move m) { return beg != m.beg || end != m.end; }
    operator bool() { return beg != end; }
    operator int() { return beg * 100 + end; }
};

struct Timer {
    std::chrono::steady_clock::time_point beg { };
    std::chrono::milliseconds limit { };
    Timer() : beg(std::chrono::steady_clock::now()) { }
    Timer(int _limit)
        : beg(std::chrono::steady_clock::now())
        , limit(std::chrono::milliseconds(_limit)) { }
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

using UINT32 = std::uint32_t;
using UINT16 = std::uint16_t;
using UINT8 = std::uint8_t;
using POS = std::uint8_t;
using PTYPE = std::int8_t;
using PID = std::uint8_t;
using TEAM = std::int8_t;
using DEPTH = std::uint8_t;
using VL = std::int16_t;
using HASH = std::int64_t;
using HASH_FLAG = std::int8_t;
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

std::uint64_t gen_random_()
{
    static std::mt19937_64 rand_engine(2820795095);
    static std::uniform_int_distribution<long long> gen_rand {
        std::numeric_limits<long long>::min(),
        std::numeric_limits<long long>::max()
    };
    return gen_rand(rand_engine);
}

const std::array<std::array<HASH, 90>, 15> HASH_KEYS_ = []() {
    std::array<std::array<HASH, 90>, 15> ret { };
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
