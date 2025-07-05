#pragma once
#include "board.hpp"
#include "utils.hpp"

class Moves
{
public:
    static MOVES king(TEAM team, Board &board, int x, int y);

    static MOVES guard(TEAM team, Board &board, int x, int y);

    static MOVES bishop(TEAM team, Board &board, int x, int y);

    static MOVES knight(TEAM team, Board &board, int x, int y);

    static MOVES rook(TEAM team, Board &board, int x, int y);

    static MOVES cannon(TEAM team, Board &board, int x, int y);

    static MOVES pawn(TEAM team, Board &board, int x, int y);

    static MOVES king_capture(TEAM team, Board &board, int x, int y);

    static MOVES guard_capture(TEAM team, Board &board, int x, int y);

    static MOVES bishop_capture(TEAM team, Board &board, int x, int y);

    static MOVES knight_capture(TEAM team, Board &board, int x, int y);

    static MOVES rook_capture(TEAM team, Board &board, int x, int y);

    static MOVES cannon_capture(TEAM team, Board &board, int x, int y);

    static MOVES pawn_capture(TEAM team, Board &board, int x, int y);

    static MOVES generateMoves(Board &board, int x, int y);

    static MOVES generateCaptureMoves(Board &board, int x, int y);

    static MOVES getCaptureMovesUnordered(Board &board);

    static MOVES getMoves(Board &board);

    static MOVES getCaptureMoves(Board &board);
};

MOVES Moves::king(TEAM team, Board &board, int x, int y)
{
    MOVES result{};
    result.reserve(8);

    // 横坐标应当在3, 5之间，纵坐标的话，红方在0, 2之间，黑方在7, 9之间
    const int left = x - 1;
    const int right = x + 1;
    const int up = y + 1;
    const int down = y - 1;

    if (left >= 3 && board.teamOn(left, y) != team)
        result.emplace_back(Move{x, y, left, y});
    if (right <= 5 && board.teamOn(right, y) != team)
        result.emplace_back(Move{x, y, right, y});
    if (team == RED)
    {
        if (up <= 2 && board.teamOn(x, up) != team)
            result.emplace_back(Move{x, y, x, up});
        if (down >= 0 && board.teamOn(x, down) != team)
            result.emplace_back(Move{x, y, x, down});
    }
    else
    {
        if (up <= 9 && board.teamOn(x, up) != team)
            result.emplace_back(Move{x, y, x, up});
        if (down >= 7 && board.teamOn(x, down) != team)
            result.emplace_back(Move{x, y, x, down});
    }

    return result;
}

MOVES Moves::guard(TEAM team, Board &board, int x, int y)
{
    MOVES result{};
    result.reserve(8);

    // 横坐标也应在3, 5之间，纵坐标的话，红方在0, 2之间，黑方在7, 9之间
    const int left = x - 1;
    const int right = x + 1;
    const int up = y + 1;
    const int down = y - 1;
    if (left >= 3)
    {
        if (team == RED)
        {
            if (board.teamOn(left, up) != team && up <= 2)
                result.emplace_back(Move{x, y, left, up});
            if (board.teamOn(left, down) != team && down >= 0)
                result.emplace_back(Move{x, y, left, down});
        }
        else
        {
            if (board.teamOn(left, up) != team && up <= 9)
                result.emplace_back(Move{x, y, left, up});
            if (board.teamOn(left, down) != team && down >= 7)
                result.emplace_back(Move{x, y, left, down});
        }
    }
    if (right <= 5)
    {
        if (team == RED)
        {
            if (board.teamOn(right, up) != team && up <= 2)
                result.emplace_back(Move{x, y, right, up});
            if (board.teamOn(right, down) != team && down >= 0)
                result.emplace_back(Move{x, y, right, down});
        }
        else
        {
            if (board.teamOn(right, up) != team && up <= 9)
                result.emplace_back(Move{x, y, right, up});
            if (board.teamOn(right, down) != team && down >= 7)
                result.emplace_back(Move{x, y, right, down});
        }
    }

    return result;
}

