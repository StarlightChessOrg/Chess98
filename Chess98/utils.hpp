#pragma once
#include "board.hpp"

/// @brief 是否被将军
/// @param board
/// @return
bool inCheck(Board &board)
{
    Piece *king = board.team == RED ? board.pieceRedKing : board.pieceBlackKing;
    int x = king->x;
    int y = king->y;
    int team = king->team();

    // 判断敌方的兵是否在附近
    bool c1 = abs(board.pieceidOn(king->x + 1, king->y)) == R_PAWN;
    bool c2 = abs(board.pieceidOn(king->x - 1, king->y)) == R_PAWN;
    bool c3 = abs(board.pieceidOn(king->x, (king->team() == RED ? king->y - 1 : king->y + 1))) == R_PAWN;
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
    // 横向着法
    BITLINE bitlineY = board.getBitLineY(king->y);
    REGION_CANNON regionY = board.bitboard->getCannonRegion(bitlineY, king->x, 8);
    if ((abs(board.pieceidOn(regionY[1] - 1, y)) == R_ROOK || abs(board.pieceidOn(regionY[1] - 1, y)) == R_KING) &&
        board.teamOn(regionY[1] - 1, y) != king->team())
        return true;
    if ((abs(board.pieceidOn(regionY[2] + 1, y)) == R_ROOK || abs(board.pieceidOn(regionY[2] + 1, y)) == R_KING) &&
        board.teamOn(regionY[2] + 1, y) != king->team())
        return true;
    if (abs(board.pieceidOn(regionY[0], y)) == R_CANNON && board.teamOn(regionY[0], y) != king->team())
        return true;
    if (abs(board.pieceidOn(regionY[3], y)) == R_CANNON && board.teamOn(regionY[3], y) != king->team())
        return true;

    // 纵向着法
    BITLINE bitlineX = board.getBitLineX(x);
    REGION_CANNON regionX = board.bitboard->getCannonRegion(bitlineX, y, 9);
    if ((abs(board.pieceidOn(x, regionX[1] - 1)) == R_ROOK || abs(board.pieceidOn(x, regionX[1] - 1)) == R_KING) &&
        board.teamOn(x, regionX[1] - 1) != team)
        return true;
    if ((abs(board.pieceidOn(x, regionX[2] + 1)) == R_ROOK || abs(board.pieceidOn(x, regionX[2] + 1)) == R_KING) &&
        board.teamOn(x, regionX[2] + 1) != team)
        return true;
    if (abs(board.pieceidOn(x, regionX[0])) == R_CANNON && board.teamOn(x, regionX[0]) != team)
        return true;
    if (abs(board.pieceidOn(x, regionX[3])) == R_CANNON && board.teamOn(x, regionX[3]) != team)
        return true;

    return false;
}

