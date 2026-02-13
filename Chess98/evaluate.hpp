#pragma once
#include "base.hpp"

int vlAdvanced = 0;
int vlPawn = 0;

PID_MATRIX OPEN_ATTACK_KING_PAWN_WEIGHT = { {
    { 0, 0, 0, 21, 21, 67, 97, 97, 97, 7 },
    { 0, 0, 0, 0, 0, 91, 118, 127, 127, 7 },
    { 0, 0, 0, 21, 39, 103, 142, 172, 187, 7 },
    { 10037, 10006, 10000, 0, 0, 142, 157, 202, 232, 13 },
    { 10053, 10020, 10000, 45, 48, 157, 163, 202, 247, 19 },
    { 10037, 10006, 10000, 0, 0, 142, 157, 202, 232, 13 },
    { 0, 0, 0, 21, 39, 103, 142, 172, 187, 7 },
    { 0, 0, 0, 0, 0, 91, 118, 127, 127, 7 },
    { 0, 0, 0, 21, 21, 67, 97, 97, 97, 7 },
} };

PID_MATRIX OPEN_DEFEND_KING_PAWN_WEIGHT = { {
    { 0, 0, 0, 21, 21, 87, 117, 117, 117, 27 },
    { 0, 0, 0, 0, 0, 111, 138, 147, 147, 27 },
    { 0, 0, 0, 21, 39, 123, 162, 192, 207, 27 },
    { 10033, 10006, 10003, 0, 0, 162, 177, 222, 252, 33 },
    { 10045, 10006, 10003, 45, 48, 177, 183, 222, 267, 39 },
    { 10033, 10006, 10003, 0, 0, 162, 177, 222, 252, 33 },
    { 0, 0, 0, 21, 39, 123, 162, 192, 207, 27 },
    { 0, 0, 0, 0, 0, 111, 138, 147, 147, 27 },
    { 0, 0, 0, 21, 21, 87, 117, 117, 117, 27 },
} };

PID_MATRIX END_ATTACK_KING_PAWN_WEIGHT = { {
    { 0, 0, 0, 120, 135, 190, 205, 175, 130, 10 },
    { 0, 0, 0, 0, 0, 190, 220, 190, 145, 10 },
    { 0, 0, 0, 105, 120, 175, 220, 190, 160, 10 },
    { 10003, 10009, 10015, 120, 135, 190, 220, 205, 235, 25 },
    { 10033, 10039, 10045, 120, 135, 190, 220, 205, 280, 25 },
    { 10003, 10009, 10015, 120, 135, 190, 220, 205, 235, 25 },
    { 0, 0, 0, 105, 120, 175, 220, 190, 160, 10 },
    { 0, 0, 0, 0, 0, 190, 220, 190, 145, 10 },
    { 0, 0, 0, 120, 135, 190, 205, 175, 130, 10 },
} };

PID_MATRIX END_DEFEND_KING_PAWN_WEIGHT = { {
    { 0, 0, 0, 60, 75, 75, 105, 75, 30, 30 },
    { 0, 0, 0, 0, 0, 90, 120, 90, 45, 30 },
    { 0, 0, 0, 60, 75, 90, 120, 90, 60, 30 },
    { 10003, 10009, 10015, 60, 75, 105, 135, 105, 135, 45 },
    { 10033, 10036, 10039, 60, 75, 105, 135, 105, 180, 45 },
    { 10003, 10009, 10015, 60, 75, 105, 135, 105, 135, 45 },
    { 0, 0, 0, 60, 75, 90, 120, 90, 60, 30 },
    { 0, 0, 0, 0, 0, 90, 120, 90, 45, 30 },
    { 0, 0, 0, 60, 75, 75, 105, 75, 30, 30 },
} };

PID_MATRIX SAFE_GUARD_BISHOP_WEIGHT = { {
    { 0, 0, 50, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 60, 0, 0, 0, 60, 0, 0, 0, 0, 0 },
    { 60, 0, 60, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 69, 69, 0, 0, 0, 0, 0, 0, 0 },
    { 60, 0, 60, 0, 0, 0, 0, 0, 0, 0 },
    { 60, 0, 0, 0, 60, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 50, 0, 0, 0, 0, 0, 0, 0 },
} };

