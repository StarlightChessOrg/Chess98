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

struct Move;
struct Timer;
struct TTEntry;

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
using MovePickerStatus = std::uint8_t;
using PREGEN_DATA = std::uint8_t;
using HASH_FLAG = std::int8_t;
using SEARCH_RET = std::pair<Move, VL>;
using TRICK_RET = std::pair<bool, VL>;
using MATRIX = std::array<PTYPE, 90>;
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
constexpr VL FP_MARGIN = 120;
constexpr HASH SIDE_KEY = 7655453740479314502;
constexpr MovePickerStatus STATUS_TT = 0;
constexpr MovePickerStatus STATUS_GOOD_CAPTURES = 1;
constexpr MovePickerStatus STATUS_KILLER = 2;
constexpr MovePickerStatus STATUS_QUIET = 3;
constexpr MovePickerStatus STATUS_BAD_CAPTURES = 4;
constexpr DEPTH Q_MAX_DISTANCE = 8;
constexpr DEPTH Q_CHECKING_DEPTH = 4;
constexpr VL Q_DELTA_MARGIN = 80;
constexpr DEPTH NULL_MOVE_R = 2;
constexpr DEPTH NULL_MOVE_MIN_DEPTH = 3;
constexpr DEPTH LMR_MIN_DEPTH = 3;
constexpr int LMR_MIN_MOVES = 3;
constexpr DEPTH LMR_BASE = 1;
constexpr bool NODE_PV = false;
constexpr bool NODE_CUT = true;
constexpr std::array<VL, 8> WEIGHTS { 0, 30, 2, 2, 4, 10, 5, 1 };

constexpr void set_left_4bit_(PREGEN_DATA& d, UINT32 n) { d |= n << 4; }
constexpr void set_right_4bit_(PREGEN_DATA& d, UINT32 n) { d |= n; }
constexpr int get_left_4bit(PREGEN_DATA d) { return d >> 4; }
constexpr int get_right_4bit(PREGEN_DATA d) { return d & 0xF; }
constexpr int get_bit_on_(UINT32 d, UINT32 i) { return (d >> i) & 1; }

// timer
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
        return int(ms);
    }
};

// move
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

// tt entry
struct TTEntry {
    HASH key { 0 };
    HASH_FLAG flag { 0 };
    VL vl { 0 };
    DEPTH depth { 0 };
    Move move { };
};

