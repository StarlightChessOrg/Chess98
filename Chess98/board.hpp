#pragma once

#include "bitboard.hpp"
#include "evaluate.hpp"
#include "hash.hpp"

class Board
{
public:
    Board(PIECEID_MAP pieceidMap, TEAM initTeam);

    Piece pieceIndex(PIECE_INDEX pieceIndex);

    Piece piecePosition(int x, int y);

    PIECEID pieceidOn(int x, int y) const;

    TEAM teamOn(int x, int y) const;

    PIECES getAllLivePieces();

    PIECES getPiecesByTeam(TEAM team);

    void doMove(Move move);

    void undoMove();

    void initEvaluate();

    void vlOpenCalculator(int &vlOpen);

    void vlAttackCalculator(int &vlRedAttack, int &vlBlackAttack);

    void initHashInfo();

    void getMirrorHashinfo(int32 &mirrorHashKey, int32 &mirrorHashLock);

    bool isKingLive(TEAM team) const
    {
        return team == RED ? this->isRedKingLive : this->isBlackKingLive;
    }

    int evaluate(int vlAlpha, int vlBeta);

    int rookMobility();

    int knightMobility();
    
    int bottomCannon();

    int centerCannon();

    void doNullMove()
    {
        this->team = -this->team;
    }

    void undoNullMove()
    {
        this->team = -this->team;
    }

    bool nullOkay() const
    {
        const int vlSelf = this->team == RED ? this->vlRed : this->vlBlack;
        return (vlSelf > 10000 + 600);
    }

    bool nullSafe() const
    {
        const int vlSelf = this->team == RED ? this->vlRed : this->vlBlack;
        return (vlSelf > 10000 + 1200);
    }

    BITLINE getBitLineX(int x) const
    {
        return this->bitboard->xBitBoard[x];
    }

    BITLINE getBitLineY(int y) const
    {
        return this->bitboard->yBitBoard[y];
    }

    PIECES getLivePiecesById(PIECEID pieceid)
    {
        PIECES result{};
        for (const PIECE_INDEX pieceindex : this->pieceRegistry[pieceid])
        {
            Piece piece = this->pieceIndex(pieceindex);
            if (piece.isLive)
            {
                result.emplace_back(piece);
            }
        }
        return result;
    }

    MOVES historyMoves{};
    TEAM team = -1;
    BitBoard *bitboard = nullptr;
    int distance = 0;
    int vlRed = 0;
    int vlBlack = 0;
    int32 hashKey = 0;
    int32 hashLock = 0;
    std::map<PIECEID, std::vector<PIECE_INDEX>> pieceRegistry{
        {R_KING, {}},
        {R_GUARD, {}},
        {R_BISHOP, {}},
        {R_ROOK, {}},
        {R_KNIGHT, {}},
        {R_CANNON, {}},
        {R_PAWN, {}},
        {B_KING, {}},
        {B_GUARD, {}},
        {B_BISHOP, {}},
        {B_ROOK, {}},
        {B_KNIGHT, {}},
        {B_CANNON, {}},
        {B_PAWN, {}}};

    Piece getPieceFromRegistry(PIECEID pieceid, int index)
    {
        return this->pieceIndex(this->pieceRegistry.at(pieceid)[index]);
    }

    PIECES getPiecesFromRegistry(PIECEID pieceid)
    {
        PIECES result{};
        for (PIECE_INDEX pieceindex : this->pieceRegistry[pieceid])
        {
            result.emplace_back(this->pieceIndex(pieceindex));
        }
        return result;
    }

    std::array<std::array<int, 10>, 9> pieceIndexMap{};
    PIECES pieces{};
    std::vector<PIECE_INDEX> redPieces{};
    std::vector<PIECE_INDEX> blackPieces{};
    bool isRedKingLive = false;
    bool isBlackKingLive = false;
    PIECEID_MAP pieceidMap{};
    std::vector<int32> hashKeyList{};
    std::vector<int32> hashLockList{};
};