PID_MATRIX DANGER_GUARD_BISHOP_WEIGHT = { {
    { 0, 0, 108, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 120, 0, 0, 0, 116, 0, 0, 0, 0, 0 },
    { 120, 0, 120, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 129, 129, 0, 0, 0, 0, 0, 0, 0 },
    { 120, 0, 120, 0, 0, 0, 0, 0, 0, 0 },
    { 120, 0, 0, 0, 116, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 108, 0, 0, 0, 0, 0, 0, 0 },
} };

PID_MATRIX OPEN_KNIGHT_WEIGHT = { {
    { 264, 255, 270, 276, 270, 270, 279, 276, 270, 270 },
    { 255, 270, 276, 282, 294, 300, 324, 294, 288, 270 },
    { 270, 276, 282, 294, 303, 297, 300, 297, 309, 270 },
    { 264, 279, 285, 285, 306, 309, 321, 309, 291, 288 },
    { 270, 234, 276, 294, 309, 312, 300, 297, 282, 270 },
    { 264, 279, 285, 285, 306, 309, 321, 309, 291, 288 },
    { 270, 276, 282, 294, 303, 297, 300, 297, 309, 270 },
    { 255, 270, 276, 282, 294, 300, 324, 294, 288, 270 },
    { 264, 255, 270, 276, 270, 270, 279, 276, 270, 270 },
} };

PID_MATRIX END_KNIGHT_WEIGHT = { {
    { 264, 270, 276, 282, 282, 288, 288, 288, 282, 276 },
    { 270, 276, 282, 288, 288, 294, 294, 294, 288, 282 },
    { 276, 282, 288, 294, 294, 300, 300, 300, 294, 288 },
    { 270, 276, 288, 294, 294, 300, 300, 300, 294, 288 },
    { 270, 276, 288, 294, 294, 300, 300, 300, 294, 288 },
    { 270, 276, 288, 294, 294, 300, 300, 300, 294, 288 },
    { 276, 282, 288, 294, 294, 300, 300, 300, 294, 288 },
    { 270, 276, 282, 288, 288, 294, 294, 294, 288, 282 },
    { 264, 270, 276, 282, 282, 288, 288, 288, 282, 276 },
} };

PID_MATRIX OPEN_ROOK_WEIGHT = { {
    { 582, 600, 594, 612, 624, 624, 618, 618, 618, 618 },
    { 618, 624, 624, 627, 636, 633, 639, 624, 636, 624 },
    { 612, 618, 612, 612, 636, 633, 639, 621, 627, 621 },
    { 636, 636, 636, 636, 642, 642, 648, 642, 648, 639 },
    { 600, 600, 636, 642, 645, 645, 648, 648, 699, 642 },
    { 636, 636, 636, 636, 642, 642, 648, 642, 648, 639 },
    { 612, 618, 612, 612, 636, 633, 639, 621, 627, 621 },
    { 618, 624, 624, 627, 636, 633, 639, 624, 636, 624 },
    { 582, 600, 594, 612, 624, 624, 618, 618, 618, 618 },
} };

PID_MATRIX END_ROOK_WEIGHT = { {
    { 540, 540, 540, 540, 540, 540, 540, 546, 552, 546 },
    { 540, 540, 540, 540, 540, 540, 540, 546, 552, 546 },
    { 540, 540, 540, 540, 540, 540, 540, 546, 552, 546 },
    { 546, 546, 546, 546, 546, 546, 546, 552, 558, 552 },
    { 552, 552, 552, 552, 552, 552, 552, 558, 570, 558 },
    { 546, 546, 546, 546, 546, 546, 546, 552, 558, 552 },
    { 540, 540, 540, 540, 540, 540, 540, 546, 552, 546 },
    { 540, 540, 540, 540, 540, 540, 540, 546, 552, 546 },
    { 540, 540, 540, 540, 540, 540, 540, 546, 552, 546 },
} };

