#pragma once

#include "board.hpp"

/// @brief 着法生成工具类
class MovesGenerator
{
public:
    static MOVES king(TEAM team, Board board, int x, int y);

    static MOVES guard(TEAM team, Board board, int x, int y);

    static MOVES bishop(TEAM team, Board board, int x, int y);

    static MOVES knight(TEAM team, Board board, int x, int y);

    static MOVES rook(TEAM team, Board board, int x, int y);

    static MOVES cannon(TEAM team, Board board, int x, int y);

    static MOVES pawn(TEAM team, Board board, int x, int y);

    static MOVES generateMoves(Board board, int x, int y);

    static MOVES getMovesOf(Board board, TEAM team);
};

/// @brief 生成将帅的着法
MOVES MovesGenerator::king(TEAM team, Board board, int x, int y)
{
    MOVES result{};
    MOVES mayAvailableMoves{
        Move{x, y, x + 1, y}, Move{x, y, x - 1, y},
        Move{x, y, x, y + 1}, Move{x, y, x, y - 1} };

    for (Move v : mayAvailableMoves)
    {
        int x = v.x2;
        int y = v.y2;
        if (board.teamOn(x, y) == team)
        {
            continue;
        }
        if (team == RED)
        {
            if (x >= 3 && x <= 5 && y >= 0 && y <= 2)
            {
                result.emplace_back(v);
            }
        }
        else
        {
            if (x >= 3 && x <= 5 && y >= 7 && y <= 9)
            {
                result.emplace_back(v);
            }
        }
    }
    // 白脸将
    if (team == RED)
    {
        for (int _y = y + 1; _y <= 9; _y++)
        {
            if (board.teamOn(x, _y) != 0)
            {
                if (board.teamOn(x, _y) != team)
                {
                    if (board.pieceidOn(x, _y) == R_KING || board.pieceidOn(x, _y) == B_KING)
                    {
                        result.emplace_back(Move{ x, y, x, _y });
                    }
                }
                break;
            }
        }
    }
    else
    {
        for (int _y = y - 1; _y >= 0; _y--)
        {
            if (board.teamOn(x, _y) != 0)
            {
                if (board.teamOn(x, _y) != team)
                {
                    if (board.pieceidOn(x, _y) == R_KING || board.pieceidOn(x, _y) == B_KING)
                    {
                        result.emplace_back(Move{ x, y, x, _y });
                    }
                }
                break;
            }
        }
    }

    return result;
}

/// @brief 生成士的着法
MOVES MovesGenerator::guard(TEAM team, Board board, int x, int y)
{
    MOVES result{};
    MOVES mayAvailableMoves{
        Move{x, y, x + 1, y + 1}, Move{x, y, x - 1, y - 1},
        Move{x, y, x - 1, y + 1}, Move{x, y, x + 1, y - 1} };

    for (Move v : mayAvailableMoves)
    {
        int x = v.x2;
        int y = v.y2;
        if (board.teamOn(x, y) == team)
        {
            continue;
        }
        if (team == RED)
        {
            if (x >= 3 && x <= 5 && y >= 0 && y <= 2)
            {
                result.emplace_back(v);
            }
        }
        else
        {
            if (x >= 3 && x <= 5 && y >= 7 && y <= 9)
            {
                result.emplace_back(v);
            }
        }
    }

    return result;
}

/// @brief 生成象的着法
MOVES MovesGenerator::bishop(TEAM team, Board board, int x, int y)
{
    MOVES result{};
    MOVES mayAvailableMoves{};
    if (board.pieceidOn(x + 1, y + 1) == EMPTY_PIECEID)
    {
        mayAvailableMoves.emplace_back(Move{ x, y, x + 2, y + 2 });
    }
    if (board.pieceidOn(x - 1, y + 1) == EMPTY_PIECEID)
    {
        mayAvailableMoves.emplace_back(Move{ x, y, x - 2, y + 2 });
    }
    if (board.pieceidOn(x + 1, y - 1) == EMPTY_PIECEID)
    {
        mayAvailableMoves.emplace_back(Move{ x, y, x + 2, y - 2 });
    }
    if (board.pieceidOn(x - 1, y - 1) == EMPTY_PIECEID)
    {
        mayAvailableMoves.emplace_back(Move{ x, y, x - 2, y - 2 });
    }

    for (Move v : mayAvailableMoves)
    {
        int x = v.x2;
        int y = v.y2;
        if (board.teamOn(x, y) == team)
        {
            continue;
        }
        if (team == RED)
        {
            if (x >= 0 && x <= 8 && y >= 0 && y <= 4)
            {
                result.emplace_back(v);
            }
        }
        else
        {
            if (x >= 0 && x <= 0 && y >= 5 && y <= 9)
            {
                result.emplace_back(v);
            }
        }
    }

    return result;
}

