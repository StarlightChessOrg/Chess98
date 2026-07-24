#pragma once
#include "position.hpp"

constexpr std::array<VL, 90> OPEN_ATTACK_KING_PAWN_ {
    0, 0, 0, 10037, 10055, 10037, 0, 0, 0,
    0, 0, 0, 10008, 10022, 10008, 0, 0, 0,
    0, 0, 0, 10000, 10002, 10000, 0, 0, 0,
    24, 0, 24, 0, 48, 0, 24, 0, 24,
    24, 0, 42, 0, 52, 0, 42, 0, 24,
    72, 96, 110, 150, 168, 150, 110, 96, 72,
    105, 128, 155, 172, 180, 172, 155, 128, 105,
    105, 138, 188, 220, 228, 220, 188, 138, 105,
    105, 138, 200, 250, 270, 250, 200, 138, 105,
    8, 8, 8, 14, 20, 14, 8, 8, 8
};

constexpr std::array<VL, 90> OPEN_DEFEND_KING_PAWN_ {
    0, 0, 0, 10033, 10048, 10033, 0, 0, 0,
    0, 0, 0, 10006, 10008, 10006, 0, 0, 0,
    0, 0, 0, 10003, 10004, 10003, 0, 0, 0,
    24, 0, 24, 0, 48, 0, 24, 0, 24,
    24, 0, 42, 0, 52, 0, 42, 0, 24,
    90, 115, 128, 168, 185, 168, 128, 115, 90,
    120, 142, 168, 185, 192, 185, 168, 142, 120,
    120, 152, 198, 230, 236, 230, 198, 152, 120,
    120, 152, 212, 260, 280, 260, 212, 152, 120,
    30, 30, 30, 36, 42, 36, 30, 30, 30
};

constexpr std::array<VL, 90> END_ATTACK_KING_PAWN_ {
    0, 0, 0, 10005, 10036, 10005, 0, 0, 0,
    0, 0, 0, 10012, 10042, 10012, 0, 0, 0,
    0, 0, 0, 10018, 10048, 10018, 0, 0, 0,
    130, 0, 115, 130, 135, 130, 115, 0, 130,
    145, 0, 130, 145, 150, 145, 130, 0, 145,
    200, 200, 185, 205, 210, 205, 185, 200, 200,
    220, 235, 235, 240, 245, 240, 235, 235, 220,
    185, 200, 205, 220, 225, 220, 205, 200, 185,
    140, 155, 175, 250, 300, 250, 175, 155, 140,
    12, 12, 12, 28, 32, 28, 12, 12, 12
};

constexpr std::array<VL, 90> END_DEFEND_KING_PAWN_ {
    0, 0, 0, 10005, 10036, 10005, 0, 0, 0,
    0, 0, 0, 10012, 10040, 10012, 0, 0, 0,
    0, 0, 0, 10018, 10042, 10018, 0, 0, 0,
    65, 0, 65, 65, 68, 65, 65, 0, 65,
    80, 0, 80, 80, 85, 80, 80, 0, 80,
    80, 95, 95, 115, 120, 115, 95, 95, 80,
    110, 125, 125, 145, 150, 145, 125, 125, 110,
    80, 95, 95, 115, 120, 115, 95, 95, 80,
    35, 50, 70, 150, 200, 150, 70, 50, 35,
    32, 32, 32, 48, 52, 48, 32, 32, 32
};

constexpr std::array<VL, 90> SAFE_GUARD_BISHOP_ {
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 65, 0, 0, 0, 65, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    55, 0, 0, 65, 75, 65, 0, 0, 55,
    0, 0, 0, 0, 75, 0, 0, 0, 0,
    0, 0, 65, 70, 0, 70, 65, 0, 0
};

constexpr std::array<VL, 90> DANGER_GUARD_BISHOP_ {
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 125, 0, 0, 0, 125, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    115, 0, 0, 130, 140, 130, 0, 0, 115,
    0, 0, 0, 0, 140, 0, 0, 0, 0,
    0, 0, 130, 135, 0, 135, 130, 0, 0
};