PID_MATRIX OPEN_CANNON_WEIGHT = { {
    { 288, 288, 291, 288, 285, 288, 288, 291, 294, 300 },
    { 288, 291, 288, 288, 288, 288, 297, 291, 294, 300 },
    { 291, 294, 300, 288, 297, 288, 297, 288, 288, 288 },
    { 297, 294, 297, 288, 288, 288, 294, 273, 276, 273 },
    { 297, 294, 303, 288, 300, 300, 300, 276, 267, 270 },
    { 297, 294, 297, 288, 288, 288, 294, 273, 276, 273 },
    { 291, 294, 300, 288, 297, 288, 297, 288, 288, 288 },
    { 288, 291, 288, 288, 288, 288, 297, 291, 294, 300 },
    { 288, 288, 291, 288, 285, 288, 288, 291, 294, 300 },
} };

PID_MATRIX END_CANNON_WEIGHT = { {
    { 300, 300, 300, 300, 300, 300, 300, 300, 300, 300 },
    { 300, 300, 300, 300, 300, 300, 300, 300, 300, 300 },
    { 300, 300, 300, 300, 300, 300, 300, 300, 300, 300 },
    { 312, 312, 306, 306, 306, 306, 306, 300, 300, 300 },
    { 318, 318, 312, 312, 312, 312, 312, 300, 300, 300 },
    { 312, 312, 306, 306, 306, 306, 306, 300, 300, 300 },
    { 300, 300, 300, 300, 300, 300, 300, 300, 300, 300 },
    { 300, 300, 300, 300, 300, 300, 300, 300, 300, 300 },
    { 300, 300, 300, 300, 300, 300, 300, 300, 300, 300 },
} };

// 越接近残局, 子力会越来越少, 因此可以按照给车马炮等棋子的加权分判断对局进程
const int ROOK_MIDGAME_VALUE = 6;
const int KNIGHT_CANNON_MIDGAME_VALUE = 3;
const int OTHER_MIDGAME_VALUE = 1;
const int TOTAL_MIDGAME_VALUE = ROOK_MIDGAME_VALUE * 4 + KNIGHT_CANNON_MIDGAME_VALUE * 8 + OTHER_MIDGAME_VALUE * 18;

// 先行权的基础分值, 可以按照出子效率的紧迫程度去调整（开局更紧迫）
const int TOTAL_ADVANCED_VALUE = 10;

// 对方越偏向进攻, 过河进入我方地界的棋子就越多, 因此可以按照敌方过河子数量调整攻防策略
const int TOTAL_ATTACK_VALUE = 8;
const int ADVISOR_BISHOP_ATTACKLESS_VALUE = 60 * 4;

// 开局和残局时兵的基础分数
const int OPEN_PAWN_VAL = 30;
const int END_PAWN_VAL = 120;