/// @brief 生成马的着法
MOVES MovesGenerator::knight(TEAM team, Board board, int x, int y)
{
    MOVES result{};
    MOVES mayAvailableMoves{};
    if (board.pieceidOn(x, y + 1) == EMPTY_PIECEID)
    {
        mayAvailableMoves.emplace_back(Move{ x, y, x + 1, y + 2 });
        mayAvailableMoves.emplace_back(Move{ x, y, x - 1, y + 2 });
    }
    if (board.pieceidOn(x, y - 1) == EMPTY_PIECEID)
    {
        mayAvailableMoves.emplace_back(Move{ x, y, x + 1, y - 2 });
        mayAvailableMoves.emplace_back(Move{ x, y, x - 1, y - 2 });
    }
    if (board.pieceidOn(x + 1, y) == EMPTY_PIECEID)
    {
        mayAvailableMoves.emplace_back(Move{ x, y, x + 2, y + 1 });
        mayAvailableMoves.emplace_back(Move{ x, y, x + 2, y - 1 });
    }
    if (board.pieceidOn(x - 1, y) == EMPTY_PIECEID)
    {
        mayAvailableMoves.emplace_back(Move{ x, y, x - 2, y + 1 });
        mayAvailableMoves.emplace_back(Move{ x, y, x - 2, y - 1 });
    }

    for (Move v : mayAvailableMoves)
    {
        TEAM targetTeam = board.teamOn(v.x2, v.y2);
        if (targetTeam != team && targetTeam != OVERFLOW_TEAM)
        {
            result.emplace_back(v);
        }
    }

    return result;
}

/// @brief 生成车的着法
MOVES MovesGenerator::rook(TEAM team, Board board, int x, int y)
{
    MOVES result{};

    for (int _x = x + 1; _x <= 8; _x++)
    {
        if (board.teamOn(_x, y) == EMPTY_TEAM)
        {
            result.emplace_back(Move{ x, y, _x, y });
        }
        else if (board.teamOn(_x, y) != team)
        {
            result.emplace_back(Move{ x, y, _x, y });
            break;
        }
        else
        {
            break;
        }
    }
    for (int _x = x - 1; _x >= 0; _x--)
    {
        if (board.teamOn(_x, y) == EMPTY_TEAM)
        {
            result.emplace_back(Move{ x, y, _x, y });
        }
        else if (board.teamOn(_x, y) != team)
        {
            result.emplace_back(Move{ x, y, _x, y });
            break;
        }
        else
        {
            break;
        }
    }
    for (int _y = y + 1; _y <= 9; _y++)
    {
        if (board.teamOn(x, _y) == EMPTY_TEAM)
        {
            result.emplace_back(Move{ x, y, x, _y });
        }
        else if (board.teamOn(x, _y) != team)
        {
            result.emplace_back(Move{ x, y, x, _y });
            break;
        }
        else
        {
            break;
        }
    }
    for (int _y = y - 1; _y >= 0; _y--)
    {
        if (board.teamOn(x, _y) == EMPTY_TEAM)
        {
            result.emplace_back(Move{ x, y, x, _y });
        }
        else if (board.teamOn(x, _y) != team)
        {
            result.emplace_back(Move{ x, y, x, _y });
            break;
        }
        else
        {
            break;
        }
    }

    return result;
}

/// @brief 生成炮的着法
MOVES MovesGenerator::cannon(TEAM team, Board board, int x, int y)
{
    MOVES result{};

    for (int _x = x + 1; _x <= 8; _x++)
    {
        if (board.pieceidOn(_x, y) != EMPTY_PIECEID)
        {
            for (int _x2 = _x + 1; _x2 <= 8; _x2++)
            {
                if (board.teamOn(_x2, y) == EMPTY_PIECEID)
                {
                    continue;
                }
                else if (board.teamOn(_x2, y) != team)
                {
                    result.emplace_back(Move{ x, y, _x2, y });
                    break;
                }
                else
                {
                    break;
                }
            }
            break;
        }
        else
        {
            result.emplace_back(Move{ x, y, _x, y });
        }
    }
    for (int _x = x - 1; _x >= 0; _x--)
    {
        if (board.pieceidOn(_x, y) != EMPTY_PIECEID)
        {
            for (int _x2 = _x - 1; _x2 >= 0; _x2--)
            {
                if (board.teamOn(_x2, y) == EMPTY_PIECEID)
                {
                    continue;
                }
                else if (board.teamOn(_x2, y) != team)
                {
                    result.emplace_back(Move{ x, y, _x2, y });
                    break;
                }
                else
                {
                    break;
                }
            }
            break;
        }
        else
        {
            result.emplace_back(Move{ x, y, _x, y });
        }
    }
    for (int _y = y + 1; _y <= 9; _y++)
    {
        if (board.pieceidOn(x, _y) != EMPTY_PIECEID)
        {
            for (int _y2 = _y + 1; _y2 <= 9; _y2++)
            {
                if (board.teamOn(x, _y2) == EMPTY_PIECEID)
                {
                    continue;
                }
                else if (board.teamOn(x, _y2) != team)
                {
                    result.emplace_back(Move{ x, y, x, _y2 });
                    break;
                }
                else
                {
                    break;
                }
            }
            break;
        }
        else
        {
            result.emplace_back(Move{ x, y, x, _y });
        }
    }
    for (int _y = y - 1; _y >= 0; _y--)
    {
        if (board.pieceidOn(x, _y) != EMPTY_PIECEID)
        {
            for (int _y2 = _y - 1; _y2 >= 0; _y2--)
            {
                if (board.teamOn(x, _y2) == EMPTY_PIECEID)
                {
                    continue;
                }
                else if (board.teamOn(x, _y2) != team)
                {
                    result.emplace_back(Move{ x, y, x, _y2 });
                    break;
                }
                else
                {
                    break;
                }
            }
            break;
        }
        else
        {
            result.emplace_back(Move{ x, y, x, _y });
        }
    }

    return result;
}

