#pragma once
#include "utils.hpp"

class MovesGenerate
{
public:
    static MOVES king(TEAM team, Board &board, int x, int y);
    static MOVES guard(TEAM team, Board &board, int x, int y);
    static MOVES bishop(TEAM team, Board &board, int x, int y);
    static MOVES knight(TEAM team, Board &board, int x, int y);
    static MOVES rook(TEAM team, Board &board, int x, int y);
    static MOVES cannon(TEAM team, Board &board, int x, int y);
    static MOVES pawn(TEAM team, Board &board, int x, int y);
    static MOVES generateMovesOn(Board &board, int x, int y);
    static MOVES getMoves(Board &board);

    static MOVES kingCapture(TEAM team, Board &board, int x, int y);
    static MOVES guardCapture(TEAM team, Board &board, int x, int y);
    static MOVES bishopCapture(TEAM team, Board &board, int x, int y);
    static MOVES knightCapture(TEAM team, Board &board, int x, int y);
    static MOVES rookCapture(TEAM team, Board &board, int x, int y);
    static MOVES cannonCapture(TEAM team, Board &board, int x, int y);
    static MOVES pawnCapture(TEAM team, Board &board, int x, int y);
    static MOVES generateCaptureMovesOn(Board &board, int x, int y);
    static MOVES getCaptureMoves(Board &board);
};

MOVES MovesGenerate::king(TEAM team, Board &board, int x, int y)
{
    MOVES result;
    result.reserve(8);

    // 横坐标应当在3, 5之间, 纵坐标的话, 红方在0, 2之间, 黑方在7, 9之间
    const int left = x - 1;
    const int right = x + 1;
    const int up = y + 1;
    const int down = y - 1;

    if (left >= 3 && board.teamOn(left, y) != team)
    {
        result.emplace_back(Move{x, y, left, y});
    }
    if (right <= 5 && board.teamOn(right, y) != team)
    {
        result.emplace_back(Move{x, y, right, y});
    }
    if (team == RED)
    {
        if (up <= 2 && board.teamOn(x, up) != team)
        {
            result.emplace_back(Move{x, y, x, up});
        }
        if (down >= 0 && board.teamOn(x, down) != team)
        {
            result.emplace_back(Move{x, y, x, down});
        }
    }
    else
    {
        if (up <= 9 && board.teamOn(x, up) != team)
        {
            result.emplace_back(Move{x, y, x, up});
        }
        if (down >= 7 && board.teamOn(x, down) != team)
        {
            result.emplace_back(Move{x, y, x, down});
        }
    }

    return result;
}

MOVES MovesGenerate::guard(TEAM team, Board &board, int x, int y)
{
    MOVES result{};
    result.reserve(8);

    // 横坐标也应在3, 5之间, 纵坐标的话, 红方在0, 2之间, 黑方在7, 9之间
    const int left = x - 1;
    const int right = x + 1;
    const int up = y + 1;
    const int down = y - 1;
    if (left >= 3)
    {
        if (team == RED)
        {
            if (board.teamOn(left, up) != team && up <= 2)
            {
                result.emplace_back(Move{x, y, left, up});
            }
            if (board.teamOn(left, down) != team && down >= 0)
            {
                result.emplace_back(Move{x, y, left, down});
            }
        }
        else
        {
            if (board.teamOn(left, up) != team && up <= 9)
            {
                result.emplace_back(Move{x, y, left, up});
            }
            if (board.teamOn(left, down) != team && down >= 7)
            {
                result.emplace_back(Move{x, y, left, down});
            }
        }
    }
    if (right <= 5)
    {
        if (team == RED)
        {
            if (board.teamOn(right, up) != team && up <= 2)
            {
                result.emplace_back(Move{x, y, right, up});
            }
            if (board.teamOn(right, down) != team && down >= 0)
            {
                result.emplace_back(Move{x, y, right, down});
            }
        }
        else
        {
            if (board.teamOn(right, up) != team && up <= 9)
            {
                result.emplace_back(Move{x, y, right, up});
            }
            if (board.teamOn(right, down) != team && down >= 7)
            {
                result.emplace_back(Move{x, y, right, down});
            }
        }
    }

    return result;
}