Board::Board(PIECEID_MAP pieceidMap, TEAM initTeam)
{
    this->distance = 0;
    this->team = initTeam;
    this->pieceidMap = pieceidMap;
    for (int x = 0; x < 9; x++)
    {
        for (int y = 0; y < 10; y++)
        {
            PIECEID pieceid = this->pieceidMap[x][y];
            if (pieceid != 0)
            {
                Piece piece{this->pieceidMap[x][y], x, y, (int)this->pieces.size()};
                this->pieces.emplace_back(piece);
                PIECE_INDEX index = int(this->pieces.size()) - 1;
                this->pieceIndexMap[x][y] = index;
                if (pieceid > 0)
                    this->redPieces.emplace_back(index);
                else
                    this->blackPieces.emplace_back(index);
                if (pieceid == R_KING)
                    this->isRedKingLive = true;
                if (pieceid == B_KING)
                    this->isBlackKingLive = true;
                this->pieceRegistry[pieceid].emplace_back(this->pieces.back().pieceIndex);
            }
            else
            {
                this->pieceIndexMap[x][y] = -1;
            }
        }
    }
    // 初始化评估分
    initEvaluate();
    // 初始化局面哈希
    initHashInfo();

    this->bitboard = new BitBoard{this->pieceidMap};
}

Piece Board::pieceIndex(PIECE_INDEX pieceIndex)
{
    return this->pieces[pieceIndex];
}

Piece Board::piecePosition(int x, int y)
{
    if (x >= 0 && x <= 8 && y >= 0 && y <= 9)
    {
        PIECEID pieceid = this->pieceidMap[x][y];
        if (pieceid != 0)
        {
            PIECE_INDEX pieceIndex = this->pieceIndexMap[x][y];
            return this->pieceIndex(pieceIndex);
        }
        else
        {
            return Piece{EMPTY_PIECEID, -1, -1, EMPTY_INDEX};
        }
    }
    else
    {
        return Piece{OVERFLOW_PIECEID, -1, -1, EMPTY_INDEX};
    }
}

PIECEID Board::pieceidOn(int x, int y) const
{
    if (x >= 0 && x <= 8 && y >= 0 && y <= 9)
    {
        return this->pieceidMap[x][y];
    }
    else
    {
        return OVERFLOW_PIECEID;
    }
}

TEAM Board::teamOn(int x, int y) const
{
    if (x >= 0 && x <= 8 && y >= 0 && y <= 9)
    {
        PIECEID pieceid = this->pieceidMap[x][y];
        if (pieceid > 0)
        {
            return RED;
        }
        else if (pieceid < 0)
        {
            return BLACK;
        }
        else
        {
            return EMPTY_TEAM;
        }
    }
    else
    {
        return OVERFLOW_TEAM;
    }
}

PIECES Board::getAllLivePieces()
{
    PIECES result{};
    for (Piece piece : this->pieces)
    {
        if (piece.isLive == true)
        {
            result.emplace_back(piece);
        }
    }
    return result;
}

PIECES Board::getPiecesByTeam(TEAM team)
{
    PIECES result{};
    PIECES allPieces = this->getAllLivePieces();
    for (Piece piece : allPieces)
    {
        if (piece.team() == team)
        {
            result.emplace_back(piece);
        }
    }

    return result;
}