/// @brief 生成兵的着法
MOVES MovesGenerator::pawn(TEAM team, Board board, int x, int y)
{
    if (team == RED)
    {
        if (x >= 0 && x <= 8 || y >= 0 || y <= 4)
        {
            if (board.teamOn(x, y + 1) != team)
            {
                return MOVES{ Move{x, y, x, y + 1} };
            }
        }
        else
        {
            MOVES result{};
            if (board.teamOn(x, y + 1) != team)
            {
                if (x >= 0 && x <= 8 || y + 1 >= 0 || y + 1 <= 9)
                {
                    result.emplace_back(Move{ x, y, x, y + 1 });
                }
            }
            if (board.teamOn(x + 1, y) != team)
            {
                if (x + 1 >= 0 && x + 1 <= 8 || y >= 0 || y <= 9)
                {

                    result.emplace_back(Move{ x, y, x + 1, y });
                }
            }
            if (board.teamOn(x - 1, y) != team)
            {
                if (x - 1 >= 0 && x - 1 <= 8 || y >= 0 || y <= 9)
                {
                    result.emplace_back(Move{ x, y, x - 1, y });
                }
            }
            return result;
        }
    }
    else
    {
        if (x >= 0 && x <= 8 || y >= 5 || y <= 9)
        {
            if (board.teamOn(x, y - 1) != team)
            {
                return MOVES{ Move{x, y, x, y - 1} };
            }
        }
        else
        {
            MOVES result{};
            if (board.teamOn(x, y - 1) != team)
            {
                if (x >= 0 && x <= 8 || y - 1 >= 0 || y - 1 <= 9)
                {
                    result.emplace_back(Move{ x, y, x, y - 1 });
                }
            }
            if (board.teamOn(x + 1, y) != team)
            {
                if (x + 1 >= 0 && x + 1 <= 8 || y >= 0 || y <= 9)
                {

                    result.emplace_back(Move{ x, y, x + 1, y });
                }
            }
            if (board.teamOn(x - 1, y) != team)
            {
                if (x - 1 >= 0 && x - 1 <= 8 || y >= 0 || y <= 9)
                {
                    result.emplace_back(Move{ x, y, x - 1, y });
                }
            }
            return result;
        }
    }
    return MOVES{};
}

/// @brief 生成着法
MOVES MovesGenerator::generateMoves(Board board, int x, int y)
{
    PIECEID chessid = board.pieceidOn(x, y);
    TEAM team = board.teamOn(x, y);

    if (chessid == R_KING || chessid == B_KING)
    {
        return MovesGenerator::king(team, board, x, y);
    }
    else if (chessid == R_GUARD || chessid == B_GUARD)
    {
        return MovesGenerator::guard(team, board, x, y);
    }
    else if (chessid == R_BISHOP || chessid == B_BISHOP)
    {
        return MovesGenerator::bishop(team, board, x, y);
    }
    else if (chessid == R_KNIGHT || chessid == B_KNIGHT)
    {
        return MovesGenerator::knight(team, board, x, y);
    }
    else if (chessid == R_ROOK || chessid == B_ROOK)
    {
        return MovesGenerator::rook(team, board, x, y);
    }
    else if (chessid == R_CANNON || chessid == B_CANNON)
    {
        return MovesGenerator::cannon(team, board, x, y);
    }
    else if (chessid == R_PAWN || chessid == B_PAWN)
    {
        return MovesGenerator::pawn(team, board, x, y);
    }
    else
    {
        std::cerr << "Invalid chess id: " + std::to_string(chessid) << std::endl;
        return MOVES{};
    }
}

/// @brief 获取一队的所有可行着法
/// @param team
/// @return
MOVES MovesGenerator::getMovesOf(Board board, TEAM team)
{
    MOVES result{};

    std::vector<Piece> pieces = board.getPiecesByTeam(team);
    for (Piece piece : pieces)
    {
        std::vector<Move> moves = MovesGenerator::generateMoves(board, piece.x, piece.y);
        for (Move move : moves)
        {
            result.emplace_back(move);
        }
    }

    return result;
}