MOVES MovesGenerate::bishop(TEAM team, Board &board, int x, int y)
{
    MOVES result{};
    result.reserve(8);

    // 横坐标应在0, 9之间, 纵坐标的话, 红方在0, 4之间, 黑方在5, 9之间
    if (team == RED)
    {
        if (board.teamOn(x - 1, y - 1) == EMPTY_TEAM && board.teamOn(x - 2, y - 2) != team)
        {
            result.emplace_back(Move{x, y, x - 2, y - 2});
        }
        if (board.teamOn(x + 1, y - 1) == EMPTY_TEAM && board.teamOn(x + 2, y - 2) != team)
        {
            result.emplace_back(Move{x, y, x + 2, y - 2});
        }
        if (board.teamOn(x - 1, y + 1) == EMPTY_TEAM && board.teamOn(x - 2, y + 2) != team && y + 1 <= 4)
        {
            result.emplace_back(Move{x, y, x - 2, y + 2});
        }
        if (board.teamOn(x + 1, y + 1) == EMPTY_TEAM && board.teamOn(x + 2, y + 2) != team && y + 1 <= 4)
        {
            result.emplace_back(Move{x, y, x + 2, y + 2});
        }
    }
    else
    {
        if (board.teamOn(x + 1, y + 1) == EMPTY_TEAM && board.teamOn(x + 2, y + 2) != team)
        {
            result.emplace_back(Move{x, y, x + 2, y + 2});
        }
        if (board.teamOn(x + 1, y - 1) == EMPTY_TEAM && board.teamOn(x + 2, y - 2) != team && y - 1 >= 5)
        {
            result.emplace_back(Move{x, y, x + 2, y - 2});
        }
        if (board.teamOn(x - 1, y + 1) == EMPTY_TEAM && board.teamOn(x - 2, y + 2) != team)
        {
            result.emplace_back(Move{x, y, x - 2, y + 2});
        }
        if (board.teamOn(x - 1, y - 1) == EMPTY_TEAM && board.teamOn(x - 2, y - 2) != team && y - 1 >= 5)
        {
            result.emplace_back(Move{x, y, x - 2, y - 2});
        }
    }

    return result;
}

MOVES MovesGenerate::knight(TEAM team, Board &board, int x, int y)
{
    MOVES result{};
    result.reserve(16);

    if (board.teamOn(x, y - 1) == EMPTY_TEAM)
    {
        TEAM t1 = board.teamOn(x - 1, y - 2);
        TEAM t2 = board.teamOn(x + 1, y - 2);
        if (t1 != team && t1 != OVERFLOW_TEAM)
        {
            result.emplace_back(Move{x, y, x - 1, y - 2});
        }
        if (t2 != team && t2 != OVERFLOW_TEAM)
        {
            result.emplace_back(Move{x, y, x + 1, y - 2});
        }
    }
    if (board.teamOn(x, y + 1) == EMPTY_TEAM)
    {
        TEAM t1 = board.teamOn(x - 1, y + 2);
        TEAM t2 = board.teamOn(x + 1, y + 2);
        if (t1 != team && t1 != OVERFLOW_TEAM)
        {
            result.emplace_back(Move{x, y, x - 1, y + 2});
        }
        if (t2 != team && t2 != OVERFLOW_TEAM)
        {
            result.emplace_back(Move{x, y, x + 1, y + 2});
        }
    }
    if (board.teamOn(x - 1, y) == EMPTY_TEAM)
    {
        TEAM t1 = board.teamOn(x - 2, y + 1);
        TEAM t2 = board.teamOn(x - 2, y - 1);
        if (t1 != team && t1 != OVERFLOW_TEAM)
        {
            result.emplace_back(Move{x, y, x - 2, y + 1});
        }
        if (t2 != team && t2 != OVERFLOW_TEAM)
        {
            result.emplace_back(Move{x, y, x - 2, y - 1});
        }
    }
    if (board.teamOn(x + 1, y) == EMPTY_TEAM)
    {
        TEAM t1 = board.teamOn(x + 2, y + 1);
        TEAM t2 = board.teamOn(x + 2, y - 1);
        if (t1 != team && t1 != OVERFLOW_TEAM)
        {
            result.emplace_back(Move{x, y, x + 2, y + 1});
        }
        if (t2 != team && t2 != OVERFLOW_TEAM)
        {
            result.emplace_back(Move{x, y, x + 2, y - 1});
        }
    }

    return result;
}

