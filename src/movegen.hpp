#pragma once
#include "position.hpp"

// king moves
std::vector<Move> gen_king_(POS pos)
{
    std::vector<Move> ret {};
    ret.reserve(4);
    const bool condition1 = (2 < pos && pos < 15) || (65 < pos && pos < 78);
    if (condition1 && not_same_team(pos + 9)) ret.emplace_back(pos, pos + 9);
    const bool condition2 = (11 < pos && pos < 24) || (74 < pos && pos < 87);
    if (condition2 && not_same_team(pos - 9)) ret.emplace_back(pos, pos - 9);
    if (pos % 9 == 4 && not_same_team(pos - 1)) ret.emplace_back(pos, pos - 1);
    if (pos % 9 == 4 && not_same_team(pos + 1)) ret.emplace_back(pos, pos + 1);
    return ret;
}

// advisor moves
std::vector<Move> gen_advisor_(POS pos)
{
    if (pos == 13 || pos == 76) { // black center
        std::vector<Move> ret {};
        ret.reserve(4);
        if (not_same_team(pos - 10)) ret.emplace_back(pos, pos - 10);
        if (not_same_team(pos - 8)) ret.emplace_back(pos, pos - 8);
        if (not_same_team(pos + 10)) ret.emplace_back(pos, pos + 10);
        if (not_same_team(pos + 8)) ret.emplace_back(pos, pos + 8);
        return ret;
    } else if (pos < 24 && not_same_team(13)) { // black corner
        return { Move(pos, 13) };
    } else if (not_same_team(76)) { // red corner
        return { Move(pos, 76) };
    }
    return {};
}

// bishop moves
std::vector<Move> gen_bishop_(POS pos)
{
    std::vector<Move> ret {};
    ret.reserve(4);
    if (pos / 9 == 0 || pos / 9 == 5 || pos / 9 == 7 || pos / 9 == 3) {
        if (!piece_on(pos + 10) && not_same_team(pos + 16)) {
            ret.emplace_back(pos, pos + 16);
        }
        if (!piece_on(pos + 12) && not_same_team(pos + 20)) {
            ret.emplace_back(pos, pos + 20);
        }
    }
    if (pos / 9 == 4 || pos / 9 == 9 || pos / 9 == 7 || pos / 9 == 3) {
        if (!piece_on(pos - 10) && not_same_team(pos - 16)) {
            ret.emplace_back(pos, pos - 16);
        }
        if (!piece_on(pos - 12) && not_same_team(pos - 20)) {
            ret.emplace_back(pos, pos - 20);
        }
    }
    return ret;
}

// knight moves
template <bool GEN_CAPTURE>
std::vector<Move> gen_knight_(POS pos)
{
    std::vector<Move> ret {};
    ret.reserve(8);
    if (pos > 17 && !piece_on(pos - 9)) {
        if (pos % 9 != 0 && team_diff<GEN_CAPTURE>(pos - 19)) {
            ret.emplace_back(pos, pos - 19);
        }
        if (pos % 9 != 8 && team_diff<GEN_CAPTURE>(pos - 17)) {
            ret.emplace_back(pos, pos - 17);
        }
    }
    if (pos < 72 && !piece_on(pos + 9)) {
        if (pos % 9 != 0 && team_diff<GEN_CAPTURE>(pos + 19)) {
            ret.emplace_back(pos, pos + 19);
        }
        if (pos % 9 != 8 && team_diff<GEN_CAPTURE>(pos + 17)) {
            ret.emplace_back(pos, pos + 17);
        }
    }
    if (pos % 9 > 1 && !piece_on(pos - 1)) {
        if (pos / 9 != 0 && team_diff<GEN_CAPTURE>(pos - 11)) {
            ret.emplace_back(pos, pos - 11);
        }
        if (pos / 9 != 9 && team_diff<GEN_CAPTURE>(pos + 7)) {
            ret.emplace_back(pos, pos + 7);
        }
    }
    if (pos % 9 < 7 && !piece_on(pos + 1)) {
        if (pos / 9 != 0 && team_diff<GEN_CAPTURE>(pos + 11)) {
            ret.emplace_back(pos, pos + 11);
        }
        if (pos / 9 != 9 && team_diff<GEN_CAPTURE>(pos - 7)) {
            ret.emplace_back(pos, pos - 7);
        }
    }
    return ret;
}

// rook moves (legacy)
template <bool GEN_CAPTURE>
std::vector<Move> gen_rook_(POS pos)
{
    std::vector<Move> ret {};
    ret.reserve(GEN_CAPTURE ? 4 : 17);
    for (int p = pos - 9; p >= 0; p -= 9) {
        if (team_diff<GEN_CAPTURE>(p)) {
            ret.emplace_back(pos, p);
        }
        if (piece_on(p)) {
            break;
        }
    }
    for (int p = pos + 9; p < 90; p += 9) {
        if (team_diff<GEN_CAPTURE>(p)) {
            ret.emplace_back(pos, p);
        }
        if (piece_on(p)) {
            break;
        }
    }
    for (int p = pos + 1; p / 9 == pos / 9; p += 1) {
        if (team_diff<GEN_CAPTURE>(p)) {
            ret.emplace_back(pos, p);
        }
        if (piece_on(p)) {
            break;
        }
    }
    for (int p = pos - 1; p / 9 == pos / 9; p -= 1) {
        if (team_diff<GEN_CAPTURE>(p)) {
            ret.emplace_back(pos, p);
        }
        if (piece_on(p)) {
            break;
        }
    }
    return ret;
}