MOVES Moves::bishop(TEAM team, Board &board, int x, int y)
{
    MOVES result{};
    result.reserve(8);

    // 横坐标应在0, 9之间，纵坐标的话，红方在0, 4之间，黑方在5, 9之间
    if (team == RED)
    {
        if (board.teamOn(x - 1, y - 1) == EMPTY_TEAM && board.teamOn(x - 2, y - 2) != team)
            result.emplace_back(Move{x, y, x - 2, y - 2});
        if (board.teamOn(x + 1, y - 1) == EMPTY_TEAM && board.teamOn(x + 2, y - 2) != team)
            result.emplace_back(Move{x, y, x + 2, y - 2});
        if (board.teamOn(x - 1, y + 1) == EMPTY_TEAM && board.teamOn(x - 2, y + 2) != team && y + 1 <= 4)
            result.emplace_back(Move{x, y, x - 2, y + 2});
        if (board.teamOn(x + 1, y + 1) == EMPTY_TEAM && board.teamOn(x + 2, y + 2) != team && y + 1 <= 4)
            result.emplace_back(Move{x, y, x + 2, y + 2});
    }
    else
    {
        if (board.teamOn(x + 1, y + 1) == EMPTY_TEAM && board.teamOn(x + 2, y + 2) != team)
            result.emplace_back(Move{x, y, x + 2, y + 2});
        if (board.teamOn(x + 1, y - 1) == EMPTY_TEAM && board.teamOn(x + 2, y - 2) != team && y - 1 >= 5)
            result.emplace_back(Move{x, y, x + 2, y - 2});
        if (board.teamOn(x - 1, y + 1) == EMPTY_TEAM && board.teamOn(x - 2, y + 2) != team)
            result.emplace_back(Move{x, y, x - 2, y + 2});
        if (board.teamOn(x - 1, y - 1) == EMPTY_TEAM && board.teamOn(x - 2, y - 2) != team && y - 1 >= 5)
            result.emplace_back(Move{x, y, x - 2, y - 2});
    }

    return result;
}

MOVES Moves::knight(TEAM team, Board &board, int x, int y)
{
    MOVES result{};
    result.reserve(16);

    if (board.teamOn(x, y - 1) == EMPTY_TEAM)
    {
        TEAM t1 = board.teamOn(x - 1, y - 2);
        TEAM t2 = board.teamOn(x + 1, y - 2);
        if (t1 != team && t1 != OVERFLOW_TEAM)
            result.emplace_back(Move{x, y, x - 1, y - 2});
        if (t2 != team && t2 != OVERFLOW_TEAM)
            result.emplace_back(Move{x, y, x + 1, y - 2});
    }
    if (board.teamOn(x, y + 1) == EMPTY_TEAM)
    {
        TEAM t1 = board.teamOn(x - 1, y + 2);
        TEAM t2 = board.teamOn(x + 1, y + 2);
        if (t1 != team && t1 != OVERFLOW_TEAM)
            result.emplace_back(Move{x, y, x - 1, y + 2});
        if (t2 != team && t2 != OVERFLOW_TEAM)
            result.emplace_back(Move{x, y, x + 1, y + 2});
    }
    if (board.teamOn(x - 1, y) == EMPTY_TEAM)
    {
        TEAM t1 = board.teamOn(x - 2, y + 1);
        TEAM t2 = board.teamOn(x - 2, y - 1);
        if (t1 != team && t1 != OVERFLOW_TEAM)
            result.emplace_back(Move{x, y, x - 2, y + 1});
        if (t2 != team && t2 != OVERFLOW_TEAM)
            result.emplace_back(Move{x, y, x - 2, y - 1});
    }
    if (board.teamOn(x + 1, y) == EMPTY_TEAM)
    {
        TEAM t1 = board.teamOn(x + 2, y + 1);
        TEAM t2 = board.teamOn(x + 2, y - 1);
        if (t1 != team && t1 != OVERFLOW_TEAM)
            result.emplace_back(Move{x, y, x + 2, y + 1});
        if (t2 != team && t2 != OVERFLOW_TEAM)
            result.emplace_back(Move{x, y, x + 2, y - 1});
    }

    return result;
}