MOVES MovesGenerate::rook(TEAM team, Board &board, int x, int y)
{
    MOVES result{};
    result.reserve(64);

    // 纵向着法
    UINT32 bitlineX = board.getBitLineX(x);
    REGION_ROOK regionX = board.bitboard->getRookRegion(bitlineX, y, 9);
    for (int y2 = y + 1; y2 < regionX[1]; y2++)
    {
        result.emplace_back(Move{x, y, x, y2});
    }
    if (board.teamOn(x, regionX[1]) != team)
    {
        result.emplace_back(Move{x, y, x, regionX[1]});
    }
    for (int y2 = y - 1; y2 > regionX[0]; y2--)
    {
        result.emplace_back(Move{x, y, x, y2});
    }
    if (board.teamOn(x, regionX[0]) != team)
    {
        result.emplace_back(Move{x, y, x, regionX[0]});
    }

    // 横向着法
    UINT32 bitlineY = board.getBitLineY(y);
    REGION_ROOK regionY = board.bitboard->getRookRegion(bitlineY, x, 8);
    for (int x2 = x + 1; x2 < regionY[1]; x2++)
    {
        result.emplace_back(Move{x, y, x2, y});
    }
    if (board.teamOn(regionY[1], y) != team)
    {
        result.emplace_back(Move{x, y, regionY[1], y});
    }
    for (int x2 = x - 1; x2 > regionY[0]; x2--)
    {
        result.emplace_back(Move{x, y, x2, y});
    }
    if (board.teamOn(regionY[0], y) != team)
    {
        result.emplace_back(Move{x, y, regionY[0], y});
    }

    return result;
}

MOVES MovesGenerate::cannon(TEAM team, Board &board, int x, int y)
{
    MOVES result{};
    result.reserve(64);

    // 横向着法
    UINT32 bitlineY = board.getBitLineY(y);
    REGION_CANNON regionY = board.bitboard->getCannonRegion(bitlineY, x, 8);
    for (int x2 = x + 1; x2 <= regionY[2]; x2++)
    {
        result.emplace_back(Move{x, y, x2, y});
    }
    if (board.teamOn(regionY[3], y) == -team && regionY[3] != regionY[2])
    {
        result.emplace_back(Move{x, y, regionY[3], y});
    }
    for (int x2 = x - 1; x2 >= regionY[1]; x2--)
    {
        result.emplace_back(Move{x, y, x2, y});
    }
    if (board.teamOn(regionY[0], y) == -team && regionY[0] != regionY[1])
    {
        result.emplace_back(Move{x, y, regionY[0], y});
    }

    // 纵向着法
    UINT32 bitlineX = board.getBitLineX(x);
    REGION_CANNON regionX = board.bitboard->getCannonRegion(bitlineX, y, 9);
    for (int y2 = y + 1; y2 <= regionX[2]; y2++)
    {
        result.emplace_back(Move{x, y, x, y2});
    }
    if (board.teamOn(x, regionX[3]) == -team && regionX[3] != regionX[2])
    {
        result.emplace_back(Move{x, y, x, regionX[3]});
    }
    for (int y2 = y - 1; y2 >= regionX[1]; y2--)
    {
        result.emplace_back(Move{x, y, x, y2});
    }
    if (board.teamOn(x, regionX[0]) == -team && regionX[0] != regionX[1])
    {
        result.emplace_back(Move{x, y, x, regionX[0]});
    }

    return result;
}

