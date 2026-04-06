#pragma once
#include "base.hpp"

// some bit functions
constexpr void set_left_4bit_(PREGEN_DATA& data, UINT32 number)
{
    data |= number << 4;
}

constexpr void set_right_4bit_(PREGEN_DATA& data, UINT32 number)
{
    data |= number;
}

constexpr int get_left_4bit(PREGEN_DATA data)
{
    return data >> 4;
}

constexpr int get_right_4bit(PREGEN_DATA data)
{
    return data & 0xF;
}

constexpr int get_bit_on_(UINT32 data, UINT32 index_from_right)
{
    return (data >> index_from_right) & 1;
}

// rook captures or cannon scaffolds pregen points
constexpr PREGEN_TABLE LINEAR_PREGEN = []() {
    PREGEN_TABLE ret {};
    for (UINT32 pos = 0; pos < 10; pos++) {
        for (UINT32 bitline = 0; bitline < 1024; bitline++) {
            PREGEN_DATA& entry = ret[pos][bitline];
            for (UINT32 i = pos + 1; i < 10; i++) {
                if (get_bit_on_(bitline, i) || i == 9) {
                    set_right_4bit_(entry, i);
                    break;
                }
            }
            for (UINT8 i = pos - 1; i != 0xFF; i--) {
                if (get_bit_on_(bitline, i) || i == 9) {
                    set_left_4bit_(entry, i);
                    break;
                }
            }
        }
    }
    return ret;
}();

// cannon captures pregen points
constexpr PREGEN_TABLE CANNON_PREGEN = []() {
    PREGEN_TABLE ret {};
    for (UINT32 pos = 0; pos < 10; pos++) {
        for (UINT32 bitline = 0; bitline < 1024; bitline++) {
            PREGEN_DATA& entry = ret[pos][bitline];
            bool t = false;
            for (UINT8 i = pos + 1; i < 10; i++) {
                if (get_bit_on_(bitline, i)) {
                    if (t == false) {
                        t = true;
                    } else {
                        set_right_4bit_(entry, i);
                        break;
                    }
                } else if (i == 9) {
                    set_right_4bit_(entry, 0b1111);
                }
            }
            t = false;
            for (UINT8 i = pos; i != 0xFF; i--) {
                if (get_bit_on_(bitline, i)) {
                    if (t == false) {
                        t = true;
                    } else {
                        set_left_4bit_(entry, i);
                        break;
                    }
                } else if (i == 0) {
                    set_right_4bit_(entry, 0b1111);
                }
            }
        }
    }
    return ret;
}();

// interface (invalid pos which is greater than 100 contained in cannon)
template <bool IS_9, PREGEN_TABLE TABLE>
std::pair<POS, POS> get_banner(UINT16 bl, POS p)
{
    const PREGEN_DATA v = IS_9 ? TABLE[p / 9][bl] : TABLE[p % 9][bl];
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
    return get_banner<true, LINEAR_PREGEN>(bl9, p);
}

std::pair<POS, POS> rook_10(UINT16 bl10, POS p)
{
    return get_banner<false, LINEAR_PREGEN>(bl10, p);
}

std::pair<POS, POS> cannon_9(UINT16 bl9, POS p)
{
    return get_banner<true, CANNON_PREGEN>(bl9, p);
}

std::pair<POS, POS> cannon_10(UINT16 bl10, POS p)
{
    return get_banner<false, CANNON_PREGEN>(bl10, p);
}
