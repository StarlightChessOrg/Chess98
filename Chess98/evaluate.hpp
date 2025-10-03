#pragma once
#include "nnue.hpp"
#include "base.hpp"

using WEIGHT_MAP = std::array<std::array<int, 10>, 9>;

// 权重表所记录的数值都是红方位置 -> 分数, 黑方评分则需要倒置这张表, 我的习惯是红在上, 横放

// OPEN,END 开局, 残局
// ATTACK, DEFEND 进攻, 防守
// SAFE,DANGER 安全, 受到威胁的

WEIGHT_MAP OPEN_ATTACK_KING_PAWN_WEIGHT = {
    {{0, 0, 0, 21, 21, 67, 97, 97, 97, 7},
     {0, 0, 0, 0, 0, 91, 118, 127, 127, 7},
     {0, 0, 0, 21, 39, 103, 142, 172, 187, 7},
     {10037, 10006, 10000, 0, 0, 142, 157, 202, 232, 13},
     {10053, 10020, 10000, 45, 48, 157, 163, 202, 247, 19},
     {10037, 10006, 10000, 0, 0, 142, 157, 202, 232, 13},
     {0, 0, 0, 21, 39, 103, 142, 172, 187, 7},
     {0, 0, 0, 0, 0, 91, 118, 127, 127, 7},
     {0, 0, 0, 21, 21, 67, 97, 97, 97, 7}}};

WEIGHT_MAP OPEN_DEFEND_KING_PAWN_WEIGHT = {
    {
        {0, 0, 0, 21, 21, 87, 117, 117, 117, 27},
        {0, 0, 0, 0, 0, 111, 138, 147, 147, 27},
        {0, 0, 0, 21, 39, 123, 162, 192, 207, 27},
        {10033, 10006, 10003, 0, 0, 162, 177, 222, 252, 33},
        {10045, 10006, 10003, 45, 48, 177, 183, 222, 267, 39},
        {10033, 10006, 10003, 0, 0, 162, 177, 222, 252, 33},
        {0, 0, 0, 21, 39, 123, 162, 192, 207, 27},
        {0, 0, 0, 0, 0, 111, 138, 147, 147, 27},
        {0, 0, 0, 21, 21, 87, 117, 117, 117, 27},
    }};

WEIGHT_MAP END_ATTACK_KING_PAWN_WEIGHT = {
    {{0, 0, 0, 120, 135, 190, 205, 175, 130, 10},
     {0, 0, 0, 0, 0, 190, 220, 190, 145, 10},
     {0, 0, 0, 105, 120, 175, 220, 190, 160, 10},
     {10003, 10009, 10015, 120, 135, 190, 220, 205, 235, 25},
     {10033, 10039, 10045, 120, 135, 190, 220, 205, 280, 25},
     {10003, 10009, 10015, 120, 135, 190, 220, 205, 235, 25},
     {0, 0, 0, 105, 120, 175, 220, 190, 160, 10},
     {0, 0, 0, 0, 0, 190, 220, 190, 145, 10},
     {0, 0, 0, 120, 135, 190, 205, 175, 130, 10}}};

WEIGHT_MAP END_DEFEND_KING_PAWN_WEIGHT = {
    {
        {0, 0, 0, 60, 75, 75, 105, 75, 30, 30},
        {0, 0, 0, 0, 0, 90, 120, 90, 45, 30},
        {0, 0, 0, 60, 75, 90, 120, 90, 60, 30},
        {10003, 10009, 10015, 60, 75, 105, 135, 105, 135, 45},
        {10033, 10036, 10039, 60, 75, 105, 135, 105, 180, 45},
        {10003, 10009, 10015, 60, 75, 105, 135, 105, 135, 45},
        {0, 0, 0, 60, 75, 90, 120, 90, 60, 30},
        {0, 0, 0, 0, 0, 90, 120, 90, 45, 30},
        {0, 0, 0, 60, 75, 75, 105, 75, 30, 30},
    }};

