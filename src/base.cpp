#include "base.hpp"

HASH hashkey_on(PTYPE ptype, POS pos)
{
    assert(-8 < ptype && ptype < 8 && pos < 90);
    return HASH_KEYS_[static_cast<size_t>(ptype + 7)][pos];
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

MATRIX fen_to_matrix(const std::string& fen)
{
    constexpr const char* pieces = "KABNRCPkabnrcp";
    MATRIX m { };
    int i = 0;
    int file = 0;
    int rank = 0;

    for (size_t k = 0; k < fen.size(); k++) {
        const char c = fen[k];
        if (c == ' ') break;
        if (c == '/') {
            if (file != 9 || rank >= 9) return MATRIX { };
            file = 0;
            rank++;
            continue;
        }
        if (c >= '1' && c <= '9') {
            const int n = c - '0';
            if (file + n > 9) return MATRIX { };
            i += n;
            file += n;
            continue;
        }
        const char* q = pieces;
        while (*q && *q != c) q++;
        if (!*q || file >= 9 || i >= 90) return MATRIX { };
        const int id = static_cast<int>(q - pieces);
        m[static_cast<size_t>(i++)]
            = id < 7 ? static_cast<PTYPE>(id + 1) : static_cast<PTYPE>(6 - id);
        file++;
    }

    if (rank != 9 || file != 9 || i != 90) return MATRIX { };
    return m;
}

FEN matrix_to_fen(const MATRIX& m)
{
    std::string s;
    s.reserve(64);
    for (int r = 0; r < 10; r++) {
        if (r) s += '/';
        int e = 0;
        for (int c = 0; c < 9; c++) {
            const PTYPE p = m[static_cast<size_t>(r * 9 + c)];
            if (p < -7 || p > 7) return { };
            if (!p) {
                e++;
                continue;
            }
            if (e) s += static_cast<char>('0' + e), e = 0;
            const char ch = p > 0 ? "KABNRCP"[p - 1] : "kabnrcp"[-p - 1];
            s += ch;
        }
        if (e) s += static_cast<char>('0' + e);
    }
    return s;
}

FEN move_to_ucimove(Move move)
{
    if (move.beg >= 90 || move.end >= 90 || move.beg == move.end) return { };
    std::string s;
    s += static_cast<char>('a' + move.beg % 9);
    s += static_cast<char>('0' + 9 - move.beg / 9);
    s += static_cast<char>('a' + move.end % 9);
    s += static_cast<char>('0' + 9 - move.end / 9);
    return s;
}

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