MOVES Moves::rook(TEAM team, Board &board, int x, int y)
{
    MOVES result{};
    result.reserve(64);

    // 纵向着法
    BITLINE bitlineX = board.getBitLineX(x);
    REGION_ROOK regionX = board.bitboard->getRookRegion(bitlineX, y, 9);
    for (int y2 = y + 1; y2 < regionX[1]; y2++)
        result.emplace_back(Move{x, y, x, y2});
    if (board.teamOn(x, regionX[1]) != team)
        result.emplace_back(Move{x, y, x, regionX[1]});
    for (int y2 = y - 1; y2 > regionX[0]; y2--)
        result.emplace_back(Move{x, y, x, y2});
    if (board.teamOn(x, regionX[0]) != team)
        result.emplace_back(Move{x, y, x, regionX[0]});

    // 横向着法
    BITLINE bitlineY = board.getBitLineY(y);
    REGION_ROOK regionY = board.bitboard->getRookRegion(bitlineY, x, 8);
    for (int x2 = x + 1; x2 < regionY[1]; x2++)
        result.emplace_back(Move{x, y, x2, y});
    if (board.teamOn(regionY[1], y) != team)
        result.emplace_back(Move{x, y, regionY[1], y});
    for (int x2 = x - 1; x2 > regionY[0]; x2--)
        result.emplace_back(Move{x, y, x2, y});
    if (board.teamOn(regionY[0], y) != team)
        result.emplace_back(Move{x, y, regionY[0], y});

    return result;
}

MOVES Moves::cannon(TEAM team, Board &board, int x, int y)
{
    MOVES result{};
    result.reserve(64);

    // 横向着法
    BITLINE bitlineY = board.getBitLineY(y);
    REGION_CANNON regionY = board.bitboard->getCannonRegion(bitlineY, x, 8);
    for (int x2 = x + 1; x2 <= regionY[2]; x2++)
        result.emplace_back(Move{x, y, x2, y});
    if (board.teamOn(regionY[3], y) == -team && regionY[3] != regionY[2])
        result.emplace_back(Move{x, y, regionY[3], y});
    for (int x2 = x - 1; x2 >= regionY[1]; x2--)
        result.emplace_back(Move{x, y, x2, y});
    if (board.teamOn(regionY[0], y) == -team && regionY[0] != regionY[1])
        result.emplace_back(Move{x, y, regionY[0], y});

    // 纵向着法
    BITLINE bitlineX = board.getBitLineX(x);
    REGION_CANNON regionX = board.bitboard->getCannonRegion(bitlineX, y, 9);
    for (int y2 = y + 1; y2 <= regionX[2]; y2++)
        result.emplace_back(Move{x, y, x, y2});
    if (board.teamOn(x, regionX[3]) == -team && regionX[3] != regionX[2])
        result.emplace_back(Move{x, y, x, regionX[3]});
    for (int y2 = y - 1; y2 >= regionX[1]; y2--)
        result.emplace_back(Move{x, y, x, y2});
    if (board.teamOn(x, regionX[0]) == -team && regionX[0] != regionX[1])
        result.emplace_back(Move{x, y, x, regionX[0]});

    return result;
}

MOVES Moves::pawn(TEAM team, Board &board, int x, int y)
{
    MOVES result{};
    result.reserve(8);

    if (team == RED)
    {
        if (board.teamOn(x, y + 1) != team && board.teamOn(x, y + 1) != OVERFLOW_TEAM)
            result.emplace_back(Move{x, y, x, y + 1});
        // 如果过河了
        if (y > 4)
        {
            if (board.teamOn(x - 1, y) != team && x - 1 >= 0)
                result.emplace_back(Move{x, y, x - 1, y});
            if (board.teamOn(x + 1, y) != team && x + 1 <= 8)
                result.emplace_back(Move{x, y, x + 1, y});
        }
    }
    else
    {
        if (board.teamOn(x, y - 1) != team && board.teamOn(x, y - 1) != OVERFLOW_TEAM)
            result.emplace_back(Move{x, y, x, y - 1});
        // 如果过河了
        if (y < 5)
        {
            if (board.teamOn(x - 1, y) != team && x - 1 >= 0)
                result.emplace_back(Move{x, y, x - 1, y});
            if (board.teamOn(x + 1, y) != team && x + 1 <= 8)
                result.emplace_back(Move{x, y, x + 1, y});
        }
    }

    return result;
}

