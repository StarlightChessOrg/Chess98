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
using POS = unsigned char;
using PTYPE = char;
using PID = unsigned char;
using TEAM = char;
using DEPTH = unsigned char;
using VL = short;
using GAME_TYPE = unsigned char;
using HASH = long long;
using SEARCH_RET = std::pair<Move, VL>;
using TRICK_RET = std::pair<bool, VL>;
using MATRIX = std::array<PTYPE, 90>;
using FLAG = char;
using PREGEN_TABLE = std::array<std::array<unsigned short, 1024>, 10>;
constexpr POS INVALID_POS = 100;
constexpr FLAG EXACT = 0;
constexpr FLAG ALPHA = 1;
constexpr FLAG BETA = 2;
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
constexpr bool CAPTURE = true;
constexpr bool NORMAL = false;

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
    FLAG flag { 0 };
    VL vl { 0 };
    DEPTH depth { 0 };
    Move move {};

    TTEntry() = default;
};

// timer

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

// pregen table

const auto ROOK_TARGETS = []() {
    static PREGEN_TABLE ret {};
    for (unsigned int pos = 0; pos < 10; pos++) {
        for (unsigned int bitline = 0; bitline < 1024; bitline++) {
            if (!((bitline >> pos) & 1)) { // invalid position
                ret[pos][bitline] = ~(0); // set to 111111...111111
                continue;
            }
            for (unsigned int i = pos + 1; i < 10; i++) {
                if ((bitline >> i) & 1) { // set pos data to the 8~16 bit
                    ret[pos][bitline] |= i << 8;
                    break;
                } else if (i == 9) { // end
                    ret[pos][bitline] |= i << 8;
                }
            }
            for (unsigned int i = pos; i-- > 0;) {
                if ((bitline >> i) & 1) { // set pos data to the 1~8 bit
                    ret[pos][bitline] |= i;
                    break;
                }
            }
        }
    }
    return ret;
}();

const auto CANNON_TARGETS = []() {
    static PREGEN_TABLE ret {};
    for (unsigned int pos = 0; pos < 10; pos++) {
        for (unsigned int bitline = 0; bitline < 1024; bitline++) {
            if (!((bitline >> pos) & 1)) {
                ret[pos][bitline] = ~0;
                continue;
            }
            for (unsigned int i = pos + 1; i < 10; i++) {
                if ((bitline >> i) & 1) {
                    ret[pos][bitline] |= i << 12;
                    unsigned int j = i + 1;
                    for (; j < 10 && !((bitline >> j) & 1); j++) { }
                    if (j < 10 && ((bitline >> j) & 1))
                        ret[pos][bitline] |= j << 4;
                    else
                        ret[pos][bitline] |= 0xF << 4;
                    break;
                } else if (i == 9) {
                    ret[pos][bitline] |= i << 12;
                    ret[pos][bitline] |= 0xF << 4;
                }
            }
            if (pos == 9) {
                ret[pos][bitline] |= 9 << 12;
                ret[pos][bitline] |= 0xF << 4;
            }
            for (int i = pos - 1; i >= 0; i--) {
                if ((bitline >> i) & 1) {
                    ret[pos][bitline] |= i << 8;
                    int j = i - 1;
                    for (; j >= 0 && !((bitline >> j) & 1); j--) { }
                    if (j >= 0 && ((bitline >> j) & 1))
                        ret[pos][bitline] |= j;
                    else
                        ret[pos][bitline] |= 0xF;
                    break;
                } else if (i == 0) {
                    ret[pos][bitline] |= i << 8;
                    ret[pos][bitline] |= 0xF;
                }
            }
            if (pos == 0) {
                ret[pos][bitline] |= 0 << 8;
                ret[pos][bitline] |= 0xF;
            }
        }
    }
    return ret;
}();