/// @brief 是否有保护
/// @param board
/// @param x
/// @param y
/// @return
bool hasProtector(Board &board, int x, int y)
{
    TEAM team = -board.teamOn(x, y);

    // 兵、将
    if ((abs(board.pieceidOn(x + 1, y)) == R_PAWN || abs(board.pieceidOn(x + 1, y) == R_KING)) &&
        board.teamOn(x + 1, y) != team)
        return true;
    if ((abs(board.pieceidOn(x - 1, y)) == R_PAWN || abs(board.pieceidOn(x - 1, y) == R_KING)) &&
        board.teamOn(x - 1, y) != team)
        return true;
    if (abs(board.pieceidOn(x, (team == RED) ? y - 1 : y + 1)) == R_PAWN &&
        board.teamOn(x, (team == RED) ? y - 1 : y + 1) != team)
        return true;
    if (abs(board.pieceidOn(x, y + 1)) == R_KING && board.teamOn(x, y + 1) != team)
        return true;
    if (abs(board.pieceidOn(x, y - 1)) == R_KING && board.teamOn(x, y - 1) != team)
        return true;

    // 马
    if (board.pieceidOn(x + 1, y) == 0)
    {
        if (abs(board.pieceidOn(x + 2, y + 1)) == R_KNIGHT && board.teamOn(x + 2, y + 1) != team)
            return true;
        if (abs(board.pieceidOn(x + 2, y - 1)) == R_KNIGHT && board.teamOn(x + 2, y - 1) != team)
            return true;
    }
    if (board.pieceidOn(x - 1, y) == 0)
    {
        if (abs(board.pieceidOn(x - 2, y + 1)) == R_KNIGHT && board.teamOn(x - 2, y + 1) != team)
            return true;
        if (abs(board.pieceidOn(x - 2, y - 1)) == R_KNIGHT && board.teamOn(x - 2, y - 1) != team)
            return true;
    }
    if (board.pieceidOn(x, y + 1) == 0)
    {
        if (abs(board.pieceidOn(x + 1, y + 2)) == R_KNIGHT && board.teamOn(x + 1, y + 2) != team)
            return true;
        if (abs(board.pieceidOn(x - 1, y + 2)) == R_KNIGHT && board.teamOn(x - 1, y + 2) != team)
            return true;
    }
    if (board.pieceidOn(x, y - 1) == 0)
    {
        if (abs(board.pieceidOn(x + 1, y - 2)) == R_KNIGHT && board.teamOn(x + 1, y - 2) != team)
            return true;
        if (abs(board.pieceidOn(x - 1, y - 2)) == R_KNIGHT && board.teamOn(x - 1, y - 2) != team)
            return true;
    }

    // 士、象
    if (board.pieceidOn(x + 1, y + 1) == 0)
    {
        if (abs(board.pieceidOn(x + 2, y + 2)) == R_BISHOP && board.teamOn(x + 2, y + 2) != team)
            return true;
    }
    if (board.pieceidOn(x - 1, y + 1) == 0)
    {
        if (abs(board.pieceidOn(x - 2, y + 2)) == R_BISHOP && board.teamOn(x - 2, y + 2) != team)
            return true;
    }
    if (board.pieceidOn(x + 1, y - 1) == 0)
    {
        if (abs(board.pieceidOn(x + 2, y - 2)) == R_BISHOP && board.teamOn(x + 2, y - 2) != team)
            return true;
    }
    if (board.pieceidOn(x - 1, y - 1) == 0)
    {
        if (abs(board.pieceidOn(x - 2, y - 2)) == R_BISHOP && board.teamOn(x - 2, y - 2) != team)
            return true;
    }

    if (abs(board.pieceidOn(x + 1, y + 1)) == R_GUARD && board.teamOn(x + 1, y + 1) != team)
        return true;
    if (abs(board.pieceidOn(x - 1, y + 1)) == R_GUARD && board.teamOn(x - 1, y + 1) != team)
        return true;
    if (abs(board.pieceidOn(x + 1, y - 1)) == R_GUARD && board.teamOn(x + 1, y - 1) != team)
        return true;
    if (abs(board.pieceidOn(x - 1, y - 1)) == R_GUARD && board.teamOn(x - 1, y - 1) != team)
        return true;

    // 车、炮
    BITLINE bitlineY = board.getBitLineY(y);
    REGION_CANNON regionY = board.bitboard->getCannonRegion(bitlineY, x, 8);
    if (abs(board.pieceidOn(regionY[1] - 1, y)) == R_ROOK && board.teamOn(regionY[1] - 1, y) != team)
        return true;
    if (abs(board.pieceidOn(regionY[2] + 1, y)) == R_ROOK && board.teamOn(regionY[2] + 1, y) != team)
        return true;
    if (abs(board.pieceidOn(regionY[0], y)) == R_CANNON && regionY[0] != x && board.teamOn(regionY[0], y) != team)
        return true;
    if (abs(board.pieceidOn(regionY[3], y)) == R_CANNON && regionY[3] != x && board.teamOn(regionY[3], y) != team)
        return true;

    // 纵向着法
    BITLINE bitlineX = board.getBitLineX(x);
    REGION_CANNON regionX = board.bitboard->getCannonRegion(bitlineX, y, 9);
    if (abs(board.pieceidOn(x, regionX[1] - 1)) == R_ROOK && board.teamOn(x, regionX[1] - 1) != team)
        return true;
    if (abs(board.pieceidOn(x, regionX[2] + 1)) == R_ROOK && board.teamOn(x, regionX[2] + 1) != team)
        return true;
    if (abs(board.pieceidOn(x, regionX[0])) == R_CANNON && regionX[0] != y && board.teamOn(x, regionX[0]) != team)
        return true;
    if (abs(board.pieceidOn(x, regionX[3])) == R_CANNON && regionX[3] != y && board.teamOn(x, regionX[3]) != team)
        return true;

    return false;
}

