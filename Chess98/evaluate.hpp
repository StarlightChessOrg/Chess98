#pragma once
#include "base.hpp"

class WeightMap
{
public:
    WeightMap()
    {
        data.fill({0, 0, 0, 0, 0, 0, 0, 0, 0, 0});
    }
    WeightMap(std::array<std::array<int, 10>, 5> data) : data(data)
    {
    }
    void operator=(std::array<std::array<int, 10>, 5> rightvalue)
    {
        this->data = rightvalue;
    }
    std::array<int, 10> &operator[](int k)
    {
        return this->data[size_t((k <= 4) ? k : (8 - k))];
    }
    std::array<std::array<int, 10>, 5> data;
};

WeightMap OPEN_ATTACK_KING_PAWN_WEIGHT({{
    {0, 0, 0, 21, 21, 87, 117, 117, 117, 27},
    {0, 0, 0, 0, 0, 111, 138, 147, 147, 27},
    {0, 0, 0, 21, 39, 123, 162, 192, 207, 27},
    {10070, 10020, 10000, 0, 0, 162, 177, 222, 252, 33},
    {10100, 10030, 10000, 45, 48, 177, 183, 222, 267, 39},
}});

WeightMap OPEN_DEFEND_KING_PAWN_WEIGHT({{
    {0, 0, 0, 21, 21, 87, 117, 117, 117, 27},
    {0, 0, 0, 0, 0, 111, 138, 147, 147, 27},
    {0, 0, 0, 21, 39, 123, 162, 192, 207, 27},
    {10040, 10010, 10000, 0, 0, 162, 177, 222, 252, 33},
    {10100, 10010, 10000, 45, 48, 177, 183, 222, 267, 39},
}});

WeightMap END_ATTACK_KING_PAWN_WEIGHT({{
    {0, 0, 0, 120, 135, 210, 225, 195, 150, 30},
    {0, 0, 0, 0, 0, 210, 240, 210, 165, 30},
    {0, 0, 0, 105, 120, 195, 240, 210, 180, 30},
    {10070, 10009, 10015, 120, 135, 210, 240, 225, 255, 45},
    {10100, 10030, 10045, 120, 135, 210, 240, 225, 300, 45},
}});

WeightMap END_DEFEND_KING_PAWN_WEIGHT({{
    {0, 0, 0, 60, 75, 75, 105, 75, 30, 30},
    {0, 0, 0, 0, 0, 90, 120, 90, 45, 30},
    {0, 0, 0, 60, 75, 90, 120, 90, 60, 30},
    {10060, 10030, 10000, 60, 75, 105, 135, 105, 135, 45},
    {10100, 10030, 10000, 60, 75, 105, 135, 105, 180, 45},
}});

