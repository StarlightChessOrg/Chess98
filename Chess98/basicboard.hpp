#pragma once
#include "base.hpp"
#include "bitboard.hpp"

class BasicBoard
{
public:
    BasicBoard(PIECEID_MAP pieceidMap, TEAM team);

public:
    PIECES pieces{};
    std::vector<PIECE_INDEX> redPieces{};
    std::vector<PIECE_INDEX> blackPieces{};
    PIECEID_MAP pieceidMap{};
    std::array<std::array<PIECE_INDEX, 10>, 9> pieceIndexMap{};
    std::map<PIECEID, std::vector<PIECE_INDEX>> pieceRegistry{};
    MOVES historyMoves{};
    TEAM team{};
    std::unique_ptr<Bitboard> bitboard{};

public:
    void doMove(Move move);
    void undoMove();
    PIECEID pieceidOn(int x, int y) const;
    TEAM teamOn(int x, int y) const;
    Piece pieceIndex(int i) const;
    Piece piecePosition(int x, int y) const;
    PIECES getAllLivePieces() const;
    PIECES getPiecesByTeam(TEAM team) const;
    Piece getPieceReg(PIECEID pieceid) const;
    PIECES getPiecesReg(PIECEID pieceid) const;
};

BasicBoard::BasicBoard(PIECEID_MAP pieceidMap, TEAM team)
    : team(team), pieceidMap(pieceidMap)
{

}

void BasicBoard::doMove(Move move)
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
    this->historyMoves.back().starter = attacker;
    this->historyMoves.back().captured = captured;
    this->bitboard->doMove(x1, y1, x2, y2);
    if (captured.pieceIndex != -1)
    {
        this->pieces[captured.pieceIndex].isLive = false;
    }
}

void BasicBoard::undoMove()
{
    const Move& back = this->historyMoves.back();
    const int &x1 = back.x1, &x2 = back.x2;
    const int &y1 = back.y1, &y2 = back.y2;
    const Piece& attacker = back.starter;
    const Piece& captured = back.captured;

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

PIECEID BasicBoard::pieceidOn(int x, int y) const
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

TEAM BasicBoard::teamOn(int x, int y) const
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

Piece BasicBoard::pieceIndex(int i) const
{
    return this->pieces[i];
}

Piece BasicBoard::piecePosition(int x, int y) const
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

PIECES BasicBoard::getAllLivePieces() const
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

PIECES BasicBoard::getPiecesByTeam(TEAM team) const
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

Piece BasicBoard::getPieceReg(PIECEID pieceid) const
{
    const Piece result = this->pieceIndex(this->pieceRegistry.at(pieceid)[0]);
    return result.isLive ? result : Piece{};
}

PIECES BasicBoard::getPiecesReg(PIECEID pieceid) const
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
