#pragma once
#include "evaluate.hpp"
#include "hash.hpp"
#include "bitboard.hpp"

class Board
{
public:
    Board() = default;
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
    PIECEID_MAP pieceidMap{};
    MOVES historyMoves{};
    TEAM team{};
    std::unique_ptr<Bitboard> bitboard{};
    PIECES pieces{};
    std::vector<PIECE_INDEX> redPieces{};
    std::vector<PIECE_INDEX> blackPieces{};
    std::array<std::array<PIECE_INDEX, 10>, 9> pieceIndexMap{};
    std::map<PIECEID, std::vector<PIECE_INDEX>> pieceRegistry{};

public:
    bool isKingLive(TEAM team) const { return team == RED ? getPieceReg(R_KING).isLive : getPieceReg(B_KING).isLive; }
    int evaluate() const { return team == RED ? vlRed - vlBlack + vlAdvanced : vlBlack - vlRed + vlAdvanced; };
    void doNullMove() { team = -team; }
    void undoNullMove() { team = -team; }
    bool nullOkay() const { return team == RED ? vlRed : vlBlack > 10000 + 600; }
    bool nullSafe() const { return team == RED ? vlRed : vlBlack > 10000 + 1200; }
    UINT32 getBitLineX(int x) const { return bitboard->getBitlineX(x); }
    UINT32 getBitLineY(int y) const { return this->bitboard->getBitlineY(y); }

public:
    PIECEID pieceidOn(int x, int y) const;
    TEAM teamOn(int x, int y) const;
    Piece pieceIndex(int i) const;
    Piece piecePosition(int x, int y) const;
    PIECES getAllLivePieces() const;
    PIECES getPiecesByTeam(TEAM team) const;
    Piece getPieceReg(PIECEID pieceid) const;
    PIECES getPiecesReg(PIECEID pieceid) const;
    bool isRepeated() const; 
    bool hasCrossedRiver(int x, int y) const;
    bool isInPalace(int x, int y) const;
    bool inCheck(TEAM judgeTeam) const;
    bool hasProtector(int x, int y) const;

public:
    void doMove(Move move);
    void undoMove();
    void doMoveSimple(Move move);
    void undoMoveSimple();
    void initEvaluate();
    void vlOpenCalculator(int &vlOpen) const;
    void vlAttackCalculator(int &vlRedAttack, int &vlBlackAttack) const;
    void initHashInfo();
    void getMirrorHashinfo(int32 &mirrorHashKey, int32 &mirrorHashLock) const;
    bool isValidMoveInSituation(Move move);
};

Board::Board(PIECEID_MAP pieceidMap, TEAM team)
{
    this->pieceidMap = pieceidMap;
    this->team = team;
    this->bitboard = std::make_unique<Bitboard>(pieceidMap);
    for (const PIECEID& id : ALL_PIECEIDS)
    {
        this->pieceRegistry[id] = std::vector<PIECE_INDEX>{};
    }
    for (int x = 0; x < 9; x++)
    {
        for (int y = 0; y < 10; y++)
        {
            PIECEID &pieceid = pieceidMap[x][y];
            if (pieceid != 0)
            {
                const int &size = int(this->pieces.size());
                Piece piece{pieceidMap[x][y], x, y, size};
                PIECE_INDEX index = size;

                this->pieces.emplace_back(piece);
                this->pieceIndexMap[x][y] = index;
                this->pieceRegistry[pieceid].emplace_back(this->pieces.back().pieceIndex);
                if (pieceid > 0)
                {
                    this->redPieces.emplace_back(index);
                }
                else
                {
                    this->blackPieces.emplace_back(index);
                }
            }
            else
            {
                this->pieceIndexMap[x][y] = -1;
            }
        }
    }
    this->initEvaluate();
    this->initHashInfo();
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
        const PIECEID pieceid = this->pieceidMap[x][y];
        return pieceid > 0 ? RED : (pieceid < 0 ? BLACK : EMPTY_TEAM);
    }
    else
    {
        return OVERFLOW_TEAM;
    }
}