WeightMap SAFE_GUARD_BISHOP_WEIGHT({{
    {0, 0, 108, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {120, 0, 0, 0, 116, 0, 0, 0, 0, 0},
    {120, 0, 120, 0, 0, 0, 0, 0, 0, 0},
    {0, 129, 129, 0, 0, 0, 0, 0, 0, 0},
}});

WeightMap DANGER_GUARD_BISHOP_WEIGHT({{
    {0, 0, 108, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {120, 0, 0, 0, 116, 0, 0, 0, 0, 0},
    {120, 0, 120, 0, 0, 0, 0, 0, 0, 0},
    {0, 129, 129, 0, 0, 0, 0, 0, 0, 0},
}});

WeightMap OPEN_KNIGHT_WEIGHT({{
    {264, 255, 270, 276, 270, 270, 279, 276, 270, 270},
    {255, 270, 276, 282, 294, 300, 324, 294, 288, 270},
    {270, 276, 282, 294, 303, 297, 300, 297, 309, 270},
    {264, 279, 285, 285, 306, 309, 321, 309, 291, 288},
    {270, 234, 276, 294, 309, 312, 300, 297, 282, 270},
}});

WeightMap END_KNIGHT_WEIGHT({{
    {264, 270, 276, 282, 282, 288, 288, 288, 282, 276},
    {270, 276, 282, 288, 288, 294, 294, 294, 288, 282},
    {276, 282, 288, 294, 294, 300, 300, 300, 294, 288},
    {270, 276, 288, 294, 294, 300, 300, 300, 294, 288},
    {270, 276, 288, 294, 294, 300, 300, 300, 294, 288},
}});

WeightMap OPEN_ROOK_WEIGHT({{
    {582, 600, 594, 612, 624, 624, 618, 618, 618, 618},
    {618, 624, 624, 627, 636, 633, 639, 624, 636, 624},
    {612, 618, 612, 612, 636, 633, 639, 621, 627, 621},
    {636, 636, 636, 636, 642, 642, 648, 642, 648, 639},
    {600, 600, 636, 642, 645, 645, 648, 648, 699, 642},
}});

WeightMap END_ROOK_WEIGHT({{
    {582, 600, 594, 612, 624, 624, 618, 618, 618, 618},
    {618, 624, 624, 627, 636, 633, 639, 624, 636, 624},
    {612, 618, 612, 612, 636, 633, 639, 621, 627, 621},
    {636, 636, 636, 636, 642, 642, 648, 642, 648, 639},
    {600, 600, 636, 642, 645, 645, 648, 648, 699, 642},
}});

WeightMap OPEN_CANNON_WEIGHT({{
    {300, 300, 300, 300, 300, 300, 300, 300, 300, 300},
    {300, 300, 300, 300, 300, 300, 300, 300, 300, 300},
    {300, 300, 300, 300, 300, 300, 300, 300, 300, 300},
    {312, 312, 306, 306, 306, 306, 306, 300, 300, 300},
    {318, 318, 320, 312, 312, 312, 312, 300, 300, 300},
}});

WeightMap END_CANNON_WEIGHT({{
    {288, 288, 291, 288, 285, 288, 288, 291, 294, 300},
    {288, 291, 288, 288, 288, 288, 297, 291, 294, 300},
    {291, 294, 300, 288, 297, 288, 297, 288, 288, 288},
    {297, 294, 297, 288, 288, 288, 294, 273, 276, 273},
    {297, 294, 303, 288, 300, 300, 300, 276, 267, 270},
}});

// 越接近残局，子力会越来越少，因此可以按照给车马炮等棋子的加权分判断对局进程
const int ROOK_MIDGAME_VALUE = 6;
const int KNIGHT_CANNON_MIDGAME_VALUE = 3;
const int OTHER_MIDGAME_VALUE = 1;
const int TOTAL_MIDGAME_VALUE = ROOK_MIDGAME_VALUE * 4 + KNIGHT_CANNON_MIDGAME_VALUE * 8 + OTHER_MIDGAME_VALUE * 18;

// 先行权的基础分值，可以按照出子效率的紧迫程度去调整（开局更紧迫）
const int TOTAL_ADVANCED_VALUE = 3;

// 对方越偏向进攻，过河进入我方地界的棋子就越多，因此可以按照敌方过河子数量调整攻防策略
const int TOTAL_ATTACK_VALUE = 8;
const int ADVISOR_BISHOP_ATTACKLESS_VALUE = 240;
const int TOTAL_ADVISOR_LEAKAGE = 240;

// 开局和残局时兵的基础分数
const int OPEN_PAWN_VAL = 20;
const int END_PAWN_VAL = 40;

/// @brief 实时计算红方视角的估值权重
/// @param vlOpen
/// @param vlRedAttack
/// @param vlBlackAttack
/// @return
std::map<PIECEID, WeightMap> getBasicEvaluateWeights(int vlOpen, int vlRedAttack, int vlBlackAttack)
{
    // 兵，帅
    WeightMap RED_KING_PAWN_WEIGHT;
    WeightMap BLACK_KING_PAWN_WEIGHT;
    for (int x = 0; x < 9; x++)
    {
        for (int y = 0; y < 10; y++)
        {
            // 进攻状态时的游戏进程权重
            int vlAttackGameProcess = vlOpen * OPEN_ATTACK_KING_PAWN_WEIGHT[x][y];
            vlAttackGameProcess += (TOTAL_MIDGAME_VALUE - vlOpen) * END_ATTACK_KING_PAWN_WEIGHT[x][y];
            vlAttackGameProcess /= TOTAL_MIDGAME_VALUE;
            // 防守状态的游戏进程权重
            int vlDefendGameProcess = vlOpen * OPEN_DEFEND_KING_PAWN_WEIGHT[x][y];
            vlDefendGameProcess += (TOTAL_MIDGAME_VALUE - vlOpen) * END_DEFEND_KING_PAWN_WEIGHT[x][y];
            vlDefendGameProcess /= TOTAL_MIDGAME_VALUE;
            // 结合红方的进攻和防守状态权重
            int vlRedSummarize = vlRedAttack * vlAttackGameProcess;
            vlRedSummarize += (TOTAL_ATTACK_VALUE - vlRedAttack) * vlDefendGameProcess;
            vlRedSummarize /= TOTAL_ATTACK_VALUE;
            // 综合黑方的进攻和防守状态权重
            int vlBlackSummarize = vlBlackAttack * vlAttackGameProcess;
            vlBlackSummarize += (TOTAL_ATTACK_VALUE - vlBlackAttack) * vlDefendGameProcess;
            vlBlackSummarize /= TOTAL_ATTACK_VALUE;
            // 设置
            RED_KING_PAWN_WEIGHT[x][y] = vlRedSummarize;
            BLACK_KING_PAWN_WEIGHT[x][y] = vlBlackSummarize;
        }
    }

    // 车
    WeightMap RED_ROOK_WEIGHT;
    WeightMap BLACK_ROOK_WEIGHT;
    for (int x = 0; x < 9; x++)
    {
        for (int y = 0; y < 10; y++)
        {
            int vlSummarise = vlOpen * OPEN_ROOK_WEIGHT[x][y];
            vlSummarise += (TOTAL_MIDGAME_VALUE - vlOpen) * END_ROOK_WEIGHT[x][y];
            vlSummarise /= TOTAL_MIDGAME_VALUE;
            // 设置
            RED_ROOK_WEIGHT[x][y] = vlSummarise;
            BLACK_ROOK_WEIGHT[x][y] = vlSummarise;
        }
    }

    // 马
    WeightMap RED_KNIGHT_WEIGHT;
    WeightMap BLACK_KNIGHT_WEIGHT;
    for (int x = 0; x < 9; x++)
    {
        for (int y = 0; y < 10; y++)
        {
            int vlSummarise = vlOpen * OPEN_KNIGHT_WEIGHT[x][y];
            vlSummarise += (TOTAL_MIDGAME_VALUE - vlOpen) * END_KNIGHT_WEIGHT[x][y];
            vlSummarise /= TOTAL_MIDGAME_VALUE;
            // 设置
            RED_KNIGHT_WEIGHT[x][y] = vlSummarise;
            BLACK_KNIGHT_WEIGHT[x][y] = vlSummarise;
        }
    }

    // 炮
    WeightMap RED_CANNON_WEIGHT;
    WeightMap BLACK_CANNON_WEIGHT;
    for (int x = 0; x < 9; x++)
    {
        for (int y = 0; y < 10; y++)
        {
            int vlSummarise = vlOpen * OPEN_CANNON_WEIGHT[x][y];
            vlSummarise += (TOTAL_MIDGAME_VALUE - vlOpen) * END_CANNON_WEIGHT[x][y];
            vlSummarise /= TOTAL_MIDGAME_VALUE;
            // 设置
            RED_CANNON_WEIGHT[x][y] = vlSummarise;
            BLACK_CANNON_WEIGHT[x][y] = vlSummarise;
        }
    }

    // 士，象
    WeightMap RED_GUARD_BISHOP_WEIGHT;
    WeightMap BLACK_GUARD_BISHOP_WEIGHT;
    for (int x = 0; x < 9; x++)
    {
        for (int y = 0; y < 10; y++)
        {
            // 红
            int vlRedSummarise = vlBlackAttack * DANGER_GUARD_BISHOP_WEIGHT[x][y];
            vlRedSummarise += (TOTAL_ATTACK_VALUE - vlBlackAttack) * SAFE_GUARD_BISHOP_WEIGHT[x][y];
            vlRedSummarise /= TOTAL_ATTACK_VALUE;
            // 黑
            int vlBlackSummarise = vlRedAttack * DANGER_GUARD_BISHOP_WEIGHT[x][y];
            vlBlackSummarise += (TOTAL_ATTACK_VALUE - vlRedAttack) * SAFE_GUARD_BISHOP_WEIGHT[x][y];
            vlBlackSummarise /= TOTAL_ATTACK_VALUE;
            // 设置
            RED_GUARD_BISHOP_WEIGHT[x][y] = vlRedSummarise;
            BLACK_GUARD_BISHOP_WEIGHT[x][y] = vlBlackSummarise;
        }
    }

    std::map<PIECEID, WeightMap> pieceWeights{
        {R_KING, RED_KING_PAWN_WEIGHT}, {R_GUARD, RED_GUARD_BISHOP_WEIGHT}, {R_BISHOP, RED_GUARD_BISHOP_WEIGHT}, {R_KNIGHT, RED_KNIGHT_WEIGHT}, {R_ROOK, RED_ROOK_WEIGHT}, {R_CANNON, RED_CANNON_WEIGHT}, {R_PAWN, RED_KING_PAWN_WEIGHT}, {B_KING, BLACK_KING_PAWN_WEIGHT}, {B_GUARD, BLACK_GUARD_BISHOP_WEIGHT}, {B_BISHOP, BLACK_GUARD_BISHOP_WEIGHT}, {B_KNIGHT, BLACK_KNIGHT_WEIGHT}, {B_ROOK, BLACK_ROOK_WEIGHT}, {B_CANNON, BLACK_CANNON_WEIGHT}, {B_PAWN, BLACK_KING_PAWN_WEIGHT}};

    return pieceWeights;
}

std::map<PIECEID, WeightMap> pieceWeights;
int vlAdvanced = 0;
int vlPawn = 0;

// 剪裁的边界参数

const int MAX_SEARCH_DISTANCE = 64;
const int DELTA_PRUNING_MARGIN = 300;
const int FUTILITY_PRUNING_MARGIN = 400;
