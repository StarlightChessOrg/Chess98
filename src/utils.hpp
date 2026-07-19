#pragma once
#include "base.hpp"

MATRIX fen_to_matrix(const std::string& fen)
{
    MATRIX m { };
    int i = 0;
    for (const char c : fen) {
        if (c == ' ') break;
        if (c == '/') continue;
        if (c >= '1' && c <= '9') {
            i += c - '0';
            continue;
        }
        constexpr const char* t = "KABNRCPkabnrcp";
        const char* q = t;
        while (*q && *q != c) q++;
        if (!*q || i >= 90) continue;
        const int k = int(q - t);
        m[i++] = k < 7 ? static_cast<PTYPE>(k + 1) : static_cast<PTYPE>(6 - k);
    }
    return m;
}

std::string matrix_to_fen(const MATRIX& m)
{
    std::string s;
    for (int r = 0; r < 10; r++) {
        if (r) s += '/';
        int e = 0;
        for (int c = 0; c < 9; c++) {
            const PTYPE p = m[r * 9 + c];
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

std::string move_to_ucimove(Move move)
{
    std::string s;
    s += char('a' + move.beg % 9);
    s += char('0' + 9 - move.beg / 9);
    s += char('a' + move.end % 9);
    s += char('0' + 9 - move.end / 9);
    return s;
}
