#pragma once
#include "evaluate.hpp"
#include "basicboard.hpp"
#include "hash.hpp"

class Board : public BasicBoard
{
public:
    Board(PIECEID_MAP pieceidMap, TEAM initTeam);

public:
    int distance = 0;
    int vlRed = 0;
    int vlBlack = 0;
    int32 hashKey = 0;
    int32 hashLock = 0;
    std::vector<int32> hashKeyList{};
    std::vector<int32> hashLockList{};
    
public:
    bool isKingLive(TEAM team) const { return team == RED ? getPieceReg(R_KING).isLive : getPieceReg(B_KING).isLive; }
    int evaluate() const { return team == RED ? vlRed - vlBlack + vlAdvanced : vlBlack - vlRed + vlAdvanced; };
    void doNullMove() { team = -team; }
    void undoNullMove() { team = -team; }
    bool nullOkay() const { return team == RED ? vlRed : vlBlack > 10000 + 600; }
    bool nullSafe() const { return team == RED ? vlRed : vlBlack > 10000 + 1200; }

public:
    void doMove(Move move);
    void undoMove();
    void initEvaluate();
    void vlOpenCalculator(int &vlOpen);
    void vlAttackCalculator(int &vlRedAttack, int &vlBlackAttack);
    void initHashInfo();
    void getMirrorHashinfo(int32 &mirrorHashKey, int32 &mirrorHashLock);
};

Board::Board(PIECEID_MAP pieceidMap, TEAM team) : BasicBoard(pieceidMap, team)
{
    initEvaluate();
    initHashInfo();
}

void Board::doMove(Move move)
{
    const int& x1 = move.x1;
    const int& x2 = move.x2;
    const int& y1 = move.y1;
    const int& y2 = move.y2;
    const Piece& attacker = this->piecePosition(x1, y1);
    const Piece& captured = this->piecePosition(x2, y2);

    // 更新棋盘数据
    this->team = -this->team;
    this->distance += 1;
    this->historyMoves.emplace_back(Move{x1, y1, x2, y2});
    this->historyMoves.back().attacker = attacker;
    this->historyMoves.back().captured = captured;
    this->bitboard->doMove(x1, y1, x2, y2);
    this->pieceidMap[x2][y2] = this->pieceidMap[x1][y1];
    this->pieceidMap[x1][y1] = 0;
    this->pieceIndexMap[x2][y2] = this->pieceIndexMap[x1][y1];
    this->pieceIndexMap[x1][y1] = -1;
    this->pieces[attacker.pieceIndex].x = x2;
    this->pieces[attacker.pieceIndex].y = y2;
    if (captured.pieceIndex != -1)
    {
        this->pieces[captured.pieceIndex].isLive = false;
    }
    // 更新评估分
    if (attacker.team == RED)
    {
        int valNewPos = pieceWeights[attacker.pieceid][x2][y2];
        int valOldPos = pieceWeights[attacker.pieceid][x1][y1];
        this->vlRed += (valNewPos - valOldPos);
        if (captured.pieceid != EMPTY_PIECEID)
        {
            this->vlBlack -= pieceWeights[captured.pieceid][x2][size_t(9) - y2];
        }
    }
    else
    {
        int valNewPos = pieceWeights[attacker.pieceid][x2][size_t(9) - y2];
        int valOldPos = pieceWeights[attacker.pieceid][x1][size_t(9) - y1];
        this->vlBlack += (valNewPos - valOldPos);
        if (captured.pieceid != EMPTY_PIECEID)
        {
            this->vlRed -= pieceWeights[captured.pieceid][x2][y2];
        }
    }
    // 记录旧哈希值
    this->hashKeyList.emplace_back(this->hashKey);
    this->hashLockList.emplace_back(this->hashLock);
    // 更新哈希值
    this->hashKey ^= hashKeys[attacker.pieceid][x1][y1];
    this->hashKey ^= hashKeys[attacker.pieceid][x2][y2];
    this->hashLock ^= hashLocks[attacker.pieceid][x1][y1];
    this->hashLock ^= hashLocks[attacker.pieceid][x2][y2];
    if (captured.pieceid != EMPTY_PIECEID)
    {
        this->hashKey ^= hashKeys[captured.pieceid][x1][y1];
        this->hashLock ^= hashLocks[captured.pieceid][x2][y2];
    }
    this->hashKey ^= PLAYER_KEY;
    this->hashLock ^= PLAYER_LOCK;
}