MOVES Moves::king_capture(TEAM team, Board &board, int x, int y)
{
    MOVES result{};
    result.reserve(8);

    // 横坐标应当在3, 5之间，纵坐标的话，红方在0, 2之间，黑方在7, 9之间
    const int left = x - 1;
    const int right = x + 1;
    const int up = y + 1;
    const int down = y - 1;

    if (left >= 3 && board.teamOn(left, y) == -team)
        result.emplace_back(Move{x, y, left, y});
    if (right <= 5 && board.teamOn(right, y) == -team)
        result.emplace_back(Move{x, y, right, y});
    if (team == RED)
    {
        if (up <= 2 && board.teamOn(x, up) == -team)
            result.emplace_back(Move{x, y, x, up});
        if (down >= 0 && board.teamOn(x, down) == -team)
            result.emplace_back(Move{x, y, x, down});
    }
    else
    {
        if (up <= 9 && board.teamOn(x, up) == -team)
            result.emplace_back(Move{x, y, x, up});
        if (down >= 7 && board.teamOn(x, down) == -team)
            result.emplace_back(Move{x, y, x, down});
    }

    return result;
}

MOVES Moves::guard_capture(TEAM team, Board &board, int x, int y)
{
    MOVES result{};
    result.reserve(8);

    // 横坐标也应在3, 5之间，纵坐标的话，红方在0, 2之间，黑方在7, 9之间
    const int left = x - 1;
    const int right = x + 1;
    const int up = y + 1;
    const int down = y - 1;
    if (left >= 3)
    {
        if (team == RED)
        {
            if (board.teamOn(left, up) == -team && up <= 2)
                result.emplace_back(Move{x, y, left, up});
            if (board.teamOn(left, down) == -team && down >= 0)
                result.emplace_back(Move{x, y, left, down});
        }
        else
        {
            if (board.teamOn(left, up) == -team && up <= 9)
                result.emplace_back(Move{x, y, left, up});
            if (board.teamOn(left, down) == -team && down >= 7)
                result.emplace_back(Move{x, y, left, down});
        }
    }
    if (right <= 5)
    {
        if (team == RED)
        {
            if (board.teamOn(right, up) == -team && up <= 2)
                result.emplace_back(Move{x, y, right, up});
            if (board.teamOn(right, down) == -team && down >= 0)
                result.emplace_back(Move{x, y, right, down});
        }
        else
        {
            if (board.teamOn(right, up) == -team && up <= 9)
                result.emplace_back(Move{x, y, right, up});
            if (board.teamOn(right, down) == -team && down >= 7)
                result.emplace_back(Move{x, y, right, down});
        }
    }

    return result;
}

MOVES Moves::bishop_capture(TEAM team, Board &board, int x, int y)
{
    MOVES result{};
    result.reserve(8);

    // 横坐标应在0, 9之间，纵坐标的话，红方在0, 4之间，黑方在5, 9之间
    if (team == RED)
    {
        if (board.teamOn(x - 1, y - 1) == EMPTY_TEAM && board.teamOn(x - 2, y - 2) == -team)
            result.emplace_back(Move{x, y, x - 2, y - 2});
        if (board.teamOn(x + 1, y - 1) == EMPTY_TEAM && board.teamOn(x + 2, y - 2) == -team)
            result.emplace_back(Move{x, y, x + 2, y - 2});
        if (board.teamOn(x - 1, y + 1) == EMPTY_TEAM && board.teamOn(x - 2, y + 2) == -team && y + 1 <= 4)
            result.emplace_back(Move{x, y, x - 2, y + 2});
        if (board.teamOn(x + 1, y + 1) == EMPTY_TEAM && board.teamOn(x + 2, y + 2) == -team && y + 1 <= 4)
            result.emplace_back(Move{x, y, x + 2, y + 2});
    }
    else
    {
        if (board.teamOn(x + 1, y + 1) == EMPTY_TEAM && board.teamOn(x + 2, y + 2) == -team)
            result.emplace_back(Move{x, y, x + 2, y + 2});
        if (board.teamOn(x + 1, y - 1) == EMPTY_TEAM && board.teamOn(x + 2, y - 2) == -team && y - 1 >= 5)
            result.emplace_back(Move{x, y, x + 2, y - 2});
        if (board.teamOn(x - 1, y + 1) == EMPTY_TEAM && board.teamOn(x - 2, y + 2) == -team)
            result.emplace_back(Move{x, y, x - 2, y + 2});
        if (board.teamOn(x - 1, y - 1) == EMPTY_TEAM && board.teamOn(x - 2, y - 2) == -team && y - 1 >= 5)
            result.emplace_back(Move{x, y, x - 2, y - 2});
    }

    return result;
}