MOVES MovesGenerate::pawn(TEAM team, Board &board, int x, int y)
{
    MOVES result{};
    result.reserve(8);

    if (team == RED)
    {
        if (board.teamOn(x, y + 1) != team && board.teamOn(x, y + 1) != OVERFLOW_TEAM)
        {
            result.emplace_back(Move{x, y, x, y + 1});
        }
        // 如果过河了
        if (y > 4)
        {
            if (board.teamOn(x - 1, y) != team && x - 1 >= 0)
            {
                result.emplace_back(Move{x, y, x - 1, y});
            }
            if (board.teamOn(x + 1, y) != team && x + 1 <= 8)
            {
                result.emplace_back(Move{x, y, x + 1, y});
            }
        }
    }
    else
    {
        if (board.teamOn(x, y - 1) != team && board.teamOn(x, y - 1) != OVERFLOW_TEAM)
        {
            result.emplace_back(Move{x, y, x, y - 1});
        }
        // 如果过河了
        if (y < 5)
        {
            if (board.teamOn(x - 1, y) != team && x - 1 >= 0)
            {
                result.emplace_back(Move{x, y, x - 1, y});
            }
            if (board.teamOn(x + 1, y) != team && x + 1 <= 8)
            {
                result.emplace_back(Move{x, y, x + 1, y});
            }
        }
    }

    return result;
}

MOVES MovesGenerate::generateMovesOn(Board &board, int x, int y)
{
    PIECEID pieceid = abs(board.pieceidOn(x, y));
    TEAM team = board.teamOn(x, y);

    if (pieceid == R_KING)
    {
        return MovesGenerate::king(team, board, x, y);
    }
    else if (pieceid == R_GUARD)
    {
        return MovesGenerate::guard(team, board, x, y);
    }
    else if (pieceid == R_BISHOP)
    {
        return MovesGenerate::bishop(team, board, x, y);
    }
    else if (pieceid == R_KNIGHT)
    {
        return MovesGenerate::knight(team, board, x, y);
    }
    else if (pieceid == R_ROOK)
    {
        return MovesGenerate::rook(team, board, x, y);
    }
    else if (pieceid == R_CANNON)
    {
        return MovesGenerate::cannon(team, board, x, y);
    }
    else if (pieceid == R_PAWN)
    {
        return MovesGenerate::pawn(team, board, x, y);
    }
    else
    {
        return MOVES{};
    }
}

MOVES MovesGenerate::getMoves(Board &board)
{
    // 对面笑
    const Piece &rKing = board.getPieceReg(board.team * R_KING);
    const Piece &bKing = board.getPieceReg(board.team * B_KING);
    if (rKing.x == bKing.x)
    {
        UINT32 bitlineX = board.getBitLineX(rKing.x);
        REGION_ROOK region = board.bitboard->getRookRegion(bitlineX, rKing.y, 9);
        if (region[1] == bKing.y)
        {
            MOVES result;
            if (board.team == RED)
            {
                result = MOVES{Move{rKing.x, rKing.y, bKing.x, bKing.y}};
                result[0].attacker = rKing;
            }
            else
            {
                result = MOVES{Move{bKing.x, bKing.y, rKing.x, rKing.y}};
                result[0].attacker = bKing;
            }
            return result;
        }
    }

    const std::map<PIECEID, int> weightPairs{
        {R_KING, 4},
        {R_ROOK, 4},
        {R_CANNON, 3},
        {R_KNIGHT, 3},
        {R_BISHOP, 2},
        {R_GUARD, 2},
        {R_PAWN, 1},
    };

    MOVES result{};
    result.reserve(64);

    for (const Piece &piece : board.getPiecesReg(board.team * R_ROOK))
    {
        std::vector<Move> moves = MovesGenerate::generateMovesOn(board, piece.x, piece.y);
        result.insert(result.end(), moves.begin(), moves.end());
    }
    for (const Piece &piece : board.getPiecesReg(board.team * R_PAWN))
    {
        std::vector<Move> moves = MovesGenerate::generateMovesOn(board, piece.x, piece.y);
        result.insert(result.end(), moves.begin(), moves.end());
    }
    for (const Piece &piece : board.getPiecesReg(board.team * R_CANNON))
    {
        std::vector<Move> moves = MovesGenerate::generateMovesOn(board, piece.x, piece.y);
        result.insert(result.end(), moves.begin(), moves.end());
    }
    for (const Piece &piece : board.getPiecesReg(board.team * R_KNIGHT))
    {
        std::vector<Move> moves = MovesGenerate::generateMovesOn(board, piece.x, piece.y);
        result.insert(result.end(), moves.begin(), moves.end());
    }
    for (const Piece &piece : board.getPiecesReg(board.team * R_BISHOP))
    {
        std::vector<Move> moves = MovesGenerate::generateMovesOn(board, piece.x, piece.y);
        result.insert(result.end(), moves.begin(), moves.end());
    }
    for (const Piece &piece : board.getPiecesReg(board.team * R_GUARD))
    {
        std::vector<Move> moves = MovesGenerate::generateMovesOn(board, piece.x, piece.y);
        result.insert(result.end(), moves.begin(), moves.end());
    }
    for (const Piece &piece : board.getPiecesReg(board.team * R_KING))
    {
        std::vector<Move> moves = MovesGenerate::generateMovesOn(board, piece.x, piece.y);
        result.insert(result.end(), moves.begin(), moves.end());
    }

    MOVES moves;
    for (Move move : result)
    {
        board.doMoveSimple(move);
        const bool skip = inCheck(board, -board.team);
        board.undoMoveSimple();
        if (!skip)
        {
            move.attacker = board.piecePosition(move.x1, move.y1);
            move.captured = board.piecePosition(move.x2, move.y2);
            if (move.captured.pieceid != EMPTY_PIECEID)
            {
                move.moveType = CAPTURE;
                move.val = weightPairs.at(abs(move.captured.pieceid)) - weightPairs.at(abs(move.attacker.pieceid));
            }
            moves.emplace_back(move);
        }
    }

    return moves;
}