void Board::undoMove()
{
    const Move& back = this->historyMoves.back();
    const int& x1 = back.x1;
    const int& x2 = back.x2;
    const int& y1 = back.y1;
    const int& y2 = back.y2;
    const Piece& attacker = this->historyMoves.back().attacker;
    const Piece& captured = this->historyMoves.back().captured;

    // 更新棋盘数据
    this->distance -= 1;
    this->team = -this->team;
    this->historyMoves.pop_back();
    this->bitboard->undoMove(x1, y1, x2, y2, captured.pieceid != 0);
    this->pieceidMap[x1][y1] = this->pieceidMap[x2][y2];
    this->pieceidMap[x2][y2] = captured.pieceid;
    this->pieceIndexMap[x1][y1] = this->pieceIndexMap[x2][y2];
    this->pieceIndexMap[x2][y2] = captured.pieceIndex;
    this->pieces[attacker.pieceIndex].x = x1;
    this->pieces[attacker.pieceIndex].y = y1;
    if (captured.pieceIndex != -1)
    {
        this->pieces[captured.pieceIndex].isLive = true;
    }
    // 更新评估分
    if (attacker.team == RED)
    {
        int valPos1 = pieceWeights[attacker.pieceid][x1][y1];
        int valPos2 = pieceWeights[attacker.pieceid][x2][y2];
        this->vlRed -= (valPos2 - valPos1);
        if (captured.pieceid != EMPTY_PIECEID)
        {
            this->vlBlack += pieceWeights[captured.pieceid][x2][size_t(9) - y2];
        }
    }
    else
    {
        int valPos1 = pieceWeights[attacker.pieceid][x1][size_t(9) - y1];
        int valPos2 = pieceWeights[attacker.pieceid][x2][size_t(9) - y2];
        this->vlBlack -= (valPos2 - valPos1);
        if (captured.pieceid != EMPTY_PIECEID)
        {
            this->vlRed += pieceWeights[captured.pieceid][x2][y2];
        }
    }
    // 回滚哈希值
    this->hashKey = this->hashKeyList.back();
    this->hashLock = this->hashLockList.back();
    this->hashKeyList.pop_back();
    this->hashLockList.pop_back();
}

void Board::initEvaluate()
{
    // 更新权重数组
    int vlOpen = 0;
    int vlRedAttack = 0;
    int vlBlackAttack = 0;
    this->vlOpenCalculator(vlOpen);
    this->vlAttackCalculator(vlRedAttack, vlBlackAttack);

    pieceWeights = getBasicEvaluateWeights(vlOpen, vlRedAttack, vlBlackAttack);
    vlAdvanced = (TOTAL_ADVANCED_VALUE * vlOpen + TOTAL_ADVANCED_VALUE / 2) / TOTAL_MIDGAME_VALUE;
    vlPawn = (vlOpen * OPEN_PAWN_VAL + (TOTAL_MIDGAME_VALUE - vlOpen) * END_PAWN_VAL) / TOTAL_MIDGAME_VALUE;

    const int BOTTOM_CANNON_REWARD = (vlOpen * INITIAL_BOTTOM_CANNON_REWARD + (TOTAL_MIDGAME_VALUE - vlOpen) * TERMINAL_BOTTOM_CANNON_REWARD) / TOTAL_MIDGAME_VALUE;
    const int CENTER_CANNON_REWARD = (vlOpen * INITIAL_CENTER_CANNON_REWARD + (TOTAL_MIDGAME_VALUE - vlOpen) * TERMINAL_CENTER_CANNON_REWARD) / TOTAL_MIDGAME_VALUE;

    // 底炮
    const std::vector<int> bottomCannonXList = {0, 1, 7, 8};
    for (auto x : bottomCannonXList)
    {
        pieceWeights[R_CANNON][x][9] += (x == 0 || x == 8) ? BOTTOM_CANNON_REWARD : BOTTOM_CANNON_REWARD / 2;
        pieceWeights[B_CANNON][x][9] += (x == 0 || x == 8) ? BOTTOM_CANNON_REWARD : BOTTOM_CANNON_REWARD / 2;
    }

    // 中炮
    const std::vector<int> centerCannonYList = {2, 4, 5, 6};
    for (auto y : centerCannonYList)
    {
        pieceWeights[R_CANNON][4][y] += CENTER_CANNON_REWARD / (y - 1);
        pieceWeights[B_CANNON][4][y] += CENTER_CANNON_REWARD / (y - 1);
    }

    // 调整不受威胁方少掉的士象分
    this->vlRed = ADVISOR_BISHOP_ATTACKLESS_VALUE * (TOTAL_ATTACK_VALUE - vlBlackAttack) / TOTAL_ATTACK_VALUE;
    this->vlBlack = ADVISOR_BISHOP_ATTACKLESS_VALUE * (TOTAL_ATTACK_VALUE - vlRedAttack) / TOTAL_ATTACK_VALUE;

    // 进一步重新计算分数
    for (int x = 0; x < 9; x++)
    {
        for (int y = 0; y < 10; y++)
        {
            PIECEID pid = this->pieceidMap[x][y];
            if (pid > 0)
            {
                this->vlRed += pieceWeights[pid][x][y];
            }
            else if (pid < 0)
            {
                this->vlBlack += pieceWeights[pid][x][size_t(9) - y];
            }
        }
    }
}