// 实时计算红方视角的估值权重
std::map<PIECEID, PID_MATRIX> getBasicEvaluateWeights(int vlOpen, int vlRedAttack, int vlBlackAttack)
{
    // 兵, 帅
    PID_MATRIX RED_KING_PAWN_WEIGHT = { 0 };
    PID_MATRIX BLACK_KING_PAWN_WEIGHT = { 0 };
    for (int x = 0; x < 9; x++) {
        for (int y = 0; y < 10; y++) {
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
    PID_MATRIX RED_ROOK_WEIGHT = { 0 };
    PID_MATRIX BLACK_ROOK_WEIGHT = { 0 };
    for (int x = 0; x < 9; x++) {
        for (int y = 0; y < 10; y++) {
            int vlSummarise = vlOpen * OPEN_ROOK_WEIGHT[x][y];
            vlSummarise += (TOTAL_MIDGAME_VALUE - vlOpen) * END_ROOK_WEIGHT[x][y];
            vlSummarise /= TOTAL_MIDGAME_VALUE;
            // 设置
            RED_ROOK_WEIGHT[x][y] = vlSummarise;
            BLACK_ROOK_WEIGHT[x][y] = vlSummarise;
        }
    }

    // 马
    PID_MATRIX RED_KNIGHT_WEIGHT = { 0 };
    PID_MATRIX BLACK_KNIGHT_WEIGHT = { 0 };
    for (int x = 0; x < 9; x++) {
        for (int y = 0; y < 10; y++) {
            int vlSummarise = vlOpen * OPEN_KNIGHT_WEIGHT[x][y];
            vlSummarise += (TOTAL_MIDGAME_VALUE - vlOpen) * END_KNIGHT_WEIGHT[x][y];
            vlSummarise /= TOTAL_MIDGAME_VALUE;
            // 设置
            RED_KNIGHT_WEIGHT[x][y] = vlSummarise;
            BLACK_KNIGHT_WEIGHT[x][y] = vlSummarise;
        }
    }

    // 炮
    PID_MATRIX RED_CANNON_WEIGHT = { 0 };
    PID_MATRIX BLACK_CANNON_WEIGHT = { 0 };
    for (int x = 0; x < 9; x++) {
        for (int y = 0; y < 10; y++) {
            int vlSummarise = vlOpen * OPEN_CANNON_WEIGHT[x][y];
            vlSummarise += (TOTAL_MIDGAME_VALUE - vlOpen) * END_CANNON_WEIGHT[x][y];
            vlSummarise /= TOTAL_MIDGAME_VALUE;
            // 设置
            RED_CANNON_WEIGHT[x][y] = vlSummarise;
            BLACK_CANNON_WEIGHT[x][y] = vlSummarise;
        }
    }

    // 士, 象
    PID_MATRIX RED_GUARD_BISHOP_WEIGHT = { 0 };
    PID_MATRIX BLACK_GUARD_BISHOP_WEIGHT = { 0 };
    for (int x = 0; x < 9; x++) {
        for (int y = 0; y < 10; y++) {
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

    return { { R_KING, RED_KING_PAWN_WEIGHT }, { R_GUARD, RED_GUARD_BISHOP_WEIGHT }, { R_BISHOP, RED_GUARD_BISHOP_WEIGHT },
        { R_KNIGHT, RED_KNIGHT_WEIGHT }, { R_ROOK, RED_ROOK_WEIGHT }, { R_CANNON, RED_CANNON_WEIGHT },
        { R_PAWN, RED_KING_PAWN_WEIGHT }, { B_KING, BLACK_KING_PAWN_WEIGHT }, { B_GUARD, BLACK_GUARD_BISHOP_WEIGHT },
        { B_BISHOP, BLACK_GUARD_BISHOP_WEIGHT }, { B_KNIGHT, BLACK_KNIGHT_WEIGHT }, { B_ROOK, BLACK_ROOK_WEIGHT },
        { B_CANNON, BLACK_CANNON_WEIGHT }, { B_PAWN, BLACK_KING_PAWN_WEIGHT } };
}

void Board::initEvaluate()
{
    // 更新权重数组
    int vlOpen = 0;
    int vlRedAttack = 0;
    int vlBlackAttack = 0;
    this->calculateVlOpen(vlOpen);
    this->vlAttackCalculator(vlRedAttack, vlBlackAttack);

    pieceWeights = getBasicEvaluateWeights(vlOpen, vlRedAttack, vlBlackAttack);
    vlAdvanced = (TOTAL_ADVANCED_VALUE * vlOpen + TOTAL_ADVANCED_VALUE / 2) / TOTAL_MIDGAME_VALUE;
    vlPawn = (vlOpen * OPEN_PAWN_VAL + (TOTAL_MIDGAME_VALUE - vlOpen) * END_PAWN_VAL) / TOTAL_MIDGAME_VALUE;

    // 调整不受威胁方少掉的士象分
    this->vlRed = ADVISOR_BISHOP_ATTACKLESS_VALUE * (TOTAL_ATTACK_VALUE - vlBlackAttack) / TOTAL_ATTACK_VALUE;
    this->vlBlack = ADVISOR_BISHOP_ATTACKLESS_VALUE * (TOTAL_ATTACK_VALUE - vlRedAttack) / TOTAL_ATTACK_VALUE;

    // 进一步重新计算分数
    for (int x = 0; x < 9; x++) {
        for (int y = 0; y < 10; y++) {
            PIECEID pid = this->pid_matrix[x][y];
            if (pid > 0) {
                this->vlRed += pieceWeights[pid][x][y];
            } else if (pid < 0) {
                this->vlBlack += pieceWeights[pid][x][size_t(9) - y];
            }
        }
    }
}

void Board::calculateVlOpen(int& vlOpen) const
{
    // 首先判断局势处于开中局还是残局阶段, 方法是计算各种棋子的数量, 按照车=6、马炮=3、其它=1相加
    int rookLiveSum = 0;
    int knightCannonLiveSum = 0;
    int otherLiveSum = 0;
    for (const Piece& piece : this->getAllLivePieces()) {
        PIECEID pid = std::abs(piece.pieceid);
        if (pid == R_ROOK) {
            rookLiveSum++;
        } else if (pid == R_KNIGHT || pid == R_CANNON) {
            knightCannonLiveSum++;
        } else if (pid != R_KING) {
            otherLiveSum++;
        }
    }
    vlOpen = rookLiveSum * 6 + knightCannonLiveSum * 3 + otherLiveSum;
    // 使用二次函数, 子力很少时才认为接近残局
    vlOpen = (2 * TOTAL_MIDGAME_VALUE - vlOpen) * vlOpen;
    vlOpen /= TOTAL_MIDGAME_VALUE;
}

void Board::vlAttackCalculator(int& vlRedAttack, int& vlBlackAttack) const
{
    // 然后判断各方是否处于进攻状态, 方法是计算各种过河棋子的数量, 按照车马2炮兵1相加
    int redAttackLiveRookSum = 0;
    int blackAttackLiveRookSum = 0;
    int redAttackLiveKnightSum = 0;
    int blackAttackLiveKnightSum = 0;
    int redAttackLiveCannonSum = 0;
    int blackAttackLiveCannonSum = 0;
    int redAttackLivePawnSum = 0;
    int blackAttackLivePawnSum = 0;
    for (const Piece& piece : this->getAllLivePieces()) {
        PIECEID pid = std::abs(piece.pieceid);
        if (piece.team == RED) {
            if (piece.y >= 5) {
                if (pid == R_ROOK) {
                    redAttackLiveRookSum++;
                } else if (pid == R_CANNON) {
                    redAttackLiveCannonSum++;
                } else if (pid == R_KNIGHT) {
                    redAttackLiveKnightSum++;
                } else if (pid == R_PAWN) {
                    redAttackLivePawnSum++;
                }
            }
        } else if (piece.team == BLACK) {
            if (piece.y <= 4) {
                if (pid == R_ROOK) {
                    blackAttackLiveRookSum++;
                } else if (pid == R_CANNON) {
                    blackAttackLiveCannonSum++;
                } else if (pid == R_KNIGHT) {
                    blackAttackLiveKnightSum++;
                } else if (pid == R_PAWN) {
                    blackAttackLivePawnSum++;
                }
            }
        }
    }
    // 红
    vlRedAttack = redAttackLiveRookSum * 2;
    vlRedAttack += redAttackLiveKnightSum * 2;
    vlRedAttack += redAttackLiveCannonSum;
    vlRedAttack += redAttackLivePawnSum;
    // 黑
    vlBlackAttack = blackAttackLiveRookSum * 2;
    vlBlackAttack += blackAttackLiveKnightSum * 2;
    vlBlackAttack += blackAttackLiveCannonSum;
    vlBlackAttack += blackAttackLivePawnSum;
    // 如果本方轻子数比对方多, 那么每多一个轻子(车算2个轻子)威胁值加2。威胁值最多不超过8
    int redSimpleValues = 0;
    int blackSimpleValues = 0;
    // 红
    redSimpleValues += redAttackLiveRookSum * 2;
    redSimpleValues += redAttackLiveKnightSum;
    redSimpleValues += redAttackLiveCannonSum;
    redSimpleValues += redAttackLivePawnSum;
    // 黑
    blackSimpleValues += blackAttackLiveRookSum * 2;
    blackSimpleValues += blackAttackLiveKnightSum;
    blackSimpleValues += blackAttackLiveCannonSum;
    blackSimpleValues += blackAttackLivePawnSum;
    // 设置
    if (redSimpleValues > blackSimpleValues) {
        vlRedAttack += (redSimpleValues - blackSimpleValues) * 2;
    } else if (redSimpleValues < blackSimpleValues) {
        vlBlackAttack += (blackSimpleValues - redSimpleValues) * 2;
    }
    vlRedAttack = std::min<int>(vlRedAttack, TOTAL_ATTACK_VALUE);
    vlBlackAttack = std::min<int>(vlBlackAttack, TOTAL_ATTACK_VALUE);
}
