#pragma once
#include "base.hpp"

class BasicBoard
{
public:
    BasicBoard(PIECEID_MAP pieceidMap, TEAM team);

public:
    PIECES pieces{};
    std::vector<PIECE_INDEX> redPieces{};
    std::vector<PIECE_INDEX> blackPieces{};
    PIECEID_MAP pieceidMap{};
    MOVES historyMoves{};
    TEAM team{};
    std::map<PIECEID, std::vector<PIECE_INDEX>> pieceRegistry{};

public:
    void doMove(int x1, int y1, int x2, int y2);
    void undoMove(int x1, int y1, int x2, int y2);
    PIECEID pieceidOn(int x, int y) const;
    TEAM teamOn(int x, int y) const;
    Piece pieceIndex(int i) const;
    PIECES getAllLivePieces() const;
    PIECES getPiecesByTeam(TEAM team) const;
    Piece getPieceReg(PIECEID pieceid) const;
    PIECES getPiecesReg(PIECEID pieceid) const;
};

BasicBoard::BasicBoard(PIECEID_MAP pieceidMap, TEAM team)
    : team(team), pieceidMap(pieceidMap)
{
}

void BasicBoard::doMove(int x1, int y1, int x2, int y2)
{
}

void BasicBoard::undoMove(int x1, int y1, int x2, int y2)
{
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