Piece Board::pieceIndex(int i) const
{
    return this->pieces[i];
}

Piece Board::piecePosition(int x, int y) const
{
    if (x >= 0 && x <= 8 && y >= 0 && y <= 9)
    {
        if (this->pieceidMap[x][y] != 0)
        {
            return this->pieceIndex(this->pieceIndexMap[x][y]);
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

PIECES Board::getAllLivePieces() const
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

PIECES Board::getPiecesByTeam(TEAM team) const
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

Piece Board::getPieceReg(PIECEID pieceid) const
{
    return this->pieceIndex(this->pieceRegistry.at(pieceid)[0]);
}

PIECES Board::getPiecesReg(PIECEID pieceid) const
{
    PIECES result{};
    for (PIECE_INDEX pieceindex : this->pieceRegistry.at(pieceid))
    {
        const Piece &piece = this->pieceIndex(pieceindex);
        if (piece.isLive)
        {
            result.emplace_back(piece);
        }
    }
    return result;
}

bool Board::isRepeated() const
{
    const MOVES& history = this->historyMoves;
    const size_t size = history.size();
    if (size >= 5)
    {
        const Move& ply1 = history[size_t(size - 1)];
        const Move& ply2 = history[size_t(size - 2)];
        const Move& ply3 = history[size_t(size - 3)];
        const Move& ply4 = history[size_t(size - 4)];
        const Move& ply5 = history[size_t(size - 5)];
        // 判断是否出现重复局面, 没有则直接false
        // 试想如下重复局面：（格式：plyX: x1y1x2y2）
        // ply1: 0001, ply2: 0908, ply3: 0100, ply4: 0809, ply5: 0001
        const bool isRepeat = (ply1 == ply5 &&
            ply1.startpos == ply3.endpos &&
            ply1.endpos == ply3.startpos &&
            ply2.startpos == ply4.endpos &&
            ply2.endpos == ply4.startpos);
        if (!isRepeat)
        {
            return false;
        }

        // 长将在任何情况下都会判负
        // 由于性能原因, isCheckingMove是被延迟设置的, ply1可能还没有被设成checkingMove
        // 但是若判定了循环局面, ply1必然等于ply5
        // 若ply5和ply3都是将军着法, 且出现循环局面, 则直接判定违规
        if (ply5.isCheckingMove == true && ply3.isCheckingMove == true)
        {
            return true;
        }
        // 长捉情况比较特殊
        // 只有车、马、炮能作为长捉的发起者
        // 发起者不断捉同一个子, 判负
        if (abs(ply1.attacker.pieceid) == R_ROOK ||
            abs(ply1.attacker.pieceid) == R_KNIGHT ||
            abs(ply1.attacker.pieceid) == R_CANNON)
        {
            const Piece& attacker = ply1.attacker;
            const Piece& captured = ply2.attacker;
            // 车
            if (abs(attacker.pieceid) == R_ROOK)
            {
                if (ply5.x2 == ply4.x1)
                {
                    UINT32 bitlineX = this->getBitLineX(ply5.x2);
                    REGION_ROOK regionX = this->bitboard->getRookRegion(bitlineX, attacker.y, 9);
                    if (this->piecePosition(ply5.x2, regionX[1]).pieceIndex == captured.pieceIndex ||
                        this->piecePosition(ply5.x2, regionX[0]).pieceIndex == captured.pieceIndex)
                    {
                        return true;
                    }
                }
                else if (ply5.y2 == ply4.y1)
                {
                    UINT32 bitlineY = this->getBitLineY(ply5.y2);
                    REGION_ROOK regionY = this->bitboard->getRookRegion(bitlineY, attacker.x, 8);
                    if (this->piecePosition(regionY[0], ply5.y2).pieceIndex == captured.pieceIndex ||
                        this->piecePosition(regionY[1], ply5.y2).pieceIndex == captured.pieceIndex)
                    {
                        return true;
                    }
                }
            }
            // 炮
            else if (abs(attacker.pieceid) == R_CANNON)
            {
                if (ply5.x2 == ply4.x1)
                {
                    UINT32 bitlineX = this->getBitLineX(ply5.x2);
                    REGION_CANNON regionX = this->bitboard->getCannonRegion(bitlineX, attacker.y, 9);
                    if (this->piecePosition(ply5.x2, regionX[1]).pieceIndex == captured.pieceIndex ||
                        this->piecePosition(ply5.x2, regionX[3]).pieceIndex == captured.pieceIndex)
                    {
                        return true;
                    }
                }
                else if (ply5.y2 == ply4.y1)
                {
                    UINT32 bitlineY = this->getBitLineY(ply5.y2);
                    REGION_CANNON regionY = this->bitboard->getCannonRegion(bitlineY, attacker.x, 8);
                    if (this->piecePosition(regionY[0], ply5.y2).pieceIndex == captured.pieceIndex ||
                        this->piecePosition(regionY[3], ply5.y2).pieceIndex == captured.pieceIndex)
                    {
                        return true;
                    }
                }
            }
            // 马
            else if (abs(attacker.pieceid) == R_KNIGHT)
            {
                if (
                    (attacker.x + 1 == captured.x && attacker.y + 2 == captured.y) ||
                    (attacker.x - 1 == captured.x && attacker.y + 2 == captured.y) ||
                    (attacker.x + 1 == captured.x && attacker.y - 2 == captured.y) ||
                    (attacker.x - 1 == captured.x && attacker.y - 2 == captured.y) ||
                    (attacker.x + 2 == captured.x && attacker.y + 1 == captured.y) ||
                    (attacker.x - 2 == captured.x && attacker.y + 1 == captured.y) ||
                    (attacker.x + 2 == captured.x && attacker.y - 1 == captured.y) ||
                    (attacker.x - 2 == captured.x && attacker.y - 1 == captured.y))
                {
                    return true;
                }
            }
        }
    }
    return false;
}

bool Board::hasCrossedRiver(int x, int y) const
{
    TEAM team = this->teamOn(x, y);
    if (team == RED)
    {
        return y >= 5 && y <= 9;
    }
    else if (team == BLACK)
    {
        return y >= 0 && y <= 4;
    }
    return false;
}

bool Board::isInPalace(int x, int y) const
{
    TEAM team = this->teamOn(x, y);
    if (team == RED)
    {
        return x >= 3 && x <= 5 && y >= 7 && y <= 9;
    }
    else if (team == BLACK)
    {
        return x >= 3 && x <= 5 && y >= 0 && y <= 2;
    }
    return false;
}

bool Board::inCheck(TEAM judgeTeam) const
{
    const Piece& king = judgeTeam == RED ? this->getPieceReg(R_KING) : this->getPieceReg(B_KING);
    const int& x = king.x;
    const int& y = king.y;
    const TEAM& team = king.team;

    // 兵
    const PIECEID ENEMY_PAWN = R_PAWN * -team;
    if (this->pieceidOn(x + 1, y) == ENEMY_PAWN)
    {
        return true;
    }
    if (this->pieceidOn(x - 1, y) == ENEMY_PAWN)
    {
        return true;
    }
    if (this->pieceidOn(x, (team == RED ? y - 1 : y + 1)) == ENEMY_PAWN)
    {
        return true;
    }

    // 马
    const PIECEID ENEMY_KNIGHT = R_KNIGHT * -team;
    if (this->pieceidOn(x + 1, y + 1) == EMPTY_PIECEID)
    {
        if (this->pieceidOn(x + 2, y + 1) == ENEMY_KNIGHT)
        {
            return true;
        }
        if (this->pieceidOn(x + 1, y + 2) == ENEMY_KNIGHT)
        {
            return true;
        }
    }
    if (this->pieceidOn(x - 1, y + 1) == EMPTY_PIECEID)
    {
        if (this->pieceidOn(x - 2, y + 1) == ENEMY_KNIGHT)
        {
            return true;
        }
        if (this->pieceidOn(x - 1, y + 2) == ENEMY_KNIGHT)
        {
            return true;
        }
    }
    if (this->pieceidOn(x + 1, y - 1) == EMPTY_PIECEID)
    {
        if (this->pieceidOn(x + 2, y - 1) == ENEMY_KNIGHT)
        {
            return true;
        }
        if (this->pieceidOn(x + 1, y - 2) == ENEMY_KNIGHT)
        {
            return true;
        }
    }
    if (this->pieceidOn(x - 1, y - 1) == EMPTY_PIECEID)
    {
        if (this->pieceidOn(x - 2, y - 1) == ENEMY_KNIGHT)
        {
            return true;
        }
        if (this->pieceidOn(x - 1, y - 2) == ENEMY_KNIGHT)
        {
            return true;
        }
    }

    // 将、车、炮
    const PIECEID ENEMY_ROOK = R_ROOK * -team;
    const PIECEID ENEMY_CANNON = R_CANNON * -team;
    const PIECEID ENEMY_KING = R_KING * -team;

    UINT32 bitlineY = this->getBitLineY(y);
    REGION_CANNON regionY = this->bitboard->getCannonRegion(bitlineY, x, 8);
    if (this->pieceidOn(regionY[1] - 1, y) == ENEMY_ROOK)
    {
        return true;
    }
    if (this->pieceidOn(regionY[2] + 1, y) == ENEMY_ROOK)
    {
        return true;
    }
    if (this->pieceidOn(regionY[0], y) == ENEMY_CANNON)
    {
        return true;
    }
    if (this->pieceidOn(regionY[3], y) == ENEMY_CANNON)
    {
        return true;
    }

    UINT32 bitlineX = this->getBitLineX(x);
    REGION_CANNON regionX = this->bitboard->getCannonRegion(bitlineX, y, 9);
    const PIECEID& p1 = this->pieceidOn(x, regionX[1] - 1);
    if (p1 == ENEMY_ROOK || p1 == ENEMY_KING)
    {
        return true;
    }
    const PIECEID& p2 = this->pieceidOn(x, regionX[2] + 1);
    if (p2 == ENEMY_ROOK || p2 == ENEMY_KING)
    {
        return true;
    }
    if (this->pieceidOn(x, regionX[0]) == ENEMY_CANNON)
    {
        return true;
    }
    if (this->pieceidOn(x, regionX[3]) == ENEMY_CANNON)
    {
        return true;
    }

    return false;
}

bool Board::hasProtector(int x, int y) const
{
    const Piece& piece = this->piecePosition(x, y);
    const TEAM& team = piece.team;

    // 兵
    const PIECEID MY_PAWN = R_PAWN * team;
    if (this->hasCrossedRiver(x + 1, y) && this->pieceidOn(x + 1, y) == MY_PAWN)
    {
        return true;
    }
    if (this->hasCrossedRiver(x - 1, y) && this->pieceidOn(x - 1, y) == MY_PAWN)
    {
        return true;
    }
    if (this->pieceidOn(x, team == RED ? y - 1 : y + 1) == MY_PAWN)
    {
        return true;
    }

    // 马
    const PIECEID MY_KNIGHT = R_KNIGHT * team;
    if (this->pieceidOn(x + 1, y + 1) == EMPTY_PIECEID)
    {
        if (this->pieceidOn(x + 2, y + 1) == MY_KNIGHT)
        {
            return true;
        }
        if (this->pieceidOn(x + 1, y + 2) == MY_KNIGHT)
        {
            return true;
        }
    }
    if (this->pieceidOn(x - 1, y + 1) == EMPTY_PIECEID)
    {
        if (this->pieceidOn(x - 2, y + 1) == MY_KNIGHT)
        {
            return true;
        }
        if (this->pieceidOn(x - 1, y + 2) == MY_KNIGHT)
        {
            return true;
        }
    }
    if (this->pieceidOn(x + 1, y - 1) == EMPTY_PIECEID)
    {
        if (this->pieceidOn(x + 2, y - 1) == MY_KNIGHT)
        {
            return true;
        }
        if (this->pieceidOn(x + 1, y - 2) == MY_KNIGHT)
        {
            return true;
        }
    }
    if (this->pieceidOn(x - 1, y - 1) == EMPTY_PIECEID)
    {
        if (this->pieceidOn(x - 2, y - 1) == MY_KNIGHT)
        {
            return true;
        }
        if (this->pieceidOn(x - 1, y - 2) == MY_KNIGHT)
        {
            return true;
        }
    }

    // 士、象、将
    const PIECEID MY_BISHOP = R_BISHOP * team;
    const PIECEID MY_GUARD = R_GUARD * team;
    const PIECEID MY_KING = R_KING * team;
    if (hasCrossedRiver(x, y) == false)
    {
        if (this->pieceidOn(x + 1, y + 1) == EMPTY_PIECEID)
        {
            if (this->pieceidOn(x + 2, y + 2) == MY_BISHOP)
            {
                return true;
            }
        }
        if (this->pieceidOn(x - 1, y + 1) == EMPTY_PIECEID)
        {
            if (this->pieceidOn(x - 2, y + 2) == MY_BISHOP)
            {
                return true;
            }
        }
        if (this->pieceidOn(x + 1, y - 1) == EMPTY_PIECEID)
        {
            if (this->pieceidOn(x + 2, y - 2) == MY_BISHOP)
            {
                return true;
            }
        }
        if (this->pieceidOn(x - 1, y - 1) == EMPTY_PIECEID)
        {
            if (this->pieceidOn(x - 2, y - 2) == MY_BISHOP)
            {
                return true;
            }
        }
        if (isInPalace(x, y))
        {
            if (this->pieceidOn(x + 1, y) == MY_GUARD)
            {
                return true;
            }
            if (this->pieceidOn(x - 1, y) == MY_GUARD)
            {
                return true;
            }
            if (this->pieceidOn(x, y + 1) == MY_GUARD)
            {
                return true;
            }
            if (this->pieceidOn(x, y - 1) == MY_GUARD)
            {
                return true;
            }
            if (this->pieceidOn(x + 1, y) == MY_KING)
            {
                return true;
            }
            if (this->pieceidOn(x - 1, y) == MY_KING)
            {
                return true;
            }
            if (this->pieceidOn(x, y + 1) == MY_KING)
            {
                return true;
            }
            if (this->pieceidOn(x, y - 1) == MY_KING)
            {
                return true;
            }
        }
    }

    // 车、炮
    const PIECEID MY_ROOK = R_ROOK * team;
    const PIECEID MY_CANNON = R_CANNON * team;

    UINT32 bitlineY = this->getBitLineY(y);
    REGION_CANNON regionY = this->bitboard->getCannonRegion(bitlineY, x, 8);
    if (this->pieceidOn(regionY[1] - 1, y) == MY_ROOK)
    {
        return true;
    }
    if (this->pieceidOn(regionY[2] + 1, y) == MY_ROOK)
    {
        return true;
    }
    if (this->pieceidOn(regionY[0], y) == MY_CANNON)
    {
        return true;
    }
    if (this->pieceidOn(regionY[3], y) == MY_CANNON)
    {
        return true;
    }
    UINT32 bitlineX = this->getBitLineX(x);
    REGION_CANNON regionX = this->bitboard->getCannonRegion(bitlineX, y, 9);
    if (this->pieceidOn(x, regionX[1] - 1) == MY_ROOK)
    {
        return true;
    }
    if (this->pieceidOn(x, regionX[2] + 1) == MY_ROOK)
    {
        return true;
    }
    if (this->pieceidOn(x, regionX[0]) == MY_CANNON)
    {
        return true;
    }
    if (this->pieceidOn(x, regionX[3]) == MY_CANNON)
    {
        return true;
    }
    return false;
}

void Board::doMove(Move move)
{
    const int &x1 = move.x1;
    const int &x2 = move.x2;
    const int &y1 = move.y1;
    const int &y2 = move.y2;
    const Piece &attacker = this->piecePosition(x1, y1);
    const Piece &captured = this->piecePosition(x2, y2);

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
    this->hashKey ^= HASHKEYS[attacker.pieceid][x1][y1];
    this->hashKey ^= HASHKEYS[attacker.pieceid][x2][y2];
    this->hashLock ^= HASHLOCKS[attacker.pieceid][x1][y1];
    this->hashLock ^= HASHLOCKS[attacker.pieceid][x2][y2];
    if (captured.pieceid != EMPTY_PIECEID)
    {
        this->hashKey ^= HASHKEYS[captured.pieceid][x1][y1];
        this->hashLock ^= HASHLOCKS[captured.pieceid][x2][y2];
    }
    this->hashKey ^= PLAYER_KEY;
    this->hashLock ^= PLAYER_LOCK;
}

void Board::undoMove()
{
    const Move &back = this->historyMoves.back();
    const int &x1 = back.x1;
    const int &x2 = back.x2;
    const int &y1 = back.y1;
    const int &y2 = back.y2;
    const Piece &attacker = this->historyMoves.back().attacker;
    const Piece &captured = this->historyMoves.back().captured;

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

void Board::doMoveSimple(Move move)
{
    const int &x1 = move.x1, &x2 = move.x2;
    const int &y1 = move.y1, &y2 = move.y2;
    const Piece &attacker = this->piecePosition(x1, y1);
    const Piece &captured = this->piecePosition(x2, y2);
    this->pieceidMap[x2][y2] = this->pieceidMap[x1][y1];
    this->pieceidMap[x1][y1] = 0;
    this->pieceIndexMap[x2][y2] = this->pieceIndexMap[x1][y1];
    this->pieceIndexMap[x1][y1] = -1;
    this->pieces[attacker.pieceIndex].x = x2;
    this->pieces[attacker.pieceIndex].y = y2;
    this->team = -this->team;
    this->historyMoves.emplace_back(Move{x1, y1, x2, y2});
    this->historyMoves.back().attacker = attacker;
    this->historyMoves.back().captured = captured;
    this->bitboard->doMove(x1, y1, x2, y2);
    if (captured.pieceIndex != -1)
    {
        this->pieces[captured.pieceIndex].isLive = false;
    }
}

void Board::undoMoveSimple()
{
    const Move &back = this->historyMoves.back();
    const int &x1 = back.x1, &x2 = back.x2;
    const int &y1 = back.y1, &y2 = back.y2;
    const Piece &attacker = back.attacker;
    const Piece &captured = back.captured;
    this->pieceidMap[x1][y1] = this->pieceidMap[x2][y2];
    this->pieceidMap[x2][y2] = captured.pieceid;
    this->pieceIndexMap[x1][y1] = this->pieceIndexMap[x2][y2];
    this->pieceIndexMap[x2][y2] = captured.pieceIndex;
    this->pieces[attacker.pieceIndex].x = x1;
    this->pieces[attacker.pieceIndex].y = y1;
    this->team = -this->team;
    this->historyMoves.pop_back();
    this->bitboard->undoMove(x1, y1, x2, y2, captured.pieceid != 0);
    if (captured.pieceIndex != -1)
    {
        this->pieces[captured.pieceIndex].isLive = true;
    }
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

void Board::vlOpenCalculator(int &vlOpen) const
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

void Board::vlAttackCalculator(int &vlRedAttack, int &vlBlackAttack) const
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
                this->hashKey ^= HASHKEYS[pid][x][y];
                this->hashLock ^= HASHLOCKS[pid][x][y];
            }
        }
    }
    if (this->team == BLACK)
    {
        this->hashKey ^= PLAYER_KEY;
        this->hashLock ^= PLAYER_LOCK;
    }
}

void Board::getMirrorHashinfo(int32 &mirrorHashKey, int32 &mirrorHashLock) const
{
    mirrorHashKey = 0;
    mirrorHashLock = 0;
    for (int x = 0; x < 9; x++)
    {
        for (int y = 0; y < 10; y++)
        {
            const PIECEID& pid = this->pieceidMap[x][y];
            if (pid != EMPTY_PIECEID)
            {
                mirrorHashKey ^= HASHKEYS[pid][size_t(8) - x][y];
                mirrorHashLock ^= HASHLOCKS[pid][size_t(8) - x][y];
            }
        }
    }
    if (this->team == BLACK)
    {
        mirrorHashKey ^= PLAYER_KEY;
        mirrorHashLock ^= PLAYER_LOCK;
    }
}

bool Board::isValidMoveInSituation(Move move)
{
    PIECEID attacker = this->pieceidOn(move.x1, move.y1);
    if (attacker == 0) // 若攻击者不存在, 则一定是不合理着法
        return false;
    if (attacker != move.attacker.pieceid) // 若攻击者不一致, 则一定是不合理着法
        return false;
    if (move.attacker.team != this->team) // 若攻击者的队伍和当前队伍不一致, 则一定是不合理着法
        return false;
    PIECEID captured = this->pieceidOn(move.x2, move.y2);
    if (captured != 0 && this->teamOn(move.x2, move.y2) ==
        this->teamOn(move.x1, move.y1)) // 吃子着法, 若吃子者和被吃者同队伍, 则一定不合理
        return false;

    // 分类
    if (abs(attacker) == R_ROOK)
    {
        if (move.x1 != move.x2 && move.y1 != move.y2) // 车走法, 若横纵坐标都不相同, 则一定不合理
            return false;
        // 生成车的着法范围, 看是否有障碍物
        UINT32 bitlineX = this->getBitLineX(move.x1);
        REGION_ROOK regionX = this->bitboard->getRookRegion(bitlineX, move.y1, 9);
        if (move.y2 < regionX[0] || move.y2 > regionX[1])
            return false;
        // 横向
        UINT32 bitlineY = this->getBitLineY(move.y1);
        REGION_ROOK regionY = this->bitboard->getRookRegion(bitlineY, move.x1, 8);
        if (move.x2 < regionY[0] || move.x2 > regionY[1])
            return false;
    }
    else if (abs(attacker) == R_KNIGHT)
    {
        if (move.x1 - 1 == move.x2 || move.x1 + 1 == move.x2) // 向哪一边走就判断那一边有没有障碍物
        {
            if (move.y1 - 2 == move.y2 && this->pieceidOn(move.x1, move.y1 - 1) != 0) // 若有障碍物则不合理
                return false;
            if (move.y1 + 2 == move.y2 && this->pieceidOn(move.x1, move.y1 + 1) != 0)
                return false;
        }
        else
        {
            if (move.x1 - 2 == move.x2 && this->pieceidOn(move.x1 - 1, move.y1) != 0) // 若有障碍物则不合理
                return false;
            if (move.x1 + 2 == move.x2 && this->pieceidOn(move.x1 + 1, move.y1) != 0)
                return false;
        }
        return true;
    }
    else if (abs(attacker) == R_BISHOP)
    {
        // 象走法, 不能有障碍物
        if (this->pieceidOn((move.x1 + move.x2) / 2, (move.y1 + move.y2) / 2) != 0)
            return false;
    }
    else if (abs(attacker) == R_CANNON)
    {
        if (move.x1 != move.x2 && move.y1 != move.y2) // 炮走法, 若横纵坐标都不同, 则一定不合理
            return false;
        // 生成炮的着法范围
        UINT32 bitlineX = this->getBitLineX(move.x1);
        REGION_CANNON regionX = this->bitboard->getCannonRegion(bitlineX, move.y1, 9);
        if ((move.y2 <= regionX[1] || move.y2 >= regionX[2] + 1) && move.y2 != regionX[0] && move.y2 != regionX[3])
            return false;
        // 横向
        UINT32 bitlineY = this->getBitLineY(move.y1);
        REGION_CANNON regionY = this->bitboard->getCannonRegion(bitlineY, move.x1, 8);
        if ((move.x2 <= regionY[1] || move.x2 >= regionY[2]) && move.x2 != regionY[0] && move.x2 != regionY[3])
            return false;
    }

    this->doMoveSimple(move);
    const bool skip = inCheck(-this->team);
    this->undoMoveSimple();

    return !skip;
}
