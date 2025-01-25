#pragma once
#include "board.hpp"

/// @brief 判断当前一方是否被将军
/// @param board
/// @return
bool inCheck(Board &board)
{
    Piece *king = board.team == RED ? board.pieceRedKing : board.pieceBlackKing;

    // 判断敌方的兵是否在附近
    bool c1 = abs(board.pieceidOn(king->x + 1, king->y)) == R_PAWN;
    bool c2 = abs(board.pieceidOn(king->x - 1, king->y)) == R_PAWN;
    bool c3 = abs(board.pieceidOn(king->x, (king->getTeam() == RED ? king->y - 1 : king->y + 1))) == R_PAWN;
    if (c1 || c2 || c3)
    {
        return true;
    }

    // 判断敌方的马是否在附近
    PIECEID piece1 = 0;
    PIECEID piece2 = 0;
    PIECEID piece3 = 0;
    PIECEID piece4 = 0;
    PIECEID piece5 = 0;
    PIECEID piece6 = 0;
    PIECEID piece7 = 0;
    PIECEID piece8 = 0;
    if (board.pieceidOn(king->x + 1, king->y) != 0)
    {
        piece1 = board.pieceidOn(king->x + 2, king->y + 1);
        piece2 = board.pieceidOn(king->x + 2, king->y - 1);
    }
    if (board.pieceidOn(king->x - 1, king->y) != 0)
    {
        piece3 = board.pieceidOn(king->x - 2, king->y + 1);
        piece4 = board.pieceidOn(king->x - 2, king->y - 1);
    }
    if (board.pieceidOn(king->x, king->y + 1) != 0)
    {
        piece5 = board.pieceidOn(king->x + 1, king->y + 2);
        piece6 = board.pieceidOn(king->x - 1, king->y + 2);
    }
    if (board.pieceidOn(king->x, king->y - 1) != 0)
    {
        piece7 = board.pieceidOn(king->x + 1, king->y - 2);
        piece8 = board.pieceidOn(king->x - 1, king->y - 2);
    }
    bool c4 = abs(piece1) == R_KNIGHT && (board.team > 0 ? (piece1 < 0) : (piece1 > 0));
    bool c5 = abs(piece2) == R_KNIGHT && (board.team > 0 ? (piece2 < 0) : (piece2 > 0));
    bool c6 = abs(piece3) == R_KNIGHT && (board.team > 0 ? (piece3 < 0) : (piece3 > 0));
    bool c7 = abs(piece4) == R_KNIGHT && (board.team > 0 ? (piece4 < 0) : (piece4 > 0));
    bool c8 = abs(piece5) == R_KNIGHT && (board.team > 0 ? (piece5 < 0) : (piece5 > 0));
    bool c9 = abs(piece6) == R_KNIGHT && (board.team > 0 ? (piece6 < 0) : (piece6 > 0));
    bool c10 = abs(piece7) == R_KNIGHT && (board.team > 0 ? (piece7 < 0) : (piece8 > 0));
    bool c11 = abs(piece8) == R_KNIGHT && (board.team > 0 ? (piece8 < 0) : (piece8 > 0));

    // 判断是否被将军
    if (c4 || c5 || c6 || c7 || c8 || c9 || c10 || c11)
    {
        return true;
    }

    // 白脸将、车、炮
    bool barrierDetected = false;
    for (int x = king->x + 1; x < 9; x++)
    {
        PIECEID pieceid = board.pieceidOn(x, king->y);
        TEAM team = board.teamOn(x, king->y);
        if (abs(pieceid) == R_ROOK &&
            team != board.team &&
            barrierDetected == false)
        {
            return true;
        }
        else if (abs(pieceid) == R_CANNON &&
                 team != board.team &&
                 barrierDetected == true)
        {
            return true;
        }
        else if (pieceid != 0)
        {
            if (barrierDetected == false)
            {
                barrierDetected = true;
            }
            else
            {
                break;
            }
        }
    }
    barrierDetected = false;
    for (int x = king->x - 1; x >= 0; x--)
    {
        PIECEID pieceid = board.pieceidOn(x, king->y);
        TEAM team = board.teamOn(x, king->y);
        if (abs(pieceid) == R_ROOK &&
            team != board.team &&
            barrierDetected == false)
        {
            return true;
        }
        else if (abs(pieceid) == R_CANNON &&
                 team != board.team &&
                 barrierDetected == true)
        {
            return true;
        }
        else if (pieceid != 0)
        {
            if (barrierDetected == false)
            {
                barrierDetected = true;
            }
            else
            {
                break;
            }
        }
    }
    barrierDetected = false;
    if (board.team == RED)
    {
        for (int y = king->y + 1; y < 10; y++)
        {
            PIECEID pieceid = board.pieceidOn(king->x, y);
            TEAM team = board.teamOn(king->x, y);
            if ((abs(pieceid) == R_ROOK ||
                 abs(pieceid) == R_KING) &&
                team != board.team &&
                barrierDetected == false)
            {
                return true;
            }
            else if (abs(pieceid) == R_CANNON &&
                     team != board.team &&
                     barrierDetected == true)
            {
                return true;
            }
            else if (pieceid != 0)
            {
                if (barrierDetected == false)
                {
                    barrierDetected = true;
                }
                else
                {
                    break;
                }
            }
        }
    }
    else
    {
        for (int y = king->y - 1; y >= 0; y--)
        {
            PIECEID pieceid = board.pieceidOn(king->x, y);
            TEAM team = board.teamOn(king->x, y);
            if ((abs(pieceid) == R_ROOK ||
                 abs(pieceid) == R_KING) &&
                team != board.team &&
                barrierDetected == false)
            {
                return true;
            }
            else if (abs(pieceid) == R_CANNON &&
                     team != board.team &&
                     barrierDetected == true)
            {
                return true;
            }
            else if (pieceid != 0)
            {
                if (barrierDetected == false)
                {
                    barrierDetected = true;
                }
                else
                {
                    break;
                }
            }
        }
    }
    return false;
}

