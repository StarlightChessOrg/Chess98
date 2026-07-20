#pragma once
#if defined(_MSVC_LANG)
#if _MSVC_LANG < 201703L
#error "C++17 or higher is required."
#endif
#else
#if __cplusplus < 201703L
#error "C++17 or higher is required."
#endif
#endif
#include <algorithm>
#include <array>
#include <cassert>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <limits>
#include <random>

struct Move;
struct TTEntry;
struct Timer;
using STATE = std::uint8_t;
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
using FEN = std::string;
using SEARCH_RET = std::pair<Move, VL>;
using TRICK_RET = std::pair<bool, VL>;
using MATRIX = std::array<PTYPE, 90>;
using MovePickerStatus = std::uint8_t;
using PREGEN_DATA = UINT8;
using PREGEN_TABLE = std::array<std::array<PREGEN_DATA, 1024>, 10>;
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
constexpr MovePickerStatus STATUS_TT = 0;
constexpr MovePickerStatus STATUS_GOOD_CAPTURES = 1;
constexpr MovePickerStatus STATUS_KILLER = 2;
constexpr MovePickerStatus STATUS_QUIET = 3;
constexpr MovePickerStatus STATUS_BAD_CAPTURES = 4;
constexpr HASH SIDE_KEY = -7655453740479314502;
constexpr std::array<VL, 8> WEIGHTS { 0, 30, 2, 2, 4, 10, 5, 1 };
constexpr void set_left_4bit_(PREGEN_DATA& d, UINT32 n) { d |= n << 4; }
constexpr void set_right_4bit_(PREGEN_DATA& d, UINT32 n) { d |= n; }
constexpr int get_left_4bit(PREGEN_DATA d) { return d >> 4; }
constexpr int get_right_4bit(PREGEN_DATA d) { return d & 0xF; }
constexpr int get_bit_on_(PREGEN_DATA d, UINT32 i) { return (d >> i) & 1; }
constexpr int find_nth_bit_(UINT32 bitline, int pos, int dir, int n)
{
    for (int i = pos + dir, hits = 0; 0 <= i && i <= 9; i += dir) {
        if (get_bit_on_(bitline, i) && ++hits == n) return i;
    }
    return -1;
}
constexpr PREGEN_DATA pack_(int left, int right)
{
    PREGEN_DATA e = 0;
    set_left_4bit_(e, left);
    set_right_4bit_(e, right);
    return e;
}

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
    bool time_up_3xless() const
    {
        return std::chrono::steady_clock::now() - beg >= limit / 3;
    }
    int duration() const
    {
        using namespace std::chrono;
        const auto duration = steady_clock::now() - beg;
        const auto ms = duration_cast<milliseconds>(duration).count();
        return static_cast<int>(ms);
    }
};

struct Move {
    std::uint8_t beg { 0 };
    std::uint8_t end { 0 };
    Move() = default;
    Move(int beg, int end) : beg(beg), end(end) { }
    bool operator==(Move m) const { return beg == m.beg && end == m.end; }
    bool operator!=(Move m) const { return beg != m.beg || end != m.end; }
    explicit operator bool() const { return beg != end; }
    explicit operator int() const { return beg * 100 + end; }
};

struct TTEntry {
    HASH key { 0 };
    HASH_FLAG flag { 0 };
    VL vl { 0 };
    DEPTH depth { 0 };
    Move move { };
};

inline constexpr PREGEN_TABLE ROOK_PREGEN = []() {
    PREGEN_TABLE ret { };
    for (UINT32 pos = 0; pos < 10; pos++) {
        for (UINT32 bitline = 0; bitline < 1024; bitline++) {
            const int l = find_nth_bit_(bitline, pos, -1, 1);
            const int r = find_nth_bit_(bitline, pos, +1, 1);
            ret[pos][bitline] = pack_(l < 0 ? 0 : l, r < 0 ? 9 : r);
        }
    }
    return ret;
}();

inline constexpr PREGEN_TABLE CANNON_PREGEN = []() {
    PREGEN_TABLE ret { };
    for (UINT32 pos = 0; pos < 10; pos++) {
        for (UINT32 bitline = 0; bitline < 1024; bitline++) {
            const int l = find_nth_bit_(bitline, pos, -1, 2);
            const int r = find_nth_bit_(bitline, pos, +1, 2);
            ret[pos][bitline] = pack_(l < 0 ? 0b1111 : l, r < 0 ? 0b1111 : r);
        }
    }
    return ret;
}();

template <bool IS_9, bool IS_ROOK>
inline std::pair<POS, POS> get_banner(UINT16 bl, POS p)
{
    const PREGEN_DATA v = [p, bl]() {
        if constexpr (IS_ROOK) {
            return IS_9 ? ROOK_PREGEN[p % 9][bl] : ROOK_PREGEN[p / 9][bl];
        } else {
            return IS_9 ? CANNON_PREGEN[p % 9][bl] : CANNON_PREGEN[p / 9][bl];
        }
    }();
    const POS v1 = get_left_4bit(v);
    const POS v2 = get_right_4bit(v);
    const POS l = v1 != 0b1111 ? v1 : INVALID_POS;
    if constexpr (IS_9) {
        const POS r = v2 != 0b1111 ? (v2 != 9 ? v2 : 8) : INVALID_POS;
        return { p / 9 * 9 + l, p / 9 * 9 + r };
    } else {
        const POS r = v2 != 0b1111 ? v2 : INVALID_POS;
        return { l * 9 + p % 9, r * 9 + p % 9 };
    }
}

const std::array<std::array<HASH, 90>, 15> HASH_KEYS_ = []() {
    std::mt19937_64 rand_engine(2820795095);
    std::uniform_int_distribution<long long> gen_rand {
        std::numeric_limits<long long>::min(),
        std::numeric_limits<long long>::max()
    };
    std::array<std::array<HASH, 90>, 15> ret { };
    for (std::array<HASH, 90>& m : ret) {
        for (HASH& h : m) {
            h = gen_rand(rand_engine);
        }
    }
    ret[7].fill(0);
    return ret;
}();

HASH hashkey_on(PTYPE ptype, POS pos);
MATRIX fen_to_matrix(const std::string& fen);
FEN matrix_to_fen(const MATRIX& m);
FEN move_to_ucimove(Move move);
Move ucimove_to_move(const std::string& s);
std::pair<POS, POS> rook_9(UINT16 bl9, POS p);
std::pair<POS, POS> rook_10(UINT16 bl10, POS p);
std::pair<POS, POS> cannon_9(UINT16 bl9, POS p);
std::pair<POS, POS> cannon_10(UINT16 bl10, POS p);
