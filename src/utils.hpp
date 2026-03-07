#include "base.hpp"

MATRIX utils_fen_to_matrix(const std::string& fen)
{
    constexpr std::array<char, 128> char_pid_ = []() {
        std::array<char, 128> arr {};
        arr['K'] = R_KING;
        arr['A'] = R_ADVISOR;
        arr['G'] = R_ADVISOR;
        arr['B'] = R_BISHOP;
        arr['E'] = R_BISHOP;
        arr['N'] = R_KNIGHT;
        arr['H'] = R_KNIGHT;
        arr['R'] = R_ROOK;
        arr['C'] = R_CANNON;
        arr['P'] = R_PAWN;
        arr['S'] = R_PAWN;
        arr['k'] = B_KING;
        arr['a'] = B_ADVISOR;
        arr['g'] = B_ADVISOR;
        arr['b'] = B_BISHOP;
        arr['e'] = B_BISHOP;
        arr['n'] = B_KNIGHT;
        arr['h'] = B_KNIGHT;
        arr['r'] = B_ROOK;
        arr['c'] = B_CANNON;
        arr['p'] = B_PAWN;
        arr['s'] = B_PAWN;
        return arr;
    }();
    MATRIX ret {};
    int i = 0;
    for (char c : fen) {
        if (i >= 90) {
            break;
        } else if (c >= '1' && c <= '9') {
            i += (c - '0');
        } else if (c != '/') {
            ret[i++] = char_pid_[static_cast<unsigned char>(c)];
        }
    }
    return ret;
}

std::string utils_matrix_to_fen(const MATRIX& m)
{
    constexpr std::array<char, 15> pid_char_ = []() {
        std::array<char, 15> arr {};
        arr[R_KING + 7] = 'K';
        arr[R_ADVISOR + 7] = 'A';
        arr[R_BISHOP + 7] = 'B';
        arr[R_KNIGHT + 7] = 'N';
        arr[R_ROOK + 7] = 'R';
        arr[R_CANNON + 7] = 'C';
        arr[R_PAWN + 7] = 'P';
        arr[B_KING + 7] = 'k';
        arr[B_ADVISOR + 7] = 'a';
        arr[B_BISHOP + 7] = 'b';
        arr[B_KNIGHT + 7] = 'n';
        arr[B_ROOK + 7] = 'r';
        arr[B_CANNON + 7] = 'c';
        arr[B_PAWN + 7] = 'p';
        return arr;
    }();
    std::string fen {};
    for (int r = 0; r < 10; r++) {
        int empty = 0;
        for (int c = 0; c < 9; c++) {
            const int k = r * 9 + c;
            const PTYPE ptype_i = m[k] + 7;
            if (ptype_i == 7) {
                empty++;
            } else {
                if (empty > 0) {
                    fen += char('0' + empty);
                    empty = 0;
                }
                fen += pid_char_[ptype_i];
            }
        }
        if (empty > 0) {
            fen += char('0' + empty);
        }
        if (r != 9) {
            fen += '/';
        }
    }
    return fen;
}

std::string utils_to_ucci_move(Move move)
{
    std::string s { "" };
    s += 'a' + move.beg / 9;
    s += '0' + move.beg % 9;
    s += 'a' + move.end / 9;
    s += '0' + move.end % 9;
    return s;
}

template <typename T>
void utils_print_matrix(const std::array<T, 90>& board)
{
    std::cout << "\n";
    for (int r = 0; r < 10; ++r) {
        for (int c = 0; c < 9; ++c) {
            int pos = r * 9 + c;
            std::cout << static_cast<int>(board[pos]) << '\t';
        }
        std::cout << '\n';
    }
    std::cout << std::endl;
}

template <typename T>
void utils_print_vector(std::vector<T> vec)
{
    std::cout << "\n[vector] { ";
    for (T v : vec) {
        std::cout << static_cast<int>(v);
        std::cout << ", ";
    }
    std::cout << "}";
    std::cout << std::endl;
}

template <int n>
void utils_print_bitline(std::array<unsigned short, n> bitline)
{
    std::cout << "\n[bitline]\n";
    for (int col = 0; col < n; col++) {
        const short mask = bitline[col];
        std::cout << "col " << col << ": ";
        for (int row = n; row >= 0; --row) {
            std::cout << ((mask >> row) & 1);
        }
        std::cout << "\n";
    }
    std::cout << std::endl;
}

void utils_print_bitline8(std::array<unsigned short, 9> bitline8)
{
    utils_print_bitline<9>(bitline8);
}

void utils_print_bitline9(std::array<unsigned short, 10> bitline9)
{
    utils_print_bitline<10>(bitline9);
}