MOVES MovesGenerate::kingCapture(TEAM team, Board &board, int x, int y)
{
    MOVES result{};
    result.reserve(8);

    // 横坐标应当在3, 5之间, 纵坐标的话, 红方在0, 2之间, 黑方在7, 9之间
    const int left = x - 1;
    const int right = x + 1;
    const int up = y + 1;
    const int down = y - 1;

    if (left >= 3 && board.teamOn(left, y) == -team)
    {
        result.emplace_back(Move{x, y, left, y});
    }
    if (right <= 5 && board.teamOn(right, y) == -team)
    {
        result.emplace_back(Move{x, y, right, y});
    }
    if (team == RED)
    {
        if (up <= 2 && board.teamOn(x, up) == -team)
        {
            result.emplace_back(Move{x, y, x, up});
        }
        if (down >= 0 && board.teamOn(x, down) == -team)
        {
            result.emplace_back(Move{x, y, x, down});
        }
    }
    else
    {
        if (up <= 9 && board.teamOn(x, up) == -team)
        {
            result.emplace_back(Move{x, y, x, up});
        }
        if (down >= 7 && board.teamOn(x, down) == -team)
        {
            result.emplace_back(Move{x, y, x, down});
        }
    }

    return result;
}

MOVES MovesGenerate::guardCapture(TEAM team, Board &board, int x, int y)
{
    MOVES result{};
    result.reserve(8);

    // 横坐标也应在3, 5之间, 纵坐标的话, 红方在0, 2之间, 黑方在7, 9之间
    const int left = x - 1;
    const int right = x + 1;
    const int up = y + 1;
    const int down = y - 1;
    if (left >= 3)
    {
        if (team == RED)
        {
            if (board.teamOn(left, up) == -team && up <= 2)
            {
                result.emplace_back(Move{x, y, left, up});
            }
            if (board.teamOn(left, down) == -team && down >= 0)
            {
                result.emplace_back(Move{x, y, left, down});
            }
        }
        else
        {
            if (board.teamOn(left, up) == -team && up <= 9)
            {
                result.emplace_back(Move{x, y, left, up});
            }
            if (board.teamOn(left, down) == -team && down >= 7)
            {
                result.emplace_back(Move{x, y, left, down});
            }
        }
    }
    if (right <= 5)
    {
        if (team == RED)
        {
            if (board.teamOn(right, up) == -team && up <= 2)
            {
                result.emplace_back(Move{x, y, right, up});
            }
            if (board.teamOn(right, down) == -team && down >= 0)
            {
                result.emplace_back(Move{x, y, right, down});
            }
        }
        else
        {
            if (board.teamOn(right, up) == -team && up <= 9)
            {
                result.emplace_back(Move{x, y, right, up});
            }
            if (board.teamOn(right, down) == -team && down >= 7)
            {
                result.emplace_back(Move{x, y, right, down});
            }
        }
    }

    return result;
}