WEIGHT_MAP SAFE_GUARD_BISHOP_WEIGHT = {
    {
        {0, 0, 50, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {60, 0, 0, 0, 60, 0, 0, 0, 0, 0},
        {60, 0, 60, 0, 0, 0, 0, 0, 0, 0},
        {0, 69, 69, 0, 0, 0, 0, 0, 0, 0},
        {60, 0, 60, 0, 0, 0, 0, 0, 0, 0},
        {60, 0, 0, 0, 60, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 50, 0, 0, 0, 0, 0, 0, 0},
    }};

WEIGHT_MAP DANGER_GUARD_BISHOP_WEIGHT = {
    {
        {0, 0, 108, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {120, 0, 0, 0, 116, 0, 0, 0, 0, 0},
        {120, 0, 120, 0, 0, 0, 0, 0, 0, 0},
        {0, 129, 129, 0, 0, 0, 0, 0, 0, 0},
        {120, 0, 120, 0, 0, 0, 0, 0, 0, 0},
        {120, 0, 0, 0, 116, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 108, 0, 0, 0, 0, 0, 0, 0},
    }};

WEIGHT_MAP OPEN_KNIGHT_WEIGHT = {
    {
        {264, 255, 270, 276, 270, 270, 279, 276, 270, 270},
        {255, 270, 276, 282, 294, 300, 324, 294, 288, 270},
        {270, 276, 282, 294, 303, 297, 300, 297, 309, 270},
        {264, 279, 285, 285, 306, 309, 321, 309, 291, 288},
        {270, 234, 276, 294, 309, 312, 300, 297, 282, 270},
        {264, 279, 285, 285, 306, 309, 321, 309, 291, 288},
        {270, 276, 282, 294, 303, 297, 300, 297, 309, 270},
        {255, 270, 276, 282, 294, 300, 324, 294, 288, 270},
        {264, 255, 270, 276, 270, 270, 279, 276, 270, 270},
    }};

WEIGHT_MAP END_KNIGHT_WEIGHT = {
    {
        {264, 270, 276, 282, 282, 288, 288, 288, 282, 276},
        {270, 276, 282, 288, 288, 294, 294, 294, 288, 282},
        {276, 282, 288, 294, 294, 300, 300, 300, 294, 288},
        {270, 276, 288, 294, 294, 300, 300, 300, 294, 288},
        {270, 276, 288, 294, 294, 300, 300, 300, 294, 288},
        {270, 276, 288, 294, 294, 300, 300, 300, 294, 288},
        {276, 282, 288, 294, 294, 300, 300, 300, 294, 288},
        {270, 276, 282, 288, 288, 294, 294, 294, 288, 282},
        {264, 270, 276, 282, 282, 288, 288, 288, 282, 276},
    }};

WEIGHT_MAP OPEN_ROOK_WEIGHT = {
    {
        {582, 600, 594, 612, 624, 624, 618, 618, 618, 618},
        {618, 624, 624, 627, 636, 633, 639, 624, 636, 624},
        {612, 618, 612, 612, 636, 633, 639, 621, 627, 621},
        {636, 636, 636, 636, 642, 642, 648, 642, 648, 639},
        {600, 600, 636, 642, 645, 645, 648, 648, 699, 642},
        {636, 636, 636, 636, 642, 642, 648, 642, 648, 639},
        {612, 618, 612, 612, 636, 633, 639, 621, 627, 621},
        {618, 624, 624, 627, 636, 633, 639, 624, 636, 624},
        {582, 600, 594, 612, 624, 624, 618, 618, 618, 618},
    }};

WEIGHT_MAP END_ROOK_WEIGHT = {
    {
        {540, 540, 540, 540, 540, 540, 540, 546, 552, 546},
        {540, 540, 540, 540, 540, 540, 540, 546, 552, 546},
        {540, 540, 540, 540, 540, 540, 540, 546, 552, 546},
        {546, 546, 546, 546, 546, 546, 546, 552, 558, 552},
        {552, 552, 552, 552, 552, 552, 552, 558, 570, 558},
        {546, 546, 546, 546, 546, 546, 546, 552, 558, 552},
        {540, 540, 540, 540, 540, 540, 540, 546, 552, 546},
        {540, 540, 540, 540, 540, 540, 540, 546, 552, 546},
        {540, 540, 540, 540, 540, 540, 540, 546, 552, 546},
    }};

WEIGHT_MAP OPEN_CANNON_WEIGHT = {
    {
        {288, 288, 291, 288, 285, 288, 288, 291, 294, 300},
        {288, 291, 288, 288, 288, 288, 297, 291, 294, 300},
        {291, 294, 300, 288, 297, 288, 297, 288, 288, 288},
        {297, 294, 297, 288, 288, 288, 294, 273, 276, 273},
        {297, 294, 303, 288, 300, 300, 300, 276, 267, 270},
        {297, 294, 297, 288, 288, 288, 294, 273, 276, 273},
        {291, 294, 300, 288, 297, 288, 297, 288, 288, 288},
        {288, 291, 288, 288, 288, 288, 297, 291, 294, 300},
        {288, 288, 291, 288, 285, 288, 288, 291, 294, 300},
    }};

WEIGHT_MAP END_CANNON_WEIGHT = {
    {
        {300, 300, 300, 300, 300, 300, 300, 300, 300, 300},
        {300, 300, 300, 300, 300, 300, 300, 300, 300, 300},
        {300, 300, 300, 300, 300, 300, 300, 300, 300, 300},
        {312, 312, 306, 306, 306, 306, 306, 300, 300, 300},
        {318, 318, 312, 312, 312, 312, 312, 300, 300, 300},
        {312, 312, 306, 306, 306, 306, 306, 300, 300, 300},
        {300, 300, 300, 300, 300, 300, 300, 300, 300, 300},
        {300, 300, 300, 300, 300, 300, 300, 300, 300, 300},
        {300, 300, 300, 300, 300, 300, 300, 300, 300, 300},
    }};

// 越接近残局, 子力会越来越少, 因此可以按照给车马炮等棋子的加权分判断对局进程

const int ROOK_MIDGAME_VALUE = 6;
const int KNIGHT_CANNON_MIDGAME_VALUE = 3;
const int OTHER_MIDGAME_VALUE = 1;
const int TOTAL_MIDGAME_VALUE = ROOK_MIDGAME_VALUE * 4 + KNIGHT_CANNON_MIDGAME_VALUE * 8 + OTHER_MIDGAME_VALUE * 18;

// 先行权的基础分值, 可以按照出子效率的紧迫程度去调整（开局更紧迫）

const int TOTAL_ADVANCED_VALUE = 3;

// 对方越偏向进攻, 过河进入我方地界的棋子就越多, 因此可以按照敌方过河子数量调整攻防策略

const int TOTAL_ATTACK_VALUE = 8;
const int ADVISOR_BISHOP_ATTACKLESS_VALUE = 60 * 4;

// 开局和残局时兵的基础分数
const int OPEN_PAWN_VAL = 30;
const int END_PAWN_VAL = 120;

// 实时计算红方视角的估值权重

std::map<PIECEID, WEIGHT_MAP> getBasicEvaluateWeights(int vlOpen, int vlRedAttack, int vlBlackAttack)
{
    // 兵, 帅
    WEIGHT_MAP RED_KING_PAWN_WEIGHT = {0};
    WEIGHT_MAP BLACK_KING_PAWN_WEIGHT = {0};
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
    WEIGHT_MAP RED_ROOK_WEIGHT = {0};
    WEIGHT_MAP BLACK_ROOK_WEIGHT = {0};
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
    WEIGHT_MAP RED_KNIGHT_WEIGHT = {0};
    WEIGHT_MAP BLACK_KNIGHT_WEIGHT = {0};
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
    WEIGHT_MAP RED_CANNON_WEIGHT = {0};
    WEIGHT_MAP BLACK_CANNON_WEIGHT = {0};
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

    // 士, 象
    WEIGHT_MAP RED_GUARD_BISHOP_WEIGHT = {0};
    WEIGHT_MAP BLACK_GUARD_BISHOP_WEIGHT = {0};
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

    std::map<PIECEID, WEIGHT_MAP> pieceWeights{
        {R_KING, RED_KING_PAWN_WEIGHT},
        {R_GUARD, RED_GUARD_BISHOP_WEIGHT},
        {R_BISHOP, RED_GUARD_BISHOP_WEIGHT},
        {R_KNIGHT, RED_KNIGHT_WEIGHT},
        {R_ROOK, RED_ROOK_WEIGHT},
        {R_CANNON, RED_CANNON_WEIGHT},
        {R_PAWN, RED_KING_PAWN_WEIGHT},
        {B_KING, BLACK_KING_PAWN_WEIGHT},
        {B_GUARD, BLACK_GUARD_BISHOP_WEIGHT},
        {B_BISHOP, BLACK_GUARD_BISHOP_WEIGHT},
        {B_KNIGHT, BLACK_KNIGHT_WEIGHT},
        {B_ROOK, BLACK_ROOK_WEIGHT},
        {B_CANNON, BLACK_CANNON_WEIGHT},
        {B_PAWN, BLACK_KING_PAWN_WEIGHT}};

    return pieceWeights;
}

std::map<PIECEID, WEIGHT_MAP> pieceWeights;
int vlAdvanced = 0;
int vlPawn = 0;

// 剪裁的边界参数

const int MAX_SEARCH_DISTANCE = 32;
const int DELTA_PRUNING_MARGIN = 300;
const int FUTILITY_PRUNING_MARGIN = 400;

const int INITIAL_BOTTOM_CANNON_REWARD = 40;
const int TERMINAL_BOTTOM_CANNON_REWARD = 20;

const int INITIAL_CENTER_CANNON_REWARD = 50;
const int TERMINAL_CENTER_CANNON_REWARD = 20;