/// @brief 步进
/// @param move
void Board::doMove(Move move)
{
    const int x1 = move.x1;
    const int x2 = move.x2;
    const int y1 = move.y1;
    const int y2 = move.y2;
    const Piece eaten = this->piecePosition(x2, y2);
    const Piece attackStarter = this->piecePosition(x1, y1);

    // 维护棋盘的棋子追踪
    this->pieceidMap[x2][y2] = this->pieceidMap[x1][y1];
    this->pieceidMap[x1][y1] = 0;
    this->pieceIndexMap[x2][y2] = this->pieceIndexMap[x1][y1];
    this->pieceIndexMap[x1][y1] = -1;
    this->pieces[attackStarter.pieceIndex].x = x2;
    this->pieces[attackStarter.pieceIndex].y = y2;
    if (eaten.pieceIndex != -1)
    {
        this->pieces[eaten.pieceIndex].isLive = false;
    }
    if (eaten.pieceid == R_KING)
    {
        this->isRedKingLive = false;
    }
    if (eaten.pieceid == B_KING)
    {
        this->isBlackKingLive = false;
    }
    this->bitboard->doMove(x1, y1, x2, y2);
    // 更新评估分
    if (attackStarter.team() == RED)
    {
        int valNewPos = pieceWeights[attackStarter.pieceid][x2][y2];
        int valOldPos = pieceWeights[attackStarter.pieceid][x1][y1];
        this->vlRed += (valNewPos - valOldPos);
        if (eaten.pieceid != EMPTY_PIECEID)
        {
            this->vlBlack -= pieceWeights[eaten.pieceid][x2][size_t(9) - y2];
        }
    }
    else
    {
        int valNewPos = pieceWeights[attackStarter.pieceid][x2][size_t(9) - y2];
        int valOldPos = pieceWeights[attackStarter.pieceid][x1][size_t(9) - y1];
        this->vlBlack += (valNewPos - valOldPos);
        if (eaten.pieceid != EMPTY_PIECEID)
        {
            this->vlRed -= pieceWeights[eaten.pieceid][x2][y2];
        }
    }
    // 记录旧哈希值
    this->hashKeyList.emplace_back(this->hashKey);
    this->hashLockList.emplace_back(this->hashLock);
    // 更新哈希值
    this->hashKey ^= hashKeys[attackStarter.pieceid][x1][y1];
    this->hashKey ^= hashKeys[attackStarter.pieceid][x2][y2];
    this->hashLock ^= hashLocks[attackStarter.pieceid][x1][y1];
    this->hashLock ^= hashLocks[attackStarter.pieceid][x2][y2];
    if (eaten.pieceid != EMPTY_PIECEID)
    {
        this->hashKey ^= hashKeys[eaten.pieceid][x1][y1];
        this->hashLock ^= hashLocks[eaten.pieceid][x2][y2];
    }
    this->hashKey ^= PLAYER_KEY;
    this->hashLock ^= PLAYER_LOCK;
    // 更新棋盘数据
    this->team = -this->team;
    this->distance += 1;
    this->historyMoves.emplace_back(Move{x1, y1, x2, y2});
    this->historyMoves.back().attacker = attackStarter;
    this->historyMoves.back().captured = eaten;
}