MOVES MovesGenerate::bishopCapture(TEAM team, Board &board, int x, int y)
{
    MOVES result{};
    result.reserve(8);

    // 横坐标应在0, 9之间, 纵坐标的话, 红方在0, 4之间, 黑方在5, 9之间
    if (team == RED)
    {
        if (board.teamOn(x - 1, y - 1) == EMPTY_TEAM && board.teamOn(x - 2, y - 2) == -team)
        {
            result.emplace_back(Move{x, y, x - 2, y - 2});
        }
        if (board.teamOn(x + 1, y - 1) == EMPTY_TEAM && board.teamOn(x + 2, y - 2) == -team)
        {
            result.emplace_back(Move{x, y, x + 2, y - 2});
        }
        if (board.teamOn(x - 1, y + 1) == EMPTY_TEAM && board.teamOn(x - 2, y + 2) == -team && y + 1 <= 4)
        {
            result.emplace_back(Move{x, y, x - 2, y + 2});
        }
        if (board.teamOn(x + 1, y + 1) == EMPTY_TEAM && board.teamOn(x + 2, y + 2) == -team && y + 1 <= 4)
        {
            result.emplace_back(Move{x, y, x + 2, y + 2});
        }
    }
    else
    {
        if (board.teamOn(x + 1, y + 1) == EMPTY_TEAM && board.teamOn(x + 2, y + 2) == -team)
        {
            result.emplace_back(Move{x, y, x + 2, y + 2});
        }
        if (board.teamOn(x + 1, y - 1) == EMPTY_TEAM && board.teamOn(x + 2, y - 2) == -team && y - 1 >= 5)
        {
            result.emplace_back(Move{x, y, x + 2, y - 2});
        }
        if (board.teamOn(x - 1, y + 1) == EMPTY_TEAM && board.teamOn(x - 2, y + 2) == -team)
        {
            result.emplace_back(Move{x, y, x - 2, y + 2});
        }
        if (board.teamOn(x - 1, y - 1) == EMPTY_TEAM && board.teamOn(x - 2, y - 2) == -team && y - 1 >= 5)
        {
            result.emplace_back(Move{x, y, x - 2, y - 2});
        }
    }

    return result;
}

MOVES MovesGenerate::knightCapture(TEAM team, Board &board, int x, int y)
{
    MOVES result{};
    result.reserve(16);

    if (board.teamOn(x, y - 1) == EMPTY_TEAM)
    {
        TEAM t1 = board.teamOn(x - 1, y - 2);
        TEAM t2 = board.teamOn(x + 1, y - 2);
        if (t1 == -team && t1 != OVERFLOW_TEAM)
        {
            result.emplace_back(Move{x, y, x - 1, y - 2});
        }
        if (t2 == -team && t2 != OVERFLOW_TEAM)
        {
            result.emplace_back(Move{x, y, x + 1, y - 2});
        }
    }
    if (board.teamOn(x, y + 1) == EMPTY_TEAM)
    {
        TEAM t1 = board.teamOn(x - 1, y + 2);
        TEAM t2 = board.teamOn(x + 1, y + 2);
        if (t1 == -team && t1 != OVERFLOW_TEAM)
        {
            result.emplace_back(Move{x, y, x - 1, y + 2});
        }
        if (t2 == -team && t2 != OVERFLOW_TEAM)
        {
            result.emplace_back(Move{x, y, x + 1, y + 2});
        }
    }
    if (board.teamOn(x - 1, y) == EMPTY_TEAM)
    {
        TEAM t1 = board.teamOn(x - 2, y + 1);
        TEAM t2 = board.teamOn(x - 2, y - 1);
        if (t1 == -team && t1 != OVERFLOW_TEAM)
        {
            result.emplace_back(Move{x, y, x - 2, y + 1});
        }
        if (t2 == -team && t2 != OVERFLOW_TEAM)
        {
            result.emplace_back(Move{x, y, x - 2, y - 1});
        }
    }
    if (board.teamOn(x + 1, y) == EMPTY_TEAM)
    {
        TEAM t1 = board.teamOn(x + 2, y + 1);
        TEAM t2 = board.teamOn(x + 2, y - 1);
        if (t1 == -team && t1 != OVERFLOW_TEAM)
        {
            result.emplace_back(Move{x, y, x + 2, y + 1});
        }
        if (t2 == -team && t2 != OVERFLOW_TEAM)
        {
            result.emplace_back(Move{x, y, x + 2, y - 1});
        }
    }

    return result;
}

