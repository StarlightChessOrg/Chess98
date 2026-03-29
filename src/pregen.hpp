#include "base.hpp"

using PREGEN_DATA = unsigned char;
using PREGEN_TABLE = std::array<std::array<PREGEN_DATA, 1024>, 10>;

// some bit functions

constexpr void set_left_4bit_(PREGEN_DATA& data, unsigned int number)
{
    data |= number << 4;
}

constexpr void set_right_4bit_(PREGEN_DATA& data, unsigned int number)
{
    data |= number;
}

constexpr void set_invalid_all(PREGEN_DATA& data)
{
    data = ~0;
}

constexpr int get_bit_on_(PREGEN_DATA data, unsigned int index_from_right)
{
    return (data >> index_from_right) & 1;
}

constexpr int get_left_4bit(PREGEN_DATA data)
{
    return data >> 4;
}

constexpr int get_right_4bit(PREGEN_DATA data)
{
    return data & 0xF;
}

// pregen

// rook captures or cannon scaffolds
constexpr PREGEN_TABLE LINEAR_PREGEN = []() {
    PREGEN_TABLE ret {};
    for (unsigned int pos = 0; pos < 10; pos++) {
        for (unsigned int bitline = 0; bitline < 1024; bitline++) {
            PREGEN_DATA& entry = ret[pos][bitline];
            for (unsigned int i = pos + 1; i < 10; i++) {
                if (get_bit_on_(bitline, i) || i == 9) {
                    set_left_4bit_(entry, i);
                    break;
                }
            }
            for (unsigned int i = pos; i-- > 0;) {
                if (get_bit_on_(bitline, i)) {
                    set_right_4bit_(entry, i);
                    break;
                }
            }
        }
    }
    return ret;
}();

// cannon captures
constexpr PREGEN_TABLE CANNON_PREGEN = []() {
    PREGEN_TABLE ret {};
    for (unsigned int pos = 0; pos < 10; pos++) {
        for (unsigned int bitline = 0; bitline < 1024; bitline++) {
            PREGEN_DATA& entry = ret[pos][bitline];
            for (unsigned int i = pos + 1, bool t = false; i < 10; i++) {
                if (get_bit_on_(bitline, i)) {
                    if (t == false) {
                        t = true;
                        continue;
                    }
                    set_left_4bit_(entry, i);
                    break;
                } else if (i == 9) {
                    set_left_4bit_(entry, i);
                }
            }
            for (unsigned int i = pos, bool t = false; i-- > 0;) {
                if (get_bit_on_(bitline, i)) {
                    if (t == false) {
                        t = true;
                        continue;
                    }
                    set_right_4bit_(entry, i);
                    break;
                }
            }
        }
    }
    return ret;
}();
