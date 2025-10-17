#pragma once
#include "board.hpp"

bool isRivercrossedPawn(Board &board, int x, int y);
bool hasCrossedRiver(Board &board, int x, int y);
bool isInPalace(Board &board, int x, int y);
bool inCheck(Board &board, TEAM judgeTeam);
bool hasProtector(Board &board, int x, int y);
bool isValidMoveInSituation(Board &board, Move move);
PIECEID_MAP fenToPieceidmap(std::string fenCode);
std::string pieceidmapToFen(PIECEID_MAP pieceidMap, TEAM team);

bool isRivercrossedPawn(Board &board, int x, int y)
{
    PIECEID pieceid = board.pieceidOn(x, y);
    if (pieceid == R_PAWN)
    {
        return y >= 5 && y <= 9;
    }
    if (pieceid == B_PAWN)
    {
        return y >= 0 && y <= 4;
    }
    return false;
}

bool hasCrossedRiver(Board &board, int x, int y)
{
    TEAM team = board.teamOn(x, y);
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

bool isInPalace(Board &board, int x, int y)
{
    TEAM team = board.teamOn(x, y);
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

bool inCheck(Board &board, TEAM judgeTeam)
{
    const Piece &king = judgeTeam == RED ? board.getPieceReg(R_KING) : board.getPieceReg(B_KING);
    const int &x = king.x;
    const int &y = king.y;
    const TEAM &team = king.team;

    // 兵
    const PIECEID ENEMY_PAWN = R_PAWN * -team;
    if (board.pieceidOn(x + 1, y) == ENEMY_PAWN)
    {
        return true;
    }
    if (board.pieceidOn(x - 1, y) == ENEMY_PAWN)
    {
        return true;
    }
    if (board.pieceidOn(x, (team == RED ? y - 1 : y + 1)) == ENEMY_PAWN)
    {
        return true;
    }

    // 马
    const PIECEID ENEMY_KNIGHT = R_KNIGHT * -team;
    if (board.pieceidOn(x + 1, y + 1) == EMPTY_PIECEID)
    {
        if (board.pieceidOn(x + 2, y + 1) == ENEMY_KNIGHT)
        {
            return true;
        }
        if (board.pieceidOn(x + 1, y + 2) == ENEMY_KNIGHT)
        {
            return true;
        }
    }
    if (board.pieceidOn(x - 1, y + 1) == EMPTY_PIECEID)
    {
        if (board.pieceidOn(x - 2, y + 1) == ENEMY_KNIGHT)
        {
            return true;
        }
        if (board.pieceidOn(x - 1, y + 2) == ENEMY_KNIGHT)
        {
            return true;
        }
    }
    if (board.pieceidOn(x + 1, y - 1) == EMPTY_PIECEID)
    {
        if (board.pieceidOn(x + 2, y - 1) == ENEMY_KNIGHT)
        {
            return true;
        }
        if (board.pieceidOn(x + 1, y - 2) == ENEMY_KNIGHT)
        {
            return true;
        }
    }
    if (board.pieceidOn(x - 1, y - 1) == EMPTY_PIECEID)
    {
        if (board.pieceidOn(x - 2, y - 1) == ENEMY_KNIGHT)
        {
            return true;
        }
        if (board.pieceidOn(x - 1, y - 2) == ENEMY_KNIGHT)
        {
            return true;
        }
    }

    // 将、车、炮
    const PIECEID ENEMY_ROOK = R_ROOK * -team;
    const PIECEID ENEMY_CANNON = R_CANNON * -team;
    const PIECEID ENEMY_KING = R_KING * -team;

    UINT32 bitlineY = board.getBitLineY(y);
    REGION_CANNON regionY = board.bitboard->getCannonRegion(bitlineY, x, 8);
    if (board.pieceidOn(regionY[1] - 1, y) == ENEMY_ROOK)
    {
        return true;
    }
    if (board.pieceidOn(regionY[2] + 1, y) == ENEMY_ROOK)
    {
        return true;
    }
    if (board.pieceidOn(regionY[0], y) == ENEMY_CANNON)
    {
        return true;
    }
    if (board.pieceidOn(regionY[3], y) == ENEMY_CANNON)
    {
        return true;
    }

    UINT32 bitlineX = board.getBitLineX(x);
    REGION_CANNON regionX = board.bitboard->getCannonRegion(bitlineX, y, 9);
    const PIECEID &p1 = board.pieceidOn(x, regionX[1] - 1);
    if (p1 == ENEMY_ROOK || p1 == ENEMY_KING)
    {
        return true;
    }
    const PIECEID &p2 = board.pieceidOn(x, regionX[2] + 1);
    if (p2 == ENEMY_ROOK || p2 == ENEMY_KING)
    {
        return true;
    }
    if (board.pieceidOn(x, regionX[0]) == ENEMY_CANNON)
    {
        return true;
    }
    if (board.pieceidOn(x, regionX[3]) == ENEMY_CANNON)
    {
        return true;
    }

    return false;
}

bool hasProtector(Board &board, int x, int y)
{
    const Piece &piece = board.piecePosition(x, y);
    const TEAM &team = piece.team;

    // 兵
    const PIECEID MY_PAWN = R_PAWN * team;
    if (isRivercrossedPawn(board, x + 1, y))
    {
        return true;
    }
    if (isRivercrossedPawn(board, x - 1, y))
    {
        return true;
    }
    if (board.pieceidOn(x, (team == RED ? y - 1 : y + 1)) == MY_PAWN)
    {
        return true;
    }

    // 马
    const PIECEID MY_KNIGHT = R_KNIGHT * team;
    if (board.pieceidOn(x + 1, y + 1) == EMPTY_PIECEID)
    {
        if (board.pieceidOn(x + 2, y + 1) == MY_KNIGHT)
        {
            return true;
        }
        if (board.pieceidOn(x + 1, y + 2) == MY_KNIGHT)
        {
            return true;
        }
    }
    if (board.pieceidOn(x - 1, y + 1) == EMPTY_PIECEID)
    {
        if (board.pieceidOn(x - 2, y + 1) == MY_KNIGHT)
        {
            return true;
        }
        if (board.pieceidOn(x - 1, y + 2) == MY_KNIGHT)
        {
            return true;
        }
    }
    if (board.pieceidOn(x + 1, y - 1) == EMPTY_PIECEID)
    {
        if (board.pieceidOn(x + 2, y - 1) == MY_KNIGHT)
        {
            return true;
        }
        if (board.pieceidOn(x + 1, y - 2) == MY_KNIGHT)
        {
            return true;
        }
    }
    if (board.pieceidOn(x - 1, y - 1) == EMPTY_PIECEID)
    {
        if (board.pieceidOn(x - 2, y - 1) == MY_KNIGHT)
        {
            return true;
        }
        if (board.pieceidOn(x - 1, y - 2) == MY_KNIGHT)
        {
            return true;
        }
    }

    // 士、象、将
    const PIECEID MY_BISHOP = R_BISHOP * team;
    const PIECEID MY_GUARD = R_GUARD * team;
    const PIECEID MY_KING = R_KING * team;
    if (hasCrossedRiver(board, x, y) == false)
    {
        if (board.pieceidOn(x + 1, y + 1) == EMPTY_PIECEID)
        {
            if (board.pieceidOn(x + 2, y + 2) == MY_BISHOP)
            {
                return true;
            }
        }
        if (board.pieceidOn(x - 1, y + 1) == EMPTY_PIECEID)
        {
            if (board.pieceidOn(x - 2, y + 2) == MY_BISHOP)
            {
                return true;
            }
        }
        if (board.pieceidOn(x + 1, y - 1) == EMPTY_PIECEID)
        {
            if (board.pieceidOn(x + 2, y - 2) == MY_BISHOP)
            {
                return true;
            }
        }
        if (board.pieceidOn(x - 1, y - 1) == EMPTY_PIECEID)
        {
            if (board.pieceidOn(x - 2, y - 2) == MY_BISHOP)
            {
                return true;
            }
        }
        if (isInPalace(board, x, y))
        {
            if (board.pieceidOn(x + 1, y) == MY_GUARD)
            {
                return true;
            }
            if (board.pieceidOn(x - 1, y) == MY_GUARD)
            {
                return true;
            }
            if (board.pieceidOn(x, y + 1) == MY_GUARD)
            {
                return true;
            }
            if (board.pieceidOn(x, y - 1) == MY_GUARD)
            {
                return true;
            }
            if (board.pieceidOn(x + 1, y) == MY_KING)
            {
                return true;
            }
            if (board.pieceidOn(x - 1, y) == MY_KING)
            {
                return true;
            }
            if (board.pieceidOn(x, y + 1) == MY_KING)
            {
                return true;
            }
            if (board.pieceidOn(x, y - 1) == MY_KING)
            {
                return true;
            }
        }
    }

    // 车、炮
    const PIECEID MY_ROOK = R_ROOK * team;
    const PIECEID MY_CANNON = R_CANNON * team;

    UINT32 bitlineY = board.getBitLineY(y);
    REGION_CANNON regionY = board.bitboard->getCannonRegion(bitlineY, x, 8);
    if (board.pieceidOn(regionY[1] - 1, y) == MY_ROOK)
    {
        return true;
    }
    if (board.pieceidOn(regionY[2] + 1, y) == MY_ROOK)
    {
        return true;
    }
    if (board.pieceidOn(regionY[0], y) == MY_CANNON)
    {
        return true;
    }
    if (board.pieceidOn(regionY[3], y) == MY_CANNON)
    {
        return true;
    }
    UINT32 bitlineX = board.getBitLineX(x);
    REGION_CANNON regionX = board.bitboard->getCannonRegion(bitlineX, y, 9);
    if (board.pieceidOn(x, regionX[1] - 1) == MY_ROOK)
    {
        return true;
    }
    if (board.pieceidOn(x, regionX[2] + 1) == MY_ROOK)
    {
        return true;
    }
    if (board.pieceidOn(x, regionX[0]) == MY_CANNON)
    {
        return true;
    }
    if (board.pieceidOn(x, regionX[3]) == MY_CANNON)
    {
        return true;
    }
    return false;
}

bool isValidMoveInSituation(Board &board, Move move)
{
    PIECEID attacker = board.pieceidOn(move.x1, move.y1);
    if (attacker == 0) // 若攻击者不存在, 则一定是不合理着法
        return false;
    if (attacker != move.attacker.pieceid) // 若攻击者不一致, 则一定是不合理着法
        return false;
    if (move.attacker.team != board.team) // 若攻击者的队伍和当前队伍不一致, 则一定是不合理着法
        return false;
    PIECEID captured = board.pieceidOn(move.x2, move.y2);
    if (captured != 0 && board.teamOn(move.x2, move.y2) ==
                             board.teamOn(move.x1, move.y1)) // 吃子着法, 若吃子者和被吃者同队伍, 则一定不合理
        return false;

    // 分类
    if (abs(attacker) == R_ROOK)
    {
        if (move.x1 != move.x2 && move.y1 != move.y2) // 车走法, 若横纵坐标都不相同, 则一定不合理
            return false;
        // 生成车的着法范围, 看是否有障碍物
        UINT32 bitlineX = board.getBitLineX(move.x1);
        REGION_ROOK regionX = board.bitboard->getRookRegion(bitlineX, move.y1, 9);
        if (move.y2 < regionX[0] || move.y2 > regionX[1])
            return false;
        // 横向
        UINT32 bitlineY = board.getBitLineY(move.y1);
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
        // 象走法, 不能有障碍物
        if (board.pieceidOn((move.x1 + move.x2) / 2, (move.y1 + move.y2) / 2) != 0)
            return false;
    }
    else if (abs(attacker) == R_CANNON)
    {
        if (move.x1 != move.x2 && move.y1 != move.y2) // 炮走法, 若横纵坐标都不同, 则一定不合理
            return false;
        // 生成炮的着法范围
        UINT32 bitlineX = board.getBitLineX(move.x1);
        REGION_CANNON regionX = board.bitboard->getCannonRegion(bitlineX, move.y1, 9);
        if ((move.y2 <= regionX[1] || move.y2 >= regionX[2] + 1) && move.y2 != regionX[0] && move.y2 != regionX[3])
            return false;
        // 横向
        UINT32 bitlineY = board.getBitLineY(move.y1);
        REGION_CANNON regionY = board.bitboard->getCannonRegion(bitlineY, move.x1, 8);
        if ((move.x2 <= regionY[1] || move.x2 >= regionY[2]) && move.x2 != regionY[0] && move.x2 != regionY[3])
            return false;
    }

    board.doMoveSimple(move);
    const bool skip = inCheck(board, -board.team);
    board.undoMoveSimple();

    return !skip;
}

PIECEID_MAP fenToPieceidmap(std::string fenCode)
{
    PIECEID_MAP pieceidMap = PIECEID_MAP{};
    int colNum = 9;
    int rowNum = 0;
    std::map<char, PIECEID> pairs{
        {'R', R_ROOK},
        {'N', R_KNIGHT},
        {'H', R_KNIGHT},
        {'B', R_BISHOP},
        {'E', R_BISHOP},
        {'G', R_GUARD},
        {'A', R_GUARD},
        {'K', R_KING},
        {'C', R_CANNON},
        {'P', R_PAWN},
        {'r', B_ROOK},
        {'n', B_KNIGHT},
        {'h', B_KNIGHT},
        {'b', B_BISHOP},
        {'e', B_BISHOP},
        {'g', B_GUARD},
        {'a', B_GUARD},
        {'k', B_KING},
        {'c', B_CANNON},
        {'p', B_PAWN}};
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

std::string pieceidmapToFen(PIECEID_MAP pieceidMap, TEAM team)
{
    std::string result = "";
    int spaceCount = 0;
    std::map<PIECEID, char> pairs{
        {R_KING, 'K'},
        {R_GUARD, 'A'},
        {R_BISHOP, 'B'},
        {R_KNIGHT, 'N'},
        {R_ROOK, 'R'},
        {R_CANNON, 'C'},
        {R_PAWN, 'P'},
        {B_KING, 'k'},
        {B_GUARD, 'a'},
        {B_BISHOP, 'b'},
        {B_KNIGHT, 'n'},
        {B_ROOK, 'r'},
        {B_CANNON, 'c'},
        {B_PAWN, 'p'}};
    for (int x = 9; x >= 0; x--)
    {
        for (int y = 0; y < 9; y++)
        {
            PIECEID pieceid = pieceidMap[y][x];
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
    result += team == RED ? " w" : " b";
    result += " - - 0 1";

    return result;
}
