#pragma once
#include "heuristic.hpp"
#include "position.hpp"

namespace movegen {

constexpr POS _ = 100;

constexpr auto KING_MOVES = []() {
    std::array<std::array<POS, 4>, 90> ret {};
    // black
    ret[3] = { 4, 12, _, _ };
    ret[4] = { 3, 5, 13, _ };
    ret[5] = { 4, 14, _, _ };
    ret[12] = { 3, 13, 21, _ };
    ret[13] = { 4, 12, 14, 22 };
    ret[14] = { 5, 13, 23, _ };
    ret[21] = { 12, 22, _, _ };
    ret[22] = { 13, 21, 23, _ };
    ret[23] = { 14, 22, _, _ };
    // red
    ret[84] = { 85, 76, _, _ };
    ret[85] = { 84, 86, 77, _ };
    ret[86] = { 85, 78, _, _ };
    ret[75] = { 76, 85, 66, _ };
    ret[76] = { 75, 77, 85, 67 };
    ret[77] = { 76, 68, 86, _ };
    ret[66] = { 75, 67, _, _ };
    ret[67] = { 76, 66, 68, _ };
    ret[68] = { 77, 67, _, _ };
    return ret;
}();

constexpr auto ADVISOR_MOVES = []() {
    std::array<std::array<POS, 4>, 90> ret {};
    // black
    ret[3] = ret[5] = ret[21] = ret[23] = { 13, _, _, _ };
    ret[13] = { 3, 5, 21, 23 };
    // red
    ret[84] = ret[86] = ret[66] = ret[68] = { 76, _, _, _ };
    ret[76] = { 84, 86, 66, 68 };
    return ret;
}();

constexpr auto BISHOP_EYES = []() {
    std::array<std::array<POS, 4>, 90> ret {};
    // black
    ret[2] = { 10, 12, _, _ };
    ret[6] = { 14, 16, _, _ };
    ret[18] = { 10, 28, _, _ };
    ret[22] = { 13, 14, 30, 32 };
    ret[26] = { 16, 34, _, _ };
    ret[38] = { 28, 30, _, _ };
    ret[42] = { 32, 34, _, _ };
    // red
    ret[83] = { 73, 75, _, _ };
    ret[87] = { 77, 79, _, _ };
    ret[63] = { 73, 55, _, _ };
    ret[67] = { 57, 59, 75, 77 };
    ret[71] = { 61, 79, _, _ };
    ret[47] = { 63, 67, _, _ };
    ret[51] = { 67, 71, _, _ };
    return ret;
}();
constexpr auto BISHOP_MOVES = []() {
    std::array<std::array<POS, 4>, 90> ret {};
    // black
    ret[2] = { 18, 22, _, _ };
    ret[6] = { 22, 26, _, _ };
    ret[18] = { 2, 38, _, _ };
    ret[22] = { 2, 6, 38, 42 };
    ret[26] = { 6, 42, _, _ };
    ret[38] = { 18, 22, _, _ };
    ret[42] = { 22, 26, _, _ };
    // red
    ret[83] = { 63, 67, _, _ };
    ret[87] = { 67, 71, _, _ };
    ret[63] = { 83, 47, _, _ };
    ret[67] = { 47, 51, 83, 87 };
    ret[71] = { 51, 87, _, _ };
    ret[47] = { 63, 67, _, _ };
    ret[51] = { 67, 71, _, _ };
    // generate
    return ret;
}();

constexpr auto KNIGHT_LEGS = []() {
    // AI generated
    std::array<std::array<POS, 4>, 90> ret {};
    constexpr int lx[4] = { 0, -1, 0, 1 };
    constexpr int ly[4] = { -1, 0, 1, 0 };
    for (POS pos = 0; pos < 90; ++pos) {
        const int x = pos % 9;
        const int y = pos / 9;
        auto& legs = ret[pos];
        for (int d = 0; d < 4; ++d) {
            const int leg_x = x + lx[d];
            const int leg_y = y + ly[d];
            if (leg_x < 0 || leg_x >= 9 || leg_y < 0 || leg_y >= 10) {
                legs[d] = _;
            } else {
                legs[d] = static_cast<POS>(leg_y * 9 + leg_x);
            }
        }
    }
    return ret;
}();
constexpr auto KNIGHT_MOVES = []() {
    // AI generated
    std::array<std::array<std::array<POS, 2>, 4>, 90> ret {};
    constexpr int mx[4][2] = { { -1, 1 }, { -2, -2 }, { -1, 1 }, { 2, 2 } };
    constexpr int my[4][2] = { { -2, -2 }, { -1, 1 }, { 2, 2 }, { -1, 1 } };
    for (POS pos = 0; pos < 90; ++pos) {
        const int x = pos % 9;
        const int y = pos / 9;
        auto& dirs = ret[pos];
        for (int d = 0; d < 4; ++d) {
            auto& two = dirs[d];
            for (int k = 0; k < 2; ++k) {
                const int nx = x + mx[d][k];
                const int ny = y + my[d][k];
                if (nx < 0 || nx >= 9 || ny < 0 || ny >= 10) {
                    two[k] = _;
                } else {
                    two[k] = static_cast<POS>(ny * 9 + nx);
                }
            }
        }
    }
    return ret;
}();

constexpr auto PAWN_MOVES = []() {
    // AI generated
    std::array<std::array<std::array<POS, 3>, 90>, 2> ret {};
    for (int side = 0; side < 2; ++side) {
        for (POS pos = 0; pos < 90; ++pos) {
            auto& arr = ret[side][pos];
            arr = { _, _, _ };
            const int x = pos % 9;
            const int y = pos / 9;
            int k = 0;
            if (side == 1) {
                if (y < 9) {
                    arr[k++] = static_cast<POS>(pos + 9);
                }
                if (y >= 5) {
                    if (x > 0) {
                        arr[k++] = static_cast<POS>(pos - 1);
                    }
                    if (x < 8) {
                        arr[k++] = static_cast<POS>(pos + 1);
                    }
                }
            } else {
                if (y > 0) {
                    arr[k++] = static_cast<POS>(pos - 9);
                }
                if (y <= 4) {
                    if (x > 0) {
                        arr[k++] = static_cast<POS>(pos - 1);
                    }
                    if (x < 8) {
                        arr[k++] = static_cast<POS>(pos + 1);
                    }
                }
            }
        }
    }
    return ret;
}();

MOVES king(POS pos)
{
    assert(pid_on(pos) == R_KING || pid_on(pos) == B_KING);
    MOVES ret {};
    ret.reserve(4);
    const TEAM team = pid_on(pos) > 0 ? R : B;
    for (const POS i : KING_MOVES[pos]) {
        if (i == _) {
            break;
        } else if (!same_team(team, i)) {
            ret.emplace_back(pos, i);
        }
    }
    return ret;
}

MOVES advisor(POS pos)
{
    assert(pid_on(pos) == R_ADVISOR || pid_on(pos) == B_ADVISOR);
    const TEAM team = pid_on(pos) > 0 ? R : B;
    if (pos == 13 || pos == 76) {
        MOVES ret {};
        ret.reserve(4);
        for (const POS i : ADVISOR_MOVES[pos]) {
            if (!same_team(team, i)) {
                ret.emplace_back(pos, i);
            }
        }
        return ret;
    } else {
        if (!same_team(team, ADVISOR_MOVES[pos][0])) {
            return { Move(pos, ADVISOR_MOVES[pos][0]) };
        }
    }
    return {};
}

MOVES bishop(POS pos)
{
    assert(pid_on(pos) == R_BISHOP || pid_on(pos) == B_BISHOP);
    const TEAM team = pid_on(pos) > 0 ? R : B;
    MOVES ret {};
    ret.reserve(4);
    for (int i = 0; i < 4; ++i) {
        const POS eye = BISHOP_EYES[pos][i];
        if (eye == _) {
            break;
        } else if (pid_on(eye) == 0 && !same_team(team, BISHOP_MOVES[pos][i])) {
            ret.emplace_back(pos, BISHOP_MOVES[pos][i]);
        }
    }
    return ret;
}

MOVES knight(POS pos)
{
    assert(pid_on(pos) == R_KNIGHT || pid_on(pos) == B_KNIGHT);
    const TEAM team = pid_on(pos) > 0 ? R : B;
    MOVES ret {};
    ret.reserve(8);
    for (int i = 0; i < 4; i++) {
        const POS leg = KNIGHT_LEGS[pos][i];
        if (leg != _ && pid_on(leg) == 0) {
            for (const POS to : KNIGHT_MOVES[pos][i]) {
                if (!same_team(team, to)) {
                    ret.emplace_back(pos, to);
                }
            }
        }
    }
    return ret;
}

MOVES pawn(POS pos)
{
    assert(pid_on(pos) == R_PAWN || pid_on(pos) == B_PAWN);
    const TEAM team = pid_on(pos) > 0 ? R : B;
    MOVES ret {};
    ret.reserve(3);
    for (const POS i : PAWN_MOVES[team == R ? 0 : 1][pos]) {
        if (i == _) {
            break;
        } else if (!same_team(team, i)) {
            ret.emplace_back(pos, i);
        }
    }
    return ret;
}

std::pair<MOVES, MOVES> cannon(POS pos)
{
    assert(pid_on(pos) == R_CANNON || pid_on(pos) == B_CANNON);
    const TEAM team = pid_on(pos) > 0 ? R : B;
    MOVES captures {};
    MOVES quiet {};
    quiet.reserve(17);
    captures.reserve(4);
    for (int p = pos - 9; 0 <= p; p -= 9) {
        if (!pid_on(p)) {
            quiet.emplace_back(pos, p);
        } else {
            for (p -= 9; 0 <= p && !pid_on(p); p -= 9) { }
            if (0 <= p && !same_team(team, p)) {
                captures.emplace_back(pos, p);
                break;
            }
        }
    }
    for (int p = pos + 9; p < 90; p += 9) {
        if (!pid_on(p)) {
            quiet.emplace_back(pos, p);
        } else {
            for (p += 9; p < 90 && !pid_on(p); p += 9) { }
            if (p < 90 && !same_team(team, p)) {
                captures.emplace_back(pos, p);
                break;
            }
        }
    }
    for (int p = pos - 1; 0 <= p && p / 9 == pos / 9; p -= 1) {
        if (!pid_on(p)) {
            quiet.emplace_back(pos, p);
        } else {
            for (p -= 1; 0 <= p && p / 9 == pos / 9 && !pid_on(p); p -= 1) { };
            if (0 <= p && p / 9 == pos / 9 && !same_team(team, p)) {
                captures.emplace_back(pos, p);
                break;
            }
        }
    }
    for (int p = pos + 1; p / 9 == pos / 9; p += 1) {
        if (!pid_on(p)) {
            quiet.emplace_back(pos, p);
        } else {
            for (p += 1; p / 9 == pos / 9 && !pid_on(p); p += 1) { };
            if (p / 9 == pos / 9 && !same_team(team, p)) {
                captures.emplace_back(pos, p);
                break;
            }
        }
    }
    return { captures, quiet };
}

std::pair<MOVES, MOVES> rook(POS pos)
{
    assert(pid_on(pos) == R_ROOK || pid_on(pos) == B_ROOK);
    const TEAM team = pid_on(pos) > 0 ? R : B;
    MOVES captures {};
    MOVES quiet {};
    quiet.reserve(17);
    captures.reserve(4);
    for (int p = pos - 9; 0 <= p && !same_team(team, p); p -= 9) {
        (pid_on(p) == 0 ? quiet : captures).emplace_back(pos, p);
    }
    for (int p = pos + 9; p < 90 && !same_team(team, p); p += 9) {
        (pid_on(p) == 0 ? quiet : captures).emplace_back(pos, p);
    }
    for (int p = pos - 1; 0 <= p && p / 9 == pos / 9 && !same_team(team, p); p -= 1) {
        (pid_on(p) == 0 ? quiet : captures).emplace_back(pos, p);
    }
    for (int p = pos + 1; p / 9 == pos / 9 && !same_team(team, p); p += 1) {
        (pid_on(p) == 0 ? quiet : captures).emplace_back(pos, p);
    }
    return { captures, quiet };
}

}