/// @brief 撤销上一次步进
void Board::undoMove()
{
    const int x1 = this->historyMoves.back().x1;
    const int x2 = this->historyMoves.back().x2;
    const int y1 = this->historyMoves.back().y1;
    const int y2 = this->historyMoves.back().y2;
    const Piece eaten = this->historyMoves.back().captured;
    const Piece attackStarter = this->historyMoves.back().attacker;

    // 更新棋盘数据
    this->distance -= 1;
    this->team = -this->team;
    this->historyMoves.pop_back();
    this->bitboard->undoMove(x1, y1, x2, y2, eaten.pieceid != 0);
    // 维护棋盘的棋子追踪
    this->pieceidMap[x1][y1] = this->pieceidMap[x2][y2];
    this->pieceidMap[x2][y2] = eaten.pieceid;
    this->pieceIndexMap[x1][y1] = this->pieceIndexMap[x2][y2];
    this->pieceIndexMap[x2][y2] = eaten.pieceIndex;
    this->pieces[attackStarter.pieceIndex].x = x1;
    this->pieces[attackStarter.pieceIndex].y = y1;
    if (eaten.pieceIndex != -1)
    {
        this->pieces[eaten.pieceIndex].isLive = true;
    }
    if (eaten.pieceid == R_KING)
    {
        this->isRedKingLive = true;
    }
    if (eaten.pieceid == B_KING)
    {
        this->isBlackKingLive = true;
    }
    // 更新评估分
    if (attackStarter.team() == RED)
    {
        int valPos1 = pieceWeights[attackStarter.pieceid][x1][y1];
        int valPos2 = pieceWeights[attackStarter.pieceid][x2][y2];
        this->vlRed -= (valPos2 - valPos1);
        if (eaten.pieceid != EMPTY_PIECEID)
        {
            this->vlBlack += pieceWeights[eaten.pieceid][x2][size_t(9) - y2];
        }
    }
    else
    {
        int valPos1 = pieceWeights[attackStarter.pieceid][x1][size_t(9) - y1];
        int valPos2 = pieceWeights[attackStarter.pieceid][x2][size_t(9) - y2];
        this->vlBlack -= (valPos2 - valPos1);
        if (eaten.pieceid != EMPTY_PIECEID)
        {
            this->vlRed += pieceWeights[eaten.pieceid][x2][y2];
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
    // 首先判断局势处于开中局还是残局阶段，方法是计算各种棋子的数量，按照车=6、马炮=3、其它=1相加
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
    // 使用二次函数，子力很少时才认为接近残局
    vlOpen = (2 * TOTAL_MIDGAME_VALUE - vlOpen) * vlOpen;
    vlOpen /= TOTAL_MIDGAME_VALUE;
}

void Board::vlAttackCalculator(int &vlRedAttack, int &vlBlackAttack)
{
    // 然后判断各方是否处于进攻状态，方法是计算各种过河棋子的数量，按照车马2炮兵1相加
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
        if (piece.team() == RED)
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
        else if (piece.team() == BLACK)
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
    // 如果本方轻子数比对方多，那么每多一个轻子(车算2个轻子)威胁值加2。威胁值最多不超过8
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

/// @brief 车的机动性
/// @return
int Board::rookMobility()
{
    int result = 0;
    for (const Piece &rook : this->getLivePiecesById(this->team * R_ROOK))
    {
        const int x = rook.x;
        const int y = rook.y;
        BITLINE bitlineX = this->getBitLineX(x);
        REGION_ROOK regionX = this->bitboard->getRookRegion(bitlineX, y, 9);
        BITLINE bitlineY = this->getBitLineY(y);
        REGION_ROOK regionY = this->bitboard->getRookRegion(bitlineY, x, 8);
        result += ROOK_EXTEND * (regionY[1] - regionY[0] + regionX[1] - regionX[0] - 2);
        if (this->teamOn(x, regionX[1]) != this->team)
        {
            result += ROOK_EXTEND;
        }
        if (this->teamOn(x, regionX[0]) != this->team)
        {
            result += ROOK_EXTEND;
        }
        if (this->teamOn(regionY[1], y) != this->team)
        {
            result += ROOK_EXTEND;
        }
        if (this->teamOn(regionY[1], y) != this->team)
        {
            result += ROOK_EXTEND;
        }
    }
    return (result / ROOK_EXTEND_FACTOR);
}

/// @brief 马的灵活性
/// @return
int Board::knightMobility()
{
    const std::array<std::array<int, 10>, 9> badKnightPosMap{
        {{{1, 1, 1, 1, 1, 1, 1, 1, 1, 1}},
         {{1, 0, 0, 0, 0, 0, 0, 0, 0, 1}},
         {{1, 0, 0, 0, 0, 0, 0, 0, 0, 1}},
         {{1, 0, 0, 0, 0, 0, 0, 0, 0, 1}},
         {{1, 1, 0, 0, 0, 0, 0, 0, 1, 1}},
         {{1, 0, 0, 0, 0, 0, 0, 0, 0, 1}},
         {{1, 0, 0, 0, 0, 0, 0, 0, 0, 1}},
         {{1, 0, 0, 0, 0, 0, 0, 0, 0, 1}},
         {{1, 1, 1, 1, 1, 1, 1, 1, 1, 1}}}};
    const std::array<std::array<int, 2>, 8> KnightMoves{
        {{-1, -2}, {1, -2}, {-2, -1}, {2, -1}, {-2, 1}, {2, 1}, {-1, 2}, {1, 2}}};

    int goodTargetCnt = 0;

    for (const Piece &knight : this->getLivePiecesById(this->team * R_KNIGHT))
    {
        const int x = knight.x;
        const int y = knight.y;
        for (const std::array<int, 2> &move : KnightMoves)
        {
            const int midX = x + move[0] / 2;
            const int midY = y + move[1] / 2;
            const int targetX = x + move[0];
            const int targetY = y + move[1];
            if (this->pieceidOn(midX, midY) != EMPTY_PIECEID)
            {
                continue;
            }
            const int teamOnTarget = this->teamOn(targetX, targetY);
            if (teamOnTarget == this->team || teamOnTarget == OVERFLOW_TEAM)
            {
                continue;
            }
            if (badKnightPosMap[size_t(targetX)][size_t(targetY)] == 1)
            {
                continue;
            }
            goodTargetCnt++;
            if (goodTargetCnt >= KNIGHT_GOOD_TARGET_SUM)
            {
                break;
            }
        }
        if (goodTargetCnt >= KNIGHT_GOOD_TARGET_SUM)
        {
            break;
        }
    }
    return goodTargetCnt * KNIGHT_EXTEND;
}

/// @brief 沉底炮威胁
/// @return
int Board::bottomCannon()
{
    int result = 0;
    PIECES enemyCannons = this->getLivePiecesById(-this->team * R_CANNON);
    const int threatPosSelf = this->team == RED ? 0 : 9;
    // 对面的炮
    for (const Piece &piece : enemyCannons)
    {
        // 是否沉底
        if (piece.y == threatPosSelf)
        {
            result -= 30;
            // 看相的位置
            for (const Piece &bishop : this->getLivePiecesById(this->team * R_BISHOP))
            {
                if ((piece.x < 4 && bishop.x < 4) || (piece.y > 4 && bishop.y > 4))
                {
                    result += 15;
                }
            }
        }
    }
    return result;
}

/// @brief 当头炮威胁
/// @return
int Board::centerCannon()
{
    int result = 0;
    const Piece selfKing = this->getPieceFromRegistry(this->team * R_KING, 0);
    const PIECES enemyCannons = this->getPiecesFromRegistry(-this->team * R_KING);
    for (const Piece &cannon : enemyCannons)
    {
        if (cannon.x == 4)
        {
            int num = barrierNumber(this->pieceidMap, selfKing.x, selfKing.y, cannon.x, cannon.y);
            if (num == 0)
            {
                result = -20 * abs(selfKing.y - cannon.y);
            }
            else if (num == 2)
            {
                result = -30;
            }
        }
    }
    return result;
}

int Board::evaluate(int vlAlpha, int vlBeta)
{
    // Level 1
    int vlEvaluate = this->team == RED ? (vlRed - vlBlack + vlAdvanced) : (vlBlack - vlRed + vlAdvanced);
    if (vlEvaluate <= vlAlpha - LAZY_MARGIN_1)
    {
        return vlAlpha - LAZY_MARGIN_1;
    }
    else if (vlEvaluate >= vlBeta + LAZY_MARGIN_1)
    {
        return vlBeta + LAZY_MARGIN_1;
    }
    // Level 2
    vlEvaluate += this->team == RED ? rookMobility() : -rookMobility();
    if (vlEvaluate <= vlAlpha - LAZY_MARGIN_2)
    {
        return vlAlpha - LAZY_MARGIN_2;
    }
    else if (vlEvaluate >= vlBeta + LAZY_MARGIN_2)
    {
        return vlBeta + LAZY_MARGIN_2;
    }
    // Level 3
    vlEvaluate += this->team == RED ? knightMobility() : -knightMobility();
    vlEvaluate += this->team == RED ? bottomCannon() : -bottomCannon();
    vlEvaluate += this->team == RED ? centerCannon() : -centerCannon();
    return vlEvaluate;
}