MOVES Moves::knight_capture(TEAM team, Board &board, int x, int y)
{
    MOVES result{};
    result.reserve(16);

    if (board.teamOn(x, y - 1) == EMPTY_TEAM)
    {
        TEAM t1 = board.teamOn(x - 1, y - 2);
        TEAM t2 = board.teamOn(x + 1, y - 2);
        if (t1 == -team && t1 != OVERFLOW_TEAM)
            result.emplace_back(Move{x, y, x - 1, y - 2});
        if (t2 == -team && t2 != OVERFLOW_TEAM)
            result.emplace_back(Move{x, y, x + 1, y - 2});
    }
    if (board.teamOn(x, y + 1) == EMPTY_TEAM)
    {
        TEAM t1 = board.teamOn(x - 1, y + 2);
        TEAM t2 = board.teamOn(x + 1, y + 2);
        if (t1 == -team && t1 != OVERFLOW_TEAM)
            result.emplace_back(Move{x, y, x - 1, y + 2});
        if (t2 == -team && t2 != OVERFLOW_TEAM)
            result.emplace_back(Move{x, y, x + 1, y + 2});
    }
    if (board.teamOn(x - 1, y) == EMPTY_TEAM)
    {
        TEAM t1 = board.teamOn(x - 2, y + 1);
        TEAM t2 = board.teamOn(x - 2, y - 1);
        if (t1 == -team && t1 != OVERFLOW_TEAM)
            result.emplace_back(Move{x, y, x - 2, y + 1});
        if (t2 == -team && t2 != OVERFLOW_TEAM)
            result.emplace_back(Move{x, y, x - 2, y - 1});
    }
    if (board.teamOn(x + 1, y) == EMPTY_TEAM)
    {
        TEAM t1 = board.teamOn(x + 2, y + 1);
        TEAM t2 = board.teamOn(x + 2, y - 1);
        if (t1 == -team && t1 != OVERFLOW_TEAM)
            result.emplace_back(Move{x, y, x + 2, y + 1});
        if (t2 == -team && t2 != OVERFLOW_TEAM)
            result.emplace_back(Move{x, y, x + 2, y - 1});
    }

    return result;
}

MOVES Moves::rook_capture(TEAM team, Board &board, int x, int y)
{
    MOVES result{};
    result.reserve(8);

    // 纵向着法
    BITLINE bitlineX = board.getBitLineX(x);
    REGION_ROOK regionX = board.bitboard->getRookRegion(bitlineX, y, 9);
    if (board.teamOn(x, regionX[1]) == -team)
        result.emplace_back(Move{x, y, x, regionX[1]});
    if (board.teamOn(x, regionX[0]) == -team)
        result.emplace_back(Move{x, y, x, regionX[0]});

    // 横向着法
    BITLINE bitlineY = board.getBitLineY(y);
    REGION_ROOK regionY = board.bitboard->getRookRegion(bitlineY, x, 8);
    if (board.teamOn(regionY[1], y) == -team)
        result.emplace_back(Move{x, y, regionY[1], y});
    if (board.teamOn(regionY[0], y) == -team)
        result.emplace_back(Move{x, y, regionY[0], y});

    return result;
}

