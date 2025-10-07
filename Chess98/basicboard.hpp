#pragma once
#include "base.hpp"

class BasicBoard
{
public:
    BasicBoard(PIECEID_MAP pieceidMap, TEAM team);
    ~BasicBoard();

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
    Piece getPieceReg(PIECEID pieceid) const;
    PIECES getPiecesReg(PIECEID pieceid) const;
};

BasicBoard::BasicBoard(PIECEID_MAP pieceidMap, TEAM team)
    : team(team), pieceidMap(pieceidMap)
{
    
}

PIECEID BasicBoard::pieceidOn(int x, int y) const
{
    return (x >= 0 && x <= 8 && y >= 0 && y <= 9) ? this->pieceidMap[x][y] : OVERFLOW_PIECEID;
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