/// @brief 判断一个兵是否过河
/// @param board
/// @param x
/// @param y
/// @return
bool isRiveredPawn(Board &board, int x, int y)
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

/// @brief fen转pieceidmap
/// @param fenCode
/// @return
PIECEID_MAP fenToPieceidMap(std::string fenCode)
{
    PIECEID_MAP pieceidMap = PIECEID_MAP{};
    int colNum = 9;
    int rowNum = 0;
    std::map<char, PIECEID> pairs{{'R', R_ROOK}, {'N', R_KNIGHT}, {'H', R_KNIGHT}, {'B', R_BISHOP}, {'E', R_BISHOP}, {'G', R_GUARD}, {'A', R_GUARD}, {'K', R_KING}, {'C', R_CANNON}, {'P', R_PAWN}, {'r', B_ROOK}, {'n', B_KNIGHT}, {'h', B_KNIGHT}, {'b', B_BISHOP}, {'e', B_BISHOP}, {'g', B_GUARD}, {'a', B_GUARD}, {'k', B_KING}, {'c', B_CANNON}, {'p', B_PAWN}};
    for (int i = 0; i < fenCode.size(); i++)
    {
        if (fenCode[i] >= '1' && fenCode[i] <= '9')
        {
            rowNum += fenCode[i] - '0';
            continue;
        }
        else if (fenCode[i] == '/')
        {
            rowNum = 0;
            colNum--;
            continue;
        }
        else if (fenCode[i] == ' ')
        {
            break;
        }
        else
        {
            pieceidMap[rowNum][colNum] = pairs.at(fenCode[i]);
        }
        rowNum++;
    }

    return pieceidMap;
}

/// @brief board转fen串
/// @param board
/// @return
std::string boardToFen(Board board)
{
    std::string result = "";
    int spaceCount = 0;
    std::map<PIECEID, char> pairs{{R_KING, 'K'}, {R_GUARD, 'A'}, {R_BISHOP, 'B'}, {R_KNIGHT, 'N'}, {R_ROOK, 'R'}, {R_CANNON, 'C'}, {R_PAWN, 'P'}, {B_KING, 'k'}, {B_GUARD, 'a'}, {B_BISHOP, 'b'}, {B_KNIGHT, 'n'}, {B_ROOK, 'r'}, {B_CANNON, 'c'}, {B_PAWN, 'p'}};
    for (int x = 9; x >= 0; x--)
    {
        for (int y = 0; y < 9; y++)
        {
            PIECEID pieceid = board.pieceidOn(y, x);
            if (pieceid == EMPTY_PIECEID)
            {
                spaceCount++;
            }
            else
            {
                if (spaceCount > 0)
                {
                    result += std::to_string(spaceCount);
                    spaceCount = 0;
                }
                result += pairs.at(pieceid);
            }
        }
        if (spaceCount > 0)
        {
            result += std::to_string(spaceCount);
            spaceCount = 0;
        }
        result += "/";
    }
    result.pop_back();
    result += board.team == RED ? " w" : " b";
    result += " - - 0 1";

    return result;
}