// cannon moves (legacy)
template <bool GEN_CAPTURE>
std::vector<Move> gen_cannon_(POS pos)
{
    std::vector<Move> ret {};
    ret.reserve(GEN_CAPTURE ? 4 : 17);
    for (int p = pos - 9; p >= 0; p -= 9) {
        if (!piece_on(p)) {
            if constexpr (!GEN_CAPTURE) {
                ret.emplace_back(pos, p);
            }
        } else {
            for (p -= 9; p >= 0 && !piece_on(p); p -= 9) { }
            if (p >= 0 && opposite(p)) {
                ret.emplace_back(pos, p);
            }
            break;
        }
    }
    for (int p = pos + 9; p < 90; p += 9) {
        if (!piece_on(p)) {
            if constexpr (!GEN_CAPTURE) {
                ret.emplace_back(pos, p);
            }
        } else {
            for (p += 9; p < 90 && !piece_on(p); p += 9) { }
            if (p < 90 && opposite(p)) {
                ret.emplace_back(pos, p);
            }
            break;
        }
    }
    for (int p = pos + 1; p / 9 == pos / 9; p += 1) {
        if (!piece_on(p)) {
            if constexpr (!GEN_CAPTURE) {
                ret.emplace_back(pos, p);
            }
        } else {
            for (p += 1; p / 9 == pos / 9 && !piece_on(p); p += 1) { }
            if (p / 9 == pos / 9 && opposite(p)) {
                ret.emplace_back(pos, p);
            }
            break;
        }
    }
    for (int p = pos - 1; p / 9 == pos / 9; p -= 1) {
        if (!piece_on(p)) {
            if constexpr (!GEN_CAPTURE) {
                ret.emplace_back(pos, p);
            }
        } else {
            for (p -= 1; p / 9 == pos / 9 && !piece_on(p); p -= 1) { }
            if (p / 9 == pos / 9 && opposite(p)) {
                ret.emplace_back(pos, p);
            }
            break;
        }
    }
    return ret;
}

// pawn moves
template <bool GEN_CAPTURE>
std::vector<Move> gen_pawn_(POS pos)
{
    std::vector<Move> ret {};
    ret.reserve(3);
    const int target = pos - 9 * g_team;
    if (0 <= target && target < 90 && team_diff<GEN_CAPTURE>(target)) {
        ret.emplace_back(pos, target);
    }
    if ((pos / 9 < 5 && g_team == R) || (pos / 9 > 4 && g_team == B)) {
        if (pos % 9 != 0 && team_diff<GEN_CAPTURE>(pos - 1)) {
            ret.emplace_back(pos, pos - 1);
        }
        if (pos % 9 != 8 && team_diff<GEN_CAPTURE>(pos + 1)) {
            ret.emplace_back(pos, pos + 1);
        }
    }
    return ret;
}

// rook moves (bit-based)
template <bool GEN_CAPTURE>
std::vector<Move> gen_rook_bit_(POS pos)
{
    std::vector<Move> ret {};
    ret.reserve(GEN_CAPTURE ? 4 : 17);
    const POS x = pos / 9;
    const POS y = pos % 9;
    const unsigned short bl8 = get_bl8(pos);
    const unsigned short bl9 = get_bl9(pos);
    const unsigned short t8 = ROOK_TARGETS[y][bl9];
    const unsigned short t9 = ROOK_TARGETS[x][bl8];
    unsigned char target8_right = (t8 >> 8) & 0xFF;
    target8_right = target8_right == 9 ? 8 : target8_right;
    const unsigned char target8_left = t8 & 0xFF;
    const unsigned char target9_right = (t9 >> 8) & 0xFF;
    const unsigned char target9_left = t9 & 0xFF;
    const unsigned char p1 = x * 9 + target8_left;
    const unsigned char p2 = x * 9 + target8_right;
    const unsigned char p3 = target9_left * 9 + y;
    const unsigned char p4 = target9_right * 9 + y;
    if constexpr (GEN_CAPTURE) {
        if (opposite(p1))
            ret.emplace_back(pos, p1);
        if (opposite(p2))
            ret.emplace_back(pos, p2);
        if (opposite(p3))
            ret.emplace_back(pos, p3);
        if (opposite(p4))
            ret.emplace_back(pos, p4);
    } else {
        const POS start1 = piece_on(p1) ? p1 + 1 : p1;
        const POS end1 = piece_on(p2) ? p2 - 1 : p2;
        for (POS p = start1; p < pos; p++) {
            ret.emplace_back(pos, p);
        }
        for (POS p = pos + 1; p <= end1; p++) {
            ret.emplace_back(pos, p);
        }
        const POS start2 = piece_on(p3) ? p3 + 9 : p3;
        const POS end2 = piece_on(p4) ? p4 - 9 : p4;
        for (POS p = start2; p <= end2; p += 9) {
            ret.emplace_back(pos, p);
        }
    }
    return ret;
}

// cannon moves (bit-based)
template <bool GEN_CAPTURE>
std::vector<Move> gen_cannon_bit_(POS pos)
{
    std::vector<Move> ret {};
    ret.reserve(GEN_CAPTURE ? 4 : 17);
    const POS x = pos / 9;
    const POS y = pos % 9;
    const unsigned short bl8 = get_bl8(x);
    const unsigned short bl9 = get_bl9(y);
    const unsigned short target8 = CANNON_TARGETS[y][bl8];
    const unsigned short target9 = CANNON_TARGETS[x][bl9];
    if constexpr (GEN_CAPTURE) {

    } else {
    }
}
