#pragma once
#include "heuristic.hpp"
#include "position.hpp"

namespace {

constexpr POS _ = 100;

//  0   1   2   3   4   5   6   7   8
//  9  10  11  12  13  14  15  16  17
// 18  19  20  21  22  23  24  25  26
// 27  28  29  30  31  32  33  34  35
// 36  37  38  39  40  41  42  43  44
// 45  46  47  48  49  50  51  52  53
// 54  55  56  57  58  59  60  61  62
// 63  64  65  66  67  68  69  70  71
// 72  73  74  75  76  77  78  79  80
// 81  82  83  84  85  86  87  88  89

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

constexpr POS ADVISOR_CENTER_R = 13;
constexpr POS ADVISOR_CENTER_B = 76;
constexpr POS ADVISOR_OFFSET = 10;
constexpr auto ADVISOR_MOVES = []() {
    std::array<std::array<POS, 4>, 90> ret {};
    // black
    ret[3] = ret[5] = ret[21] = ret[23] = { ADVISOR_CENTER_B, _, _, _ };
    ret[ADVISOR_CENTER_B] = { 3, 5, 21, 23 };
    // red
    ret[84] = ret[86] = ret[66] = ret[68] = { ADVISOR_CENTER_R, _, _, _ };
    ret[ADVISOR_CENTER_R] = { 84, 86, 66, 68 };
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
    ret[83] = { 73, 75, _, _ };
    ret[87] = { 77, 79, _, _ };
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

}

MOVES king(POS pos)
{
    assert(pid_on(pos) == R_KING || pid_on(pos) == B_KING);
    MOVES ret {};
    ret.reserve(4);
    const PID pid = pid_on(pos);
    const TEAM team = pid > 0 ? R : B;
    for (const POS i : KING_MOVES[pos]) {
        if (i == _) {
            break;
        } else if (pid_on(i) * team <= 0) {
            ret.emplace_back(pos, i);
        }
    }
    return ret;
}

MOVES advisor(POS pos)
{
    assert(pid_on(pos) == R_ADVISOR || pid_on(pos) == B_ADVISOR);
    const PID pid = pid_on(pos);
    const TEAM team = pid > 0 ? R : B;
    if (pos == ADVISOR_CENTER_R || pos == ADVISOR_CENTER_B) {
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
    const PID pid = pid_on(pos);
    const TEAM team = pid > 0 ? R : B;
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