/// @brief 检查一个着法在当前局面是否合法
/// @param board
/// @param move
/// @return
bool isValidMoveInSituation(Board &board, Move move)
{
    PIECEID attacker = board.pieceidOn(move.x1, move.y1);
    if (attacker == 0) // 若攻击者不存在，则一定是不合理着法
        return false;
    if (attacker != move.attacker.pieceid) // 若攻击者不一致，则一定是不合理着法
        return false;
    if (move.attacker.team() != board.team) // 若攻击者的队伍和当前队伍不一致，则一定是不合理着法
        return false;
    PIECEID captured = board.pieceidOn(move.x2, move.y2);
    if (captured != 0 && board.teamOn(move.x2, move.y2) ==
                             board.teamOn(move.x1, move.y1)) // 吃子着法，若吃子者和被吃者同队伍，则一定不合理
        return false;

    // 分类
    if (abs(attacker) == R_ROOK)
    {
        if (move.x1 != move.x2 && move.y1 != move.y2) // 车走法，若横纵坐标都不相同，则一定不合理
            return false;
        // 生成车的着法范围，看是否有障碍物
        BITLINE bitlineX = board.getBitLineX(move.x1);
        REGION_ROOK regionX = board.bitboard->getRookRegion(bitlineX, move.y1, 9);
        if (move.y2 < regionX[0] || move.y2 > regionX[1])
            return false;
        // 横向
        BITLINE bitlineY = board.getBitLineY(move.y1);
        REGION_ROOK regionY = board.bitboard->getRookRegion(bitlineY, move.x1, 8);
        if (move.x2 < regionY[0] || move.x2 > regionY[1])
            return false;
    }
    else if (abs(attacker) == R_KNIGHT)
    {
        if (move.x1 - 1 == move.x2 || move.x1 + 1 == move.x2) // 向哪一边走就判断那一边有没有障碍物
        {
            if (move.y1 - 2 == move.y2 && board.pieceidOn(move.x1, move.y1 - 1) != 0) // 若有障碍物则不合理
                return false;
            if (move.y1 + 2 == move.y2 && board.pieceidOn(move.x1, move.y1 + 1) != 0)
                return false;
        }
        else
        {
            if (move.x1 - 2 == move.x2 && board.pieceidOn(move.x1 - 1, move.y1) != 0) // 若有障碍物则不合理
                return false;
            if (move.x1 + 2 == move.x2 && board.pieceidOn(move.x1 + 1, move.y1) != 0)
                return false;
        }
        return true;
    }
    else if (abs(attacker) == R_BISHOP)
    {
        // 象走法，不能有障碍物
        if (board.pieceidOn((move.x1 + move.x2) / 2, (move.y1 + move.y2) / 2) != 0)
            return false;
    }
    else if (abs(attacker) == R_CANNON)
    {
        if (move.x1 != move.x2 && move.y1 != move.y2) // 炮走法，若横纵坐标都不同，则一定不合理
            return false;
        // 生成炮的着法范围
        BITLINE bitlineX = board.getBitLineX(move.x1);
        REGION_CANNON regionX = board.bitboard->getCannonRegion(bitlineX, move.y1, 9);
        if ((move.y2 <= regionX[1] || move.y2 >= regionX[2] + 1) && move.y2 != regionX[0] && move.y2 != regionX[3])
            return false;
        // 横向
        BITLINE bitlineY = board.getBitLineY(move.y1);
        REGION_CANNON regionY = board.bitboard->getCannonRegion(bitlineY, move.x1, 8);
        if ((move.x2 <= regionY[1] || move.x2 >= regionY[2]) && move.x2 != regionY[0] && move.x2 != regionY[3])
            return false;
    }

    return true;
}

/// @brief 检查两个在同行或同列的坐标之间是否存在障碍物（要求这两个位置上必须有子）
/// @param board
/// @param x1
/// @param y1
/// @param x2
/// @param y2
/// @return
bool hasBarrier(Board &board, int x1, int y1, int x2, int y2)
{
    if (x1 == x2)
    {
        BITLINE bitlineX = board.getBitLineX(x1);
        REGION_ROOK regionX = board.bitboard->getRookRegion(bitlineX, y1, 9);
        if (y2 >= regionX[0] && y2 <= regionX[1])
        {
            return false;
        }
    }
    else
    {
        BITLINE bitlineY = board.getBitLineY(y1);
        REGION_ROOK regionY = board.bitboard->getRookRegion(bitlineY, x1, 8);
        if (x2 >= regionY[0] && x2 <= regionY[1])
        {
            return false;
        }
    }
    return true;
}