MOVES MovesGenerate::rookCapture(TEAM team, Board &board, int x, int y)
{
    MOVES result{};
    result.reserve(8);

    // 纵向着法
    UINT32 bitlineX = board.getBitLineX(x);
    REGION_ROOK regionX = board.bitboard->getRookRegion(bitlineX, y, 9);
    if (board.teamOn(x, regionX[1]) == -team)
    {
        result.emplace_back(Move{x, y, x, regionX[1]});
    }
    if (board.teamOn(x, regionX[0]) == -team)
    {
        result.emplace_back(Move{x, y, x, regionX[0]});
    }

    // 横向着法
    UINT32 bitlineY = board.getBitLineY(y);
    REGION_ROOK regionY = board.bitboard->getRookRegion(bitlineY, x, 8);
    if (board.teamOn(regionY[1], y) == -team)
    {
        result.emplace_back(Move{x, y, regionY[1], y});
    }
    if (board.teamOn(regionY[0], y) == -team)
    {
        result.emplace_back(Move{x, y, regionY[0], y});
    }

    return result;
}

MOVES MovesGenerate::cannonCapture(TEAM team, Board &board, int x, int y)
{
    MOVES result{};
    result.reserve(8);

    // 横向着法
    UINT32 bitlineY = board.getBitLineY(y);
    REGION_CANNON regionY = board.bitboard->getCannonRegion(bitlineY, x, 8);
    if (board.teamOn(regionY[3], y) == -team && regionY[3] != regionY[2])
    {
        result.emplace_back(Move{x, y, regionY[3], y});
    }
    if (board.teamOn(regionY[0], y) == -team && regionY[0] != regionY[1])
    {
        result.emplace_back(Move{x, y, regionY[0], y});
    }

    // 纵向着法
    UINT32 bitlineX = board.getBitLineX(x);
    REGION_CANNON regionX = board.bitboard->getCannonRegion(bitlineX, y, 9);
    if (board.teamOn(x, regionX[3]) == -team && regionX[3] != regionX[2])
    {
        result.emplace_back(Move{x, y, x, regionX[3]});
    }
    if (board.teamOn(x, regionX[0]) == -team && regionX[0] != regionX[1])
    {
        result.emplace_back(Move{x, y, x, regionX[0]});
    }

    return result;
}

MOVES MovesGenerate::pawnCapture(TEAM team, Board &board, int x, int y)
{
    MOVES result{};
    result.reserve(8);

    if (team == RED)
    {
        if (board.teamOn(x, y + 1) == -team && board.teamOn(x, y + 1) != OVERFLOW_TEAM)
        {
            result.emplace_back(Move{x, y, x, y + 1});
        }
        // 如果过河了
        if (y > 4)
        {
            if (board.teamOn(x - 1, y) == -team && x - 1 >= 0)
            {
                result.emplace_back(Move{x, y, x - 1, y});
            }
            if (board.teamOn(x + 1, y) == -team && x + 1 <= 8)
            {
                result.emplace_back(Move{x, y, x + 1, y});
            }
        }
    }
    else
    {
        if (board.teamOn(x, y - 1) == -team && board.teamOn(x, y - 1) != OVERFLOW_TEAM)
        {
            result.emplace_back(Move{x, y, x, y - 1});
        }
        // 如果过河了
        if (y < 5)
        {
            if (board.teamOn(x - 1, y) == -team && x - 1 >= 0)
            {
                result.emplace_back(Move{x, y, x - 1, y});
            }
            if (board.teamOn(x + 1, y) == -team && x + 1 <= 8)
            {
                result.emplace_back(Move{x, y, x + 1, y});
            }
        }
    }

    return result;
}

