#pragma once
#include "evaluate.hpp"
#include "bitboard.hpp"
#include "hash.hpp"

class Board
{
public:
    Board(PIECEID_MAP pieceidMap, TEAM initTeam);

    Piece pieceIndex(PIECE_INDEX pieceIndex) const;

    Piece piecePosition(int x, int y) const;

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

    int evaluate() const;

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

    UINT32 getBitLineX(int x) const
    {
        return this->bitboard->getBitlineX(x);
    }

    UINT32 getBitLineY(int y) const
    {
        return this->bitboard->getBitlineY(y);
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

    void print() const
    {
        const PIECEID_MAP &pieceidMap = this->pieceidMap;
        for (int i = -1; i <= 8; i++)
        {
            for (int j = -1; j <= 9; j++)
            {
                if (i == -1)
                {
                    if (j == -1)
                    {
                        std::clog << "X ";
                    }
                    else
                    {
                        std::clog << j << " ";
                    }
                }
                else
                {
                    if (j == -1)
                    {
                        std::clog << i << " ";
                    }
                    else
                    {
                        std::clog << PIECE_NAME_PAIRS.at(pieceidMap[i][j]);
                    }
                }
            }
            std::clog << "\n";
        }
        std::clog << std::endl;
    }

    MOVES historyMoves{};
    std::vector<PIECEID_MAP> historySituations{};
    TEAM team = -1;
    std::unique_ptr<Bitboard> bitboard{nullptr};
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
            const Piece &piece = this->pieceIndex(pieceindex);
            if (piece.isLive)
            {
                result.emplace_back(piece);
            }
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

    this->bitboard = std::make_unique<Bitboard>(this->pieceidMap);
}

Piece Board::pieceIndex(PIECE_INDEX pieceIndex) const
{
    return this->pieces[pieceIndex];
}

Piece Board::piecePosition(int x, int y) const
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
        if (piece.isLive)
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
        if (piece.team == team)
        {
            result.emplace_back(piece);
        }
    }

    return result;
}

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
    if (attackStarter.team == RED)
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
    this->historyMoves.back().starter = attackStarter;
    this->historyMoves.back().captured = eaten;
    this->historySituations.emplace_back(this->pieceidMap);
}

void Board::undoMove()
{
    const int x1 = this->historyMoves.back().x1;
    const int x2 = this->historyMoves.back().x2;
    const int y1 = this->historyMoves.back().y1;
    const int y2 = this->historyMoves.back().y2;
    const Piece eaten = this->historyMoves.back().captured;
    const Piece attackStarter = this->historyMoves.back().starter;

    // 更新棋盘数据
    this->distance -= 1;
    this->team = -this->team;
    this->historyMoves.pop_back();
    this->historySituations.pop_back();
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
    if (attackStarter.team == RED)
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

int Board::evaluate() const
{
    return this->team == RED ? (vlRed - vlBlack + vlAdvanced) : (vlBlack - vlRed + vlAdvanced);
}