void Board::vlOpenCalculator(int &vlOpen)
{
    // 首先判断局势处于开中局还是残局阶段, 方法是计算各种棋子的数量, 按照车=6、马炮=3、其它=1相加
    int rookLiveSum = 0;
    int knightCannonLiveSum = 0;
    int otherLiveSum = 0;
    for (const Piece &piece : this->getAllLivePieces())
    {
        PIECEID pid = std::abs(piece.pieceid);
        if (pid == R_ROOK)
        {
            rookLiveSum++;
        }
        else if (pid == R_KNIGHT || pid == R_CANNON)
        {
            knightCannonLiveSum++;
        }
        else if (pid != R_KING)
        {
            otherLiveSum++;
        }
    }
    vlOpen = rookLiveSum * 6 + knightCannonLiveSum * 3 + otherLiveSum;
    // 使用二次函数, 子力很少时才认为接近残局
    vlOpen = (2 * TOTAL_MIDGAME_VALUE - vlOpen) * vlOpen;
    vlOpen /= TOTAL_MIDGAME_VALUE;
}

void Board::vlAttackCalculator(int &vlRedAttack, int &vlBlackAttack)
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
    for (const Piece &piece : this->getAllLivePieces())
    {
        PIECEID pid = std::abs(piece.pieceid);
        if (piece.team == RED)
        {
            if (piece.y >= 5)
            {
                if (pid == R_ROOK)
                {
                    redAttackLiveRookSum++;
                }
                else if (pid == R_CANNON)
                {
                    redAttackLiveCannonSum++;
                }
                else if (pid == R_KNIGHT)
                {
                    redAttackLiveKnightSum++;
                }
                else if (pid == R_PAWN)
                {
                    redAttackLivePawnSum++;
                }
            }
        }
        else if (piece.team == BLACK)
        {
            if (piece.y <= 4)
            {
                if (pid == R_ROOK)
                {
                    blackAttackLiveRookSum++;
                }
                else if (pid == R_CANNON)
                {
                    blackAttackLiveCannonSum++;
                }
                else if (pid == R_KNIGHT)
                {
                    blackAttackLiveKnightSum++;
                }
                else if (pid == R_PAWN)
                {
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
    if (redSimpleValues > blackSimpleValues)
    {
        vlRedAttack += (redSimpleValues - blackSimpleValues) * 2;
    }
    else if (redSimpleValues < blackSimpleValues)
    {
        vlBlackAttack += (blackSimpleValues - redSimpleValues) * 2;
    }
    vlRedAttack = std::min<int>(vlRedAttack, TOTAL_ATTACK_VALUE);
    vlBlackAttack = std::min<int>(vlBlackAttack, TOTAL_ATTACK_VALUE);
}

void Board::initHashInfo()
{
    this->hashKey = 0;
    this->hashLock = 0;
    for (int x = 0; x < 9; x++)
    {
        for (int y = 0; y < 10; y++)
        {
            PIECEID pid = this->pieceidMap[x][y];
            if (pid != EMPTY_PIECEID)
            {
                this->hashKey ^= hashKeys[pid][x][y];
                this->hashLock ^= hashLocks[pid][x][y];
            }
        }
    }
    if (this->team == BLACK)
    {
        this->hashKey ^= PLAYER_KEY;
        this->hashLock ^= PLAYER_LOCK;
    }
}

void Board::getMirrorHashinfo(int32 &mirrorHashKey, int32 &mirrorHashLock)
{
    mirrorHashKey = 0;
    mirrorHashLock = 0;
    for (int x = 0; x < 9; x++)
    {
        for (int y = 0; y < 10; y++)
        {
            PIECEID pid = this->pieceidMap[x][y];
            if (pid != EMPTY_PIECEID)
            {
                mirrorHashKey ^= hashKeys[pid][size_t(8) - x][y];
                mirrorHashLock ^= hashLocks[pid][size_t(8) - x][y];
            }
        }
    }
    if (this->team == BLACK)
    {
        mirrorHashKey ^= PLAYER_KEY;
        mirrorHashLock ^= PLAYER_LOCK;
    }
}