constexpr std::array<VL, 90> OPEN_KNIGHT_ {
    255, 246, 265, 258, 265, 258, 265, 246, 255,
    246, 265, 272, 276, 228, 276, 272, 265, 246,
    265, 272, 280, 285, 272, 285, 280, 272, 265,
    272, 282, 298, 288, 300, 288, 298, 282, 272,
    268, 300, 312, 318, 322, 318, 312, 300, 268,
    268, 308, 305, 322, 328, 322, 305, 308, 268,
    280, 335, 312, 335, 318, 335, 312, 335, 280,
    272, 298, 305, 318, 308, 318, 305, 298, 272,
    265, 288, 315, 295, 280, 295, 315, 288, 265,
    262, 265, 268, 285, 265, 285, 268, 265, 262
};

constexpr std::array<VL, 90> END_KNIGHT_ {
    270, 276, 282, 276, 278, 276, 282, 276, 270,
    276, 282, 288, 282, 284, 282, 288, 282, 276,
    282, 288, 295, 295, 298, 295, 295, 288, 282,
    288, 295, 302, 302, 305, 302, 302, 295, 288,
    288, 295, 302, 302, 305, 302, 302, 295, 288,
    295, 302, 310, 310, 315, 310, 310, 302, 295,
    295, 302, 310, 310, 315, 310, 310, 302, 295,
    295, 302, 308, 308, 310, 308, 308, 302, 295,
    288, 295, 302, 302, 302, 302, 302, 295, 288,
    280, 288, 295, 295, 295, 295, 295, 288, 280
};

constexpr std::array<VL, 90> OPEN_ROOK_ {
    575, 615, 610, 640, 605, 640, 610, 615, 575,
    595, 625, 620, 640, 608, 640, 620, 625, 595,
    590, 625, 615, 642, 645, 642, 615, 625, 590,
    610, 630, 618, 645, 655, 645, 618, 630, 610,
    625, 642, 642, 655, 662, 655, 642, 642, 625,
    625, 640, 640, 655, 665, 655, 640, 640, 625,
    620, 645, 645, 658, 668, 658, 645, 645, 620,
    618, 630, 628, 652, 665, 652, 628, 630, 618,
    615, 638, 632, 658, 705, 658, 632, 638, 615,
    612, 625, 622, 645, 652, 645, 622, 625, 612
};

constexpr std::array<VL, 90> END_ROOK_ {
    548, 548, 548, 555, 562, 555, 548, 548, 548,
    548, 548, 548, 555, 562, 555, 548, 548, 548,
    548, 548, 548, 555, 562, 555, 548, 548, 548,
    548, 548, 548, 555, 565, 555, 548, 548, 548,
    548, 548, 552, 560, 568, 560, 552, 548, 548,
    548, 552, 555, 565, 572, 565, 555, 552, 548,
    552, 555, 560, 570, 578, 570, 560, 555, 552,
    555, 558, 560, 572, 585, 572, 560, 558, 555,
    560, 565, 570, 580, 598, 580, 570, 565, 560,
    555, 558, 560, 570, 580, 570, 560, 558, 555
};

constexpr std::array<VL, 90> OPEN_CANNON_ {
    285, 285, 292, 305, 312, 305, 292, 285, 285,
    285, 292, 298, 302, 308, 302, 298, 292, 285,
    292, 290, 308, 305, 318, 305, 308, 290, 292,
    288, 288, 292, 295, 300, 295, 292, 288, 288,
    285, 290, 302, 295, 310, 295, 302, 290, 285,
    288, 290, 295, 295, 312, 295, 295, 290, 288,
    288, 300, 302, 298, 308, 298, 302, 300, 288,
    290, 292, 288, 275, 278, 275, 288, 292, 290,
    292, 292, 288, 272, 262, 272, 288, 292, 292,
    295, 295, 285, 268, 258, 268, 285, 295, 295
};