MOVES Moves::cannon_capture(TEAM team, Board &board, int x, int y)
{
    MOVES result{};
    result.reserve(8);

    // 横向着法
    BITLINE bitlineY = board.getBitLineY(y);
    REGION_CANNON regionY = board.bitboard->getCannonRegion(bitlineY, x, 8);
    if (board.teamOn(regionY[3], y) == -team && regionY[3] != regionY[2])
        result.emplace_back(Move{x, y, regionY[3], y});
    if (board.teamOn(regionY[0], y) == -team && regionY[0] != regionY[1])
        result.emplace_back(Move{x, y, regionY[0], y});

    // 纵向着法
    BITLINE bitlineX = board.getBitLineX(x);
    REGION_CANNON regionX = board.bitboard->getCannonRegion(bitlineX, y, 9);
    if (board.teamOn(x, regionX[3]) == -team && regionX[3] != regionX[2])
        result.emplace_back(Move{x, y, x, regionX[3]});
    if (board.teamOn(x, regionX[0]) == -team && regionX[0] != regionX[1])
        result.emplace_back(Move{x, y, x, regionX[0]});

    return result;
}

MOVES Moves::pawn_capture(TEAM team, Board &board, int x, int y)
{
    MOVES result{};
    result.reserve(8);

    if (team == RED)
    {
        if (board.teamOn(x, y + 1) == -team && board.teamOn(x, y + 1) != OVERFLOW_TEAM)
            result.emplace_back(Move{x, y, x, y + 1});
        // 如果过河了
        if (y > 4)
        {
            if (board.teamOn(x - 1, y) == -team && x - 1 >= 0)
                result.emplace_back(Move{x, y, x - 1, y});
            if (board.teamOn(x + 1, y) == -team && x + 1 <= 8)
                result.emplace_back(Move{x, y, x + 1, y});
        }
    }
    else
    {
        if (board.teamOn(x, y - 1) == -team && board.teamOn(x, y - 1) != OVERFLOW_TEAM)
            result.emplace_back(Move{x, y, x, y - 1});
        // 如果过河了
        if (y < 5)
        {
            if (board.teamOn(x - 1, y) == -team && x - 1 >= 0)
                result.emplace_back(Move{x, y, x - 1, y});
            if (board.teamOn(x + 1, y) == -team && x + 1 <= 8)
                result.emplace_back(Move{x, y, x + 1, y});
        }
    }

    return result;
}

MOVES Moves::generateMoves(Board &board, int x, int y)
{
    PIECEID pieceid = board.pieceidOn(x, y);
    TEAM team = board.teamOn(x, y);

    if (pieceid == R_KING || pieceid == B_KING)
        return Moves::king(team, board, x, y);
    else if (pieceid == R_GUARD || pieceid == B_GUARD)
        return Moves::guard(team, board, x, y);
    else if (pieceid == R_BISHOP || pieceid == B_BISHOP)
        return Moves::bishop(team, board, x, y);
    else if (pieceid == R_KNIGHT || pieceid == B_KNIGHT)
        return Moves::knight(team, board, x, y);
    else if (pieceid == R_ROOK || pieceid == B_ROOK)
        return Moves::rook(team, board, x, y);
    else if (pieceid == R_CANNON || pieceid == B_CANNON)
        return Moves::cannon(team, board, x, y);
    else if (pieceid == R_PAWN || pieceid == B_PAWN)
        return Moves::pawn(team, board, x, y);
    else
        return MOVES{};
}

MOVES Moves::generateCaptureMoves(Board &board, int x, int y)
{
    PIECEID pieceid = board.pieceidOn(x, y);
    TEAM team = board.teamOn(x, y);

    if (pieceid == R_KING || pieceid == B_KING)
        return Moves::king_capture(team, board, x, y);
    else if (pieceid == R_GUARD || pieceid == B_GUARD)
        return Moves::guard_capture(team, board, x, y);
    else if (pieceid == R_BISHOP || pieceid == B_BISHOP)
        return Moves::bishop_capture(team, board, x, y);
    else if (pieceid == R_KNIGHT || pieceid == B_KNIGHT)
        return Moves::knight_capture(team, board, x, y);
    else if (pieceid == R_ROOK || pieceid == B_ROOK)
        return Moves::rook_capture(team, board, x, y);
    else if (pieceid == R_CANNON || pieceid == B_CANNON)
        return Moves::cannon_capture(team, board, x, y);
    else if (pieceid == R_PAWN || pieceid == B_PAWN)
        return Moves::pawn_capture(team, board, x, y);
    else
        return MOVES{};
}