// hash keys for zobrist hashing
// accessing: HASH_KEYS[size_t(piece + 7)][pos]
const std::array<std::array<HASH, 90>, 15> HASH_KEYS = []() {
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

// pregen points for rook and cannon non-capture moves
const PREGEN_TABLE ROOK_PREGEN_ = []() {
    PREGEN_TABLE ret { };
    for (UINT32 pos = 0; pos < 10; pos++) {
        for (UINT32 bitline = 0; bitline < 1024; bitline++) {
            PREGEN_DATA& entry = ret[pos][bitline];
            for (UINT8 i = pos + 1;; i++) {
                if (i >= 9 || get_bit_on_(bitline, i)) {
                    set_right_4bit_(entry, (i <= 9 ? i : 9));
                    break;
                }
            }
            for (UINT8 i = pos - 1;; i--) {
                if (i == 0 || i > 90 || get_bit_on_(bitline, i)) {
                    set_left_4bit_(entry, (i != 0xFF ? i : 0));
                    break;
                }
            }
        }
    }
    return ret;
}();

// pregen points for cannon capture moves
const PREGEN_TABLE CANNON_PREGEN_ = []() {
    PREGEN_TABLE ret { };
    for (UINT32 pos = 0; pos < 10; pos++) {
        for (UINT32 bitline = 0; bitline < 1024; bitline++) {
            PREGEN_DATA& entry = ret[pos][bitline];
            for (UINT8 i = pos + 1, t = 0;; i++) {
                if (i > 9) {
                    set_right_4bit_(entry, 0b1111);
                    break;
                }
                if (get_bit_on_(bitline, i)) {
                    if (t == 0) {
                        t = 1;
                    } else if (i < 10) {
                        set_right_4bit_(entry, i);
                        break;
                    }
                }
            }
            for (UINT8 i = pos - 1, t = 0;; i--) {
                if (i == 0xFF) {
                    set_left_4bit_(entry, 0b1111);
                    break;
                }
                if (get_bit_on_(bitline, i)) {
                    if (t == 0) {
                        t = 1;
                    } else if (i >= 0) {
                        set_left_4bit_(entry, i);
                        break;
                    }
                }
            }
        }
    }
    return ret;
}();

// convert fen string to matrix
MATRIX fen_to_matrix(const std::string& fen)
{
    MATRIX m { };
    int i = 0;
    const char* t = "KABNRCPkabnrcp";
    for (const char c : fen) {
        if (c == ' ') break;
        if (c == '/') continue;
        if (c >= '1' && c <= '9') {
            i += c - '0';
            continue;
        }
        const char* q = t;
        while (*q && *q != c) q++;
        if (!*q || i >= 90) continue;
        const int k = int(q - t);
        m[i++] = k < 7 ? PTYPE(k + 1) : PTYPE(6 - k);
    }
    return m;
}

// convert matrix to fen string
std::string matrix_to_fen(const MATRIX& m)
{
    std::string s;
    for (int r = 0; r < 10; r++) {
        if (r) s += '/';
        int e = 0;
        for (int c = 0; c < 9; c++) {
            const PTYPE p = m[size_t(r * 9 + c)];
            if (!p) {
                e++;
                continue;
            }
            if (e) s += char('0' + e), e = 0;
            s += p > 0 ? "KABNRCP"[p - 1] : "kabnrcp"[-p - 1];
        }
        if (e) s += char('0' + e);
    }
    return s;
}

// convert move to uci move string like "a0a1"
std::string move_to_ucimove(Move move)
{
    std::string s;
    s += char('a' + move.beg % 9);
    s += char('0' + 9 - move.beg / 9);
    s += char('a' + move.end % 9);
    s += char('0' + 9 - move.end / 9);
    return s;
}

// convert uci move string like "a0a1" to move
Move ucimove_to_move(const std::string& s)
{
    if (s.size() != 4) return { };
    const int file0 = s[0] - 'a';
    const int rank0 = s[1] - '0';
    const int file1 = s[2] - 'a';
    const int rank1 = s[3] - '0';
    if (file0 < 0 || file0 > 8 || file1 < 0 || file1 > 8) return { };
    if (rank0 < 0 || rank0 > 9 || rank1 < 0 || rank1 > 9) return { };
    const int beg = (9 - rank0) * 9 + file0;
    const int end = (9 - rank1) * 9 + file1;
    if (beg == end) return { };
    return Move { beg, end };
}

// get the banner points for rook and cannon moves
// return {left, right} for horizontal moves and {up, down} for vertical moves
template <bool IS_9, bool IS_ROOK>
std::pair<POS, POS> get_bl_banner_points(UINT16 bl, POS p)
{
    assert(p < 90);
    PREGEN_DATA v { };
    if constexpr (IS_ROOK) {
        v = IS_9 ? ROOK_PREGEN_[p % 9][bl] : ROOK_PREGEN_[p / 9][bl];
    } else {
        v = IS_9 ? CANNON_PREGEN_[p % 9][bl] : CANNON_PREGEN_[p / 9][bl];
    }
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

// get the banner points for the rook and cannon non-capture moves
// in the horizontal direction, return {left, right}
std::pair<POS, POS> rook_9(UINT16 bl9, POS p)
{
    return get_bl_banner_points<true, true>(bl9, p);
}

// get the banner points for the rook and cannon non-capture moves
// in the vertical direction, return {up, down}
std::pair<POS, POS> rook_10(UINT16 bl10, POS p)
{
    return get_bl_banner_points<false, true>(bl10, p);
}

// get the banner points for the cannon capture moves
// in the horizontal direction, return {left, right}
std::pair<POS, POS> cannon_9(UINT16 bl9, POS p)
{
    return get_bl_banner_points<true, false>(bl9, p);
}

// get the banner points for the cannon capture moves
// in the vertical direction, return {up, down}
std::pair<POS, POS> cannon_10(UINT16 bl10, POS p)
{
    return get_bl_banner_points<false, false>(bl10, p);
}