MOVES MovesGenerate::generateCaptureMovesOn(Board &board, int x, int y)
{
    PIECEID pieceid = board.pieceidOn(x, y);
    TEAM team = board.teamOn(x, y);

    if (pieceid == R_KING || pieceid == B_KING)
    {
        return MovesGenerate::kingCapture(team, board, x, y);
    }
    else if (pieceid == R_GUARD || pieceid == B_GUARD)
    {
        return MovesGenerate::guardCapture(team, board, x, y);
    }
    else if (pieceid == R_BISHOP || pieceid == B_BISHOP)
    {
        return MovesGenerate::bishopCapture(team, board, x, y);
    }
    else if (pieceid == R_KNIGHT || pieceid == B_KNIGHT)
    {
        return MovesGenerate::knightCapture(team, board, x, y);
    }
    else if (pieceid == R_ROOK || pieceid == B_ROOK)
    {
        return MovesGenerate::rookCapture(team, board, x, y);
    }
    else if (pieceid == R_CANNON || pieceid == B_CANNON)
    {
        return MovesGenerate::cannonCapture(team, board, x, y);
    }
    else if (pieceid == R_PAWN || pieceid == B_PAWN)
    {
        return MovesGenerate::pawnCapture(team, board, x, y);
    }
    else
    {
        return MOVES{};
    }
}

MOVES MovesGenerate::getCaptureMoves(Board &board)
{
    // 对面笑
    const Piece &rKing = board.getPieceReg(board.team * R_KING);
    const Piece &bKing = board.getPieceReg(board.team * B_KING);
    if (rKing.x == bKing.x)
    {
        UINT32 bitlineX = board.getBitLineX(rKing.x);
        REGION_ROOK region = board.bitboard->getRookRegion(bitlineX, rKing.y, 9);
        if (region[1] == bKing.y)
        {
            MOVES result;
            if (board.team == RED)
            {
                result = MOVES{Move{rKing.x, rKing.y, bKing.x, bKing.y}};
                result[0].attacker = rKing;
            }
            else
            {
                result = MOVES{Move{bKing.x, bKing.y, rKing.x, rKing.y}};
                result[0].attacker = bKing;
            }
            return result;
        }
    }

    MOVES result{};
    result.reserve(64);

    for (const Piece &piece : board.getPiecesReg(board.team * R_ROOK))
    {
        std::vector<Move> moves = MovesGenerate::generateCaptureMovesOn(board, piece.x, piece.y);
        result.insert(result.end(), moves.begin(), moves.end());
    }
    for (const Piece &piece : board.getPiecesReg(board.team * R_PAWN))
    {
        std::vector<Move> moves = MovesGenerate::generateCaptureMovesOn(board, piece.x, piece.y);
        result.insert(result.end(), moves.begin(), moves.end());
    }
    for (const Piece &piece : board.getPiecesReg(board.team * R_CANNON))
    {
        std::vector<Move> moves = MovesGenerate::generateCaptureMovesOn(board, piece.x, piece.y);
        result.insert(result.end(), moves.begin(), moves.end());
    }
    for (const Piece &piece : board.getPiecesReg(board.team * R_KNIGHT))
    {
        std::vector<Move> moves = MovesGenerate::generateCaptureMovesOn(board, piece.x, piece.y);
        result.insert(result.end(), moves.begin(), moves.end());
    }
    for (const Piece &piece : board.getPiecesReg(board.team * R_BISHOP))
    {
        std::vector<Move> moves = MovesGenerate::generateCaptureMovesOn(board, piece.x, piece.y);
        result.insert(result.end(), moves.begin(), moves.end());
    }
    for (const Piece &piece : board.getPiecesReg(board.team * R_GUARD))
    {
        std::vector<Move> moves = MovesGenerate::generateCaptureMovesOn(board, piece.x, piece.y);
        result.insert(result.end(), moves.begin(), moves.end());
    }
    for (const Piece &piece : board.getPiecesReg(board.team * R_KING))
    {
        std::vector<Move> moves = MovesGenerate::generateCaptureMovesOn(board, piece.x, piece.y);
        result.insert(result.end(), moves.begin(), moves.end());
    }

    MOVES moves;
    for (Move move : result)
    {
        board.doMoveSimple(move);
        const bool skip = inCheck(board, -board.team);
        board.undoMoveSimple();
        if (!skip)
        {
            move.attacker = board.piecePosition(move.x1, move.y1);
            move.captured = board.piecePosition(move.x2, move.y2);
            moves.emplace_back(move);
        }
    }

    return result;
}