MOVES Moves::getCaptureMovesUnordered(Board &board)
{
    // 对面笑
    for (int y = board.pieceRedKing->y + 1; y <= 9; y++)
    {
        if (board.pieceidOn(board.pieceRedKing->x, y) == B_KING)
        {
            if (board.team == RED)
                return MOVES{Move{board.pieceRedKing->x, board.pieceRedKing->y, board.pieceBlackKing->x,
                                  board.pieceBlackKing->y}};
            else
                return MOVES{Move{board.pieceBlackKing->x, board.pieceBlackKing->y, board.pieceRedKing->x,
                                  board.pieceRedKing->y}};
        }
        if (board.teamOn(board.pieceRedKing->x, y) != EMPTY_TEAM)
            break;
    }

    MOVES result{};
    result.reserve(64);

    PIECES pieces = board.getPiecesByTeam(board.team);
    for (const Piece &piece : pieces)
    {
        std::vector<Move> moves = Moves::generateCaptureMoves(board, piece.x, piece.y);
        for (Move move : moves)
            result.emplace_back(move);
    }

    return result;
}

MOVES Moves::getMoves(Board &board)
{
    // 对面笑
    for (int y = board.pieceRedKing->y + 1; y <= 9; y++)
    {
        if (board.pieceidOn(board.pieceRedKing->x, y) == B_KING)
        {
            if (board.team == RED)
            {
                return MOVES{Move{board.pieceRedKing->x, board.pieceRedKing->y, board.pieceBlackKing->x,
                                  board.pieceBlackKing->y}};
            }
            else
            {
                return MOVES{Move{board.pieceBlackKing->x, board.pieceBlackKing->y, board.pieceRedKing->x,
                                  board.pieceRedKing->y}};
            }
        }
        if (board.teamOn(board.pieceRedKing->x, y) != EMPTY_TEAM)
        {
            break;
        }
    }

    MOVES result{};
    result.reserve(64);

    PIECES pieces = board.getPiecesByTeam(board.team);
    for (const Piece &piece : pieces)
    {
        std::vector<Move> moves = Moves::generateMoves(board, piece.x, piece.y);
        for (Move move : moves)
        {
            move.attacker = board.piecePosition(move.x1, move.y1);
            move.captured = board.piecePosition(move.x2, move.y2);
            if (move.captured.pieceid != EMPTY_PIECEID)
            {
                move.moveType = CAPTURE;
            }
            result.emplace_back(move);
        }
    }

    return result;
}

MOVES Moves::getCaptureMoves(Board &board)
{
    MOVES moves = Moves::getCaptureMovesUnordered(board);

    MOVES result{};
    result.reserve(64);

    const std::map<PIECEID, int> weightPairs{
        {R_KING, 4},
        {R_ROOK, 4},
        {R_CANNON, 3},
        {R_KNIGHT, 3},
        {R_BISHOP, 2},
        {R_GUARD, 2},
        {R_PAWN, 1},
    };
    std::array<std::vector<Move>, 9> orderMap{};

    for (const Move &move : moves)
    {
        int score = 0;

        Piece attacker = board.piecePosition(move.x1, move.y1);
        Piece captured = board.piecePosition(move.x2, move.y2);
        int a = weightPairs.at(abs(captured.pieceid));
        int b = weightPairs.at(abs(attacker.pieceid));
        if (hasProtector(board, captured.x, captured.y))
        {
            score = a - b + 1;

            if (score < 1)
            {
                PIECEID pieceid = board.pieceidOn(captured.x, captured.y);
                if (pieceid == R_KNIGHT || pieceid == R_CANNON || pieceid == R_ROOK ||
                    isRiveredPawn(board, captured.x, captured.y))
                {
                    score = 1;
                }
            }
        }
        else
        {
            score = a + 1;
        }
        if (score >= 1)
        {
            orderMap[score].emplace_back(move);
        }
    }

    for (int score = 8; score >= 1; score--)
    {
        for (Move &move : orderMap[score])
        {
            move.attacker = board.piecePosition(move.x1, move.y1);
            move.captured = board.piecePosition(move.x2, move.y2);
            result.emplace_back(move);
        }
    }

    return result;
}