constexpr std::array<VL, 90> END_CANNON_ {
    292, 292, 295, 308, 315, 308, 295, 292, 292,
    292, 292, 295, 308, 315, 308, 295, 292, 292,
    292, 292, 295, 302, 308, 302, 295, 292, 292,
    292, 292, 295, 302, 308, 302, 295, 292, 292,
    292, 292, 295, 302, 308, 302, 295, 292, 292,
    292, 292, 295, 302, 308, 302, 295, 292, 292,
    292, 292, 295, 300, 305, 300, 295, 292, 292,
    290, 290, 292, 292, 292, 292, 292, 290, 290,
    288, 288, 290, 290, 290, 290, 290, 288, 288,
    285, 285, 288, 288, 288, 288, 288, 285, 285
};

// FIXME: AI-generated code below, may contain bugs and I'll fix them later

namespace eval_detail_ {

constexpr int PHASE_MAX = 32;
constexpr int PHASE_ROOK = 4;
constexpr int PHASE_KNIGHT = 2;
constexpr int PHASE_CANNON = 2;
constexpr int PHASE_ADVISOR = 1;
constexpr int PHASE_BISHOP = 1;

constexpr int MAT_ROOK = 12;
constexpr int MAT_KNIGHT = 5;
constexpr int MAT_CANNON = 5;
constexpr int MAT_ADVISOR = 2;
constexpr int MAT_BISHOP = 2;
constexpr int MAT_PAWN = 1;

constexpr POS FLIP_SQ = 89;
constexpr POS RED_PALACE_HEART = 76;   // 花心
constexpr POS BLACK_PALACE_HEART = 13;

struct SideStat {
    int advisors { 0 };
    int bishops { 0 };
    int knights { 0 };
    int rooks { 0 };
    int cannons { 0 };
    int pawns { 0 };
    int material { 0 };
    int over_river { 0 };
    int phase { 0 };
    POS king { INVALID_POS };
};

POS sq_of(TEAM team, POS pos)
{
    return team == R ? pos : POS(FLIP_SQ - pos);
}

int taper(int open_vl, int end_vl, int phase)
{
    return (open_vl * phase + end_vl * (PHASE_MAX - phase)) / PHASE_MAX;
}

bool crossed_river(TEAM team, POS pos)
{
    return team == R ? pos / 9 < 5 : pos / 9 > 4;
}

SideStat collect_(TEAM team)
{
    SideStat s { };
    const auto& list = team == R ? pos_list_r_ : pos_list_b_;
    for (const POS pos : list) {
        const PTYPE raw = g_board[pos];
        const int id = std::abs(raw);
        switch (id) {
        case R_KING:
            s.king = pos;
            break;
        case R_ADVISOR:
            ++s.advisors;
            s.material += MAT_ADVISOR;
            s.phase += PHASE_ADVISOR;
            break;
        case R_BISHOP:
            ++s.bishops;
            s.material += MAT_BISHOP;
            s.phase += PHASE_BISHOP;
            break;
        case R_KNIGHT:
            ++s.knights;
            s.material += MAT_KNIGHT;
            s.phase += PHASE_KNIGHT;
            if (crossed_river(team, pos)) s.over_river += 2;
            break;
        case R_ROOK:
            ++s.rooks;
            s.material += MAT_ROOK;
            s.phase += PHASE_ROOK;
            if (crossed_river(team, pos)) s.over_river += 3;
            break;
        case R_CANNON:
            ++s.cannons;
            s.material += MAT_CANNON;
            s.phase += PHASE_CANNON;
            if (crossed_river(team, pos)) s.over_river += 2;
            break;
        case R_PAWN:
            ++s.pawns;
            s.material += MAT_PAWN;
            if (crossed_river(team, pos)) s.over_river += 1;
            break;
        default:
            break;
        }
    }
    return s;
}

// 同列无子阻隔：炮对将形成空头（以车的吃法打到将）
bool hollow_cannon_(POS cannon, POS king)
{
    if (cannon % 9 != king % 9) return false;
    const POS lo = std::min(cannon, king);
    const POS hi = std::max(cannon, king);
    for (POS p = lo + 9; p < hi; p = POS(p + 9)) {
        if (g_board[p]) return false;
    }
    return true;
}

int shape_bonus_(TEAM team, const SideStat& me, const SideStat& opp)
{
    int vl = 0;

    // 士象全：中局更愿意保持完整防守
    if (me.advisors == 2 && me.bishops == 2) vl += 18;

    // 缺士怕双车 / 缺象怕炮
    if (opp.advisors == 0 && me.rooks == 2) vl += 36;
    else if (opp.advisors <= 1 && me.rooks >= 1) vl += 12;
    if (opp.bishops == 0 && me.cannons >= 1) vl += 22;
    else if (opp.bishops <= 1 && me.cannons >= 1) vl += 8;

    // 无车怕有车
    if (me.rooks > opp.rooks) vl += 16 * (me.rooks - opp.rooks);

    // 窝心马：严重滞碍双士，扣分
    const POS heart = team == R ? RED_PALACE_HEART : BLACK_PALACE_HEART;
    if (g_board[heart] * team == R_KNIGHT) vl -= 48;

    // 空头炮：子力充足时价值高（经典经验）
    if (me.cannons && opp.king < 90) {
        const auto& list = team == R ? pos_list_r_ : pos_list_b_;
        for (const POS pos : list) {
            if (std::abs(g_board[pos]) != R_CANNON) continue;
            if (!hollow_cannon_(pos, opp.king)) continue;
            const int majors = me.rooks + me.knights + me.cannons;
            vl += 20 + majors * 8;
            break;
        }
    }

    // 过河势：鼓励多子过河取势，但别超过一马
    if (me.over_river > opp.over_river)
        vl += std::min(28, 6 * (me.over_river - opp.over_river));

    return vl;
}

int side_pst_(TEAM team, const SideStat& me, const SideStat& opp, int phase)
{
    // 攻势：子力优、过河势、或对方残缺士象时我方车炮可攻
    const bool attack = (me.material > opp.material)
        || (me.over_river >= opp.over_river + 2 && me.material + 2 >= opp.material)
        || (opp.advisors + opp.bishops <= 2 && me.rooks + me.cannons >= 2);

    // 危位：仅在己方士象不整、或过河重压且少士时抬高士象分（完整士象全不因对方双车虚高）
    const bool danger = (me.advisors + me.bishops < 3)
        || (opp.over_river >= 4 && me.advisors < 2);

    const auto& king_pawn_open = attack ? OPEN_ATTACK_KING_PAWN_ : OPEN_DEFEND_KING_PAWN_;
    const auto& king_pawn_end = attack ? END_ATTACK_KING_PAWN_ : END_DEFEND_KING_PAWN_;
    const auto& guard = danger ? DANGER_GUARD_BISHOP_ : SAFE_GUARD_BISHOP_;

    int vl = 0;
    const auto& list = team == R ? pos_list_r_ : pos_list_b_;
    for (const POS pos : list) {
        const POS sq = sq_of(team, pos);
        switch (std::abs(g_board[pos])) {
        case R_KING:
        case R_PAWN:
            vl += taper(king_pawn_open[sq], king_pawn_end[sq], phase);
            break;
        case R_ADVISOR:
        case R_BISHOP:
            vl += guard[sq];
            break;
        case R_KNIGHT:
            vl += taper(OPEN_KNIGHT_[sq], END_KNIGHT_[sq], phase);
            break;
        case R_ROOK:
            vl += taper(OPEN_ROOK_[sq], END_ROOK_[sq], phase);
            break;
        case R_CANNON:
            vl += taper(OPEN_CANNON_[sq], END_CANNON_[sq], phase);
            break;
        default:
            break;
        }
    }
    vl += shape_bonus_(team, me, opp);
    return vl;
}

} // namespace eval_detail_

VL evaluate()
{
    // TODO: |DEBUG|
    return 0;
    using namespace eval_detail_;
    const SideStat red = collect_(R);
    const SideStat black = collect_(B);
    const int phase = std::clamp(red.phase + black.phase, 0, PHASE_MAX);
    const int vl_red = side_pst_(R, red, black, phase);
    const int vl_black = side_pst_(B, black, red, phase);
    const int vl = vl_red - vl_black + (g_team == R ? 8 : -8);
    return VL(g_team * vl);
}