class MovePicker {
    MOVES gen {};
    size_t i { 0 };
    int step { 0 };

public:
    Move next()
    {
        i++;
        if (i >= gen.size()) {
            update();
        }
        return i < gen.size() ? gen[static_cast<size_t>(i - 1)] : Move {};
    }

private:
    void update()
    {
        if (step == 1) {
            for (const PINDEX pindex : pid_pindeces(R_ROOK * team_now())) {
                const auto& moves = movegen::rook(pindex_piece(pindex).pos);
                gen.insert(gen.end(), moves.first.begin(), moves.first.end());
                gen.insert(gen.end(), moves.second.begin(), moves.second.end());
            }
        } else if (step == 2) {
            for (const PINDEX pindex : pid_pindeces(R_CANNON * team_now())) {
                const auto& moves = movegen::cannon(pindex_piece(pindex).pos);
                gen.insert(gen.end(), moves.first.begin(), moves.first.end());
                gen.insert(gen.end(), moves.second.begin(), moves.second.end());
            }
        } else if (step == 3) {
            for (const PINDEX pindex : pid_pindeces(R_KNIGHT * team_now())) {
                const auto& moves = movegen::knight(pindex_piece(pindex).pos);
                gen.insert(gen.end(), moves.begin(), moves.end());
            }
        } else if (step == 4) {
            for (const PINDEX pindex : pid_pindeces(R_PAWN * team_now())) {
                const auto& moves = movegen::pawn(pindex_piece(pindex).pos);
                gen.insert(gen.end(), moves.begin(), moves.end());
            }
        } else if (step == 5) {
            for (const PINDEX pindex : pid_pindeces(R_BISHOP * team_now())) {
                const auto& moves = movegen::bishop(pindex_piece(pindex).pos);
                gen.insert(gen.end(), moves.begin(), moves.end());
            }
        } else if (step == 6) {
            for (const PINDEX pindex : pid_pindeces(R_ADVISOR * team_now())) {
                const auto& moves = movegen::advisor(pindex_piece(pindex).pos);
                gen.insert(gen.end(), moves.begin(), moves.end());
            }
        } else if (step == 7) {
            for (const PINDEX pindex : pid_pindeces(R_KING * team_now())) {
                const auto& moves = movegen::king(pindex_piece(pindex).pos);
                gen.insert(gen.end(), moves.begin(), moves.end());
            }
        }
        step++;
    }
};
