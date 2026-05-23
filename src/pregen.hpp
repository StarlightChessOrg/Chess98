#pragma once
#include "base.hpp"

using PREGEN_DATA = UINT8;
using PREGEN_TABLE = std::array<std::array<PREGEN_DATA, 1024>, 10>;

void set_left_4bit_(PREGEN_DATA& d, UINT32 n) { d |= n << 4; }

void set_right_4bit_(PREGEN_DATA& d, UINT32 n) { d |= n; }

int get_left_4bit(PREGEN_DATA d) { return d >> 4; }

int get_right_4bit(PREGEN_DATA d) { return d & 0xF; }

int get_bit_on_(PREGEN_DATA d, UINT32 i) { return (d >> i) & 1; }

// rook captures & cannon scaffolds pregen points
const PREGEN_TABLE ROOK_PREGEN = []() {
    PREGEN_TABLE ret {};
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

// cannon captures pregen points
const PREGEN_TABLE CANNON_PREGEN = []() {
    PREGEN_TABLE ret {};
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

// interface (invalid pos which is greater than 100 contained in cannon)
template <bool IS_9, bool IS_ROOK>
std::pair<POS, POS> get_banner(UINT16 bl, POS p)
{
    const PREGEN_DATA v = [p, bl]() constexpr {
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

std::pair<POS, POS> rook_9(UINT16 bl9, POS p)
{
    return get_banner<true, true>(bl9, p);
}

std::pair<POS, POS> rook_10(UINT16 bl10, POS p)
{
    return get_banner<false, true>(bl10, p);
}

std::pair<POS, POS> cannon_9(UINT16 bl9, POS p)
{
    return get_banner<true, false>(bl9, p);
}

std::pair<POS, POS> cannon_10(UINT16 bl10, POS p)
{
    return get_banner<false, false>(bl10, p);
}