/// @brief 棋子关系函数 获取一个棋子被哪些棋子攻击（不获取将帅的情况）
/// @param board
/// @param piece
/// @return
std::vector<Piece> relationship_beAttacked(Board &board, Piece piece)
{
    std::vector<Piece> selected{};

    // 兵、将
    if ((abs(board.pieceidOn(piece.x + 1, piece.y)) == R_PAWN ||
         abs(board.pieceidOn(piece.x + 1, piece.y) == R_KING)) &&
        board.teamOn(piece.x + 1, piece.y) != piece.getTeam())
    {
        selected.emplace_back(board.piecePosition(piece.x + 1, piece.y));
    }
    if ((abs(board.pieceidOn(piece.x - 1, piece.y)) == R_PAWN ||
         abs(board.pieceidOn(piece.x - 1, piece.y) == R_KING)) &&
        board.teamOn(piece.x - 1, piece.y) != piece.getTeam())
    {
        selected.emplace_back(board.piecePosition(piece.x - 1, piece.y));
    }
    if (abs(board.pieceidOn(piece.x, (piece.getTeam() == RED) ? piece.y - 1 : piece.y + 1)) == R_PAWN &&
        board.teamOn(piece.x, (piece.getTeam() == RED) ? piece.y - 1 : piece.y + 1) != piece.getTeam())
    {
        selected.emplace_back(board.piecePosition(piece.x, (piece.getTeam() == RED) ? piece.y - 1 : piece.y + 1));
    }
    if (abs(board.pieceidOn(piece.x, piece.y + 1)) == R_KING && board.teamOn(piece.x, piece.y + 1) != piece.getTeam())
    {
        selected.emplace_back(board.piecePosition(piece.x, piece.y + 1));
    }
    if (abs(board.pieceidOn(piece.x, piece.y - 1)) == R_KING && board.teamOn(piece.x, piece.y - 1) != piece.getTeam())
    {
        selected.emplace_back(board.piecePosition(piece.x, piece.y - 1));
    }

    // 马
    Piece knight1{OVERFLOW_PIECEID, -1, -1, -1};
    Piece knight2{OVERFLOW_PIECEID, -1, -1, -1};
    Piece knight3{OVERFLOW_PIECEID, -1, -1, -1};
    Piece knight4{OVERFLOW_PIECEID, -1, -1, -1};
    Piece knight5{OVERFLOW_PIECEID, -1, -1, -1};
    Piece knight6{OVERFLOW_PIECEID, -1, -1, -1};
    Piece knight7{OVERFLOW_PIECEID, -1, -1, -1};
    Piece knight8{OVERFLOW_PIECEID, -1, -1, -1};
    if (board.pieceidOn(piece.x + 1, piece.y) == 0)
    {
        if (abs(board.pieceidOn(piece.x + 2, piece.y + 1)) == R_KNIGHT &&
            board.teamOn(piece.x + 2, piece.y + 1) != piece.getTeam())
        {
            selected.emplace_back(board.piecePosition(piece.x + 2, piece.y + 1));
        }
        if (abs(board.pieceidOn(piece.x + 2, piece.y - 1)) == R_KNIGHT &&
            board.teamOn(piece.x + 2, piece.y - 1) != piece.getTeam())
        {
            selected.emplace_back(board.piecePosition(piece.x + 2, piece.y - 1));
        }
    }
    if (board.pieceidOn(piece.x - 1, piece.y) == 0)
    {
        if (abs(board.pieceidOn(piece.x - 2, piece.y + 1)) == R_KNIGHT &&
            board.teamOn(piece.x - 2, piece.y + 1) != piece.getTeam())
        {
            selected.emplace_back(board.piecePosition(piece.x - 2, piece.y + 1));
        }
        if (abs(board.pieceidOn(piece.x - 2, piece.y - 1)) == R_KNIGHT &&
            board.teamOn(piece.x - 2, piece.y - 1) != piece.getTeam())
        {
            selected.emplace_back(board.piecePosition(piece.x - 2, piece.y - 1));
        }
    }
    if (board.pieceidOn(piece.x, piece.y + 1) == 0)
    {
        if (abs(board.pieceidOn(piece.x + 1, piece.y + 2)) == R_KNIGHT &&
            board.teamOn(piece.x + 1, piece.y + 2) != piece.getTeam())
        {
            selected.emplace_back(board.piecePosition(piece.x + 1, piece.y + 2));
        }
        if (abs(board.pieceidOn(piece.x - 1, piece.y + 2)) == R_KNIGHT &&
            board.teamOn(piece.x - 1, piece.y + 2) != piece.getTeam())
        {
            selected.emplace_back(board.piecePosition(piece.x - 1, piece.y + 2));
        }
    }
    if (board.pieceidOn(piece.x, piece.y - 1) == 0)
    {
        if (abs(board.pieceidOn(piece.x + 1, piece.y - 2)) == R_KNIGHT &&
            board.teamOn(piece.x + 1, piece.y - 2) != piece.getTeam())
        {
            selected.emplace_back(board.piecePosition(piece.x + 1, piece.y - 2));
        }
        if (abs(board.pieceidOn(piece.x - 1, piece.y - 2)) == R_KNIGHT &&
            board.teamOn(piece.x - 1, piece.y - 2) != piece.getTeam())
        {
            selected.emplace_back(board.piecePosition(piece.x - 1, piece.y - 2));
        }
    }

    // 士、象
    if (board.pieceidOn(piece.x + 1, piece.y + 1) == 0)
    {
        if (abs(board.pieceidOn(piece.x + 2, piece.y + 2)) == R_BISHOP &&
            board.teamOn(piece.x + 2, piece.y + 2) != piece.getTeam())
            selected.emplace_back(board.piecePosition(piece.x + 2, piece.y + 2));
    }
    if (board.pieceidOn(piece.x - 1, piece.y + 1) == 0)
    {
        if (abs(board.pieceidOn(piece.x - 2, piece.y + 2)) == R_BISHOP &&
            board.teamOn(piece.x - 2, piece.y + 2) != piece.getTeam())
            selected.emplace_back(board.piecePosition(piece.x - 2, piece.y + 2));
    }
    if (board.pieceidOn(piece.x + 1, piece.y - 1) == 0)
    {
        if (abs(board.pieceidOn(piece.x + 2, piece.y - 2)) == R_BISHOP &&
            board.teamOn(piece.x + 2, piece.y - 2) != piece.getTeam())
            selected.emplace_back(board.piecePosition(piece.x + 2, piece.y - 2));
    }
    if (board.pieceidOn(piece.x - 1, piece.y - 1) == 0)
    {
        if (abs(board.pieceidOn(piece.x - 2, piece.y - 2)) == R_BISHOP &&
            board.teamOn(piece.x - 2, piece.y - 2) != piece.getTeam())
            selected.emplace_back(board.piecePosition(piece.x - 2, piece.y - 2));
    }

    if (abs(board.pieceidOn(piece.x + 1, piece.y + 1)) == R_GUARD &&
        board.teamOn(piece.x + 1, piece.y + 1) != piece.getTeam())
    {
        selected.emplace_back(board.piecePosition(piece.x + 1, piece.y + 1));
    }
    if (abs(board.pieceidOn(piece.x - 1, piece.y + 1)) == R_GUARD &&
        board.teamOn(piece.x - 1, piece.y + 1) != piece.getTeam())
    {
        selected.emplace_back(board.piecePosition(piece.x - 1, piece.y + 1));
    }
    if (abs(board.pieceidOn(piece.x + 1, piece.y - 1)) == R_GUARD &&
        board.teamOn(piece.x + 1, piece.y - 1) != piece.getTeam())
    {
        selected.emplace_back(board.piecePosition(piece.x + 1, piece.y - 1));
    }
    if (abs(board.pieceidOn(piece.x - 1, piece.y - 1)) == R_GUARD &&
        board.teamOn(piece.x - 1, piece.y - 1) != piece.getTeam())
    {
        selected.emplace_back(board.piecePosition(piece.x - 1, piece.y - 1));
    }

    // 车、炮
    Piece rook1{OVERFLOW_PIECEID, -1, -1, -1};
    Piece rook2{OVERFLOW_PIECEID, -1, -1, -1};
    Piece rook3{OVERFLOW_PIECEID, -1, -1, -1};
    Piece rook4{OVERFLOW_PIECEID, -1, -1, -1};
    Piece cannon1{OVERFLOW_PIECEID, -1, -1, -1};
    Piece cannon2{OVERFLOW_PIECEID, -1, -1, -1};
    Piece cannon3{OVERFLOW_PIECEID, -1, -1, -1};
    Piece cannon4{OVERFLOW_PIECEID, -1, -1, -1};
    bool barrierDetected = false;
    for (int x = piece.x + 1; x < 9; x++)
    {
        PIECEID pieceid = board.pieceidOn(x, piece.y);
        TEAM team = board.teamOn(x, piece.y);
        if (abs(pieceid) == R_ROOK &&
            team != piece.getTeam() &&
            barrierDetected == false)
        {
            selected.emplace_back(board.piecePosition(x, piece.y));
            barrierDetected = true;
        }
        else if (abs(pieceid) == R_CANNON &&
                 team != piece.getTeam() &&
                 barrierDetected == true)
        {
            selected.emplace_back(board.piecePosition(x, piece.y));
        }
        else if (pieceid != 0)
        {
            if (barrierDetected == false)
            {
                barrierDetected = true;
            }
            else
            {
                break;
            }
        }
    }
    barrierDetected = false;
    for (int x = piece.x - 1; x >= 0; x--)
    {
        PIECEID pieceid = board.pieceidOn(x, piece.y);
        TEAM team = board.teamOn(x, piece.y);
        if (abs(pieceid) == R_ROOK &&
            team != piece.getTeam() &&
            barrierDetected == false)
        {
            selected.emplace_back(board.piecePosition(x, piece.y));
            barrierDetected = true;
        }
        else if (abs(pieceid) == R_CANNON &&
                 team != piece.getTeam() &&
                 barrierDetected == true)
        {
            selected.emplace_back(board.piecePosition(x, piece.y));
        }
        else if (pieceid != 0)
        {
            if (barrierDetected == false)
            {
                barrierDetected = true;
            }
            else
            {
                break;
            }
        }
    }
    barrierDetected = false;
    for (int y = piece.y + 1; y < 10; y++)
    {
        PIECEID pieceid = board.pieceidOn(piece.x, y);
        TEAM team = board.teamOn(piece.x, y);
        if (abs(pieceid) == R_ROOK &&
            team != piece.getTeam() &&
            barrierDetected == false)
        {
            selected.emplace_back(board.piecePosition(piece.x, y));
            barrierDetected = true;
        }
        else if (abs(pieceid) == R_CANNON &&
                 team != piece.getTeam() &&
                 barrierDetected == true)
        {
            selected.emplace_back(board.piecePosition(piece.x, y));
        }
        else if (pieceid != 0)
        {
            if (barrierDetected == false)
            {
                barrierDetected = true;
            }
            else
            {
                break;
            }
        }
    }
    barrierDetected = false;
    for (int y = piece.y - 1; y >= 0; y--)
    {
        PIECEID pieceid = board.pieceidOn(piece.x, y);
        TEAM team = board.teamOn(piece.x, y);
        if ((abs(pieceid) == R_ROOK ||
             abs(pieceid) == R_KING) &&
            team != piece.getTeam() &&
            barrierDetected == false)
        {
            selected.emplace_back(board.piecePosition(piece.x, y));
            barrierDetected = true;
        }
        else if (abs(pieceid) == R_CANNON &&
                 team != piece.getTeam() &&
                 barrierDetected == true)
        {
            selected.emplace_back(board.piecePosition(piece.x, y));
        }
        else if (pieceid != 0)
        {
            if (barrierDetected == false)
            {
                barrierDetected = true;
            }
            else
            {
                break;
            }
        }
    }

    // 筛选
    std::vector<Piece> result{};
    for (const Piece &piece : selected)
    {
        if (piece.pieceIndex != -1)
        {
            result.emplace_back(piece);
        }
    }
    return result;
}

/// @brief 棋子关系函数 获取一个棋子被哪些棋子保护
/// @param board
/// @param piece
/// @return
std::vector<Piece> relationship_beProtected(Board &board, Piece piece)
{
    // 相当于一个敌方的棋子被我方的棋子攻击
    Piece replacement{-1, piece.x, piece.y, -piece.getTeam()};
    return relationship_beAttacked(board, replacement);
}

/// @brief 是否有保护的棋子
/// @param board
/// @param x
/// @param y
/// @return
bool relationship_hasProtector(Board &board, int x, int y)
{
    return relationship_beProtected(board, board.piecePosition(x, y)).size() > 0;
}

/// @brief 检查是否是过河卒
/// @param board
/// @param x
/// @param y
/// @return
bool isGoodPawn(Board &board, int x, int y)
{
    PIECEID pieceid = board.pieceidOn(x, y);
    if (pieceid == R_PAWN && y >= 5 && y <= 9)
    {
        return true;
    }
    if (pieceid == B_PAWN && y >= 0 && y <= 4)
    {
        return true;
    }
    return false;
}
