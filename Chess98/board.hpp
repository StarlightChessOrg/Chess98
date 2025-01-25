#pragma once
#include "evaluate.hpp"

/// @brief 棋盘类
class Board
{
public:
    Board(PIECEID_MAP pieceidMap, int initTeam);

    Piece pieceIndex(PIECE_INDEX pieceIndex);
    Piece piecePosition(int x, int y);

    PIECEID pieceidOn(int x, int y);
    TEAM teamOn(int x, int y);

    std::vector<Piece> getAllLivePieces();
    std::vector<Piece> getPiecesByTeam(TEAM team);

    Piece doMove(int x1, int y1, int x2, int y2);
    Piece doMove(Move move);
    void undoMove(int x1, int x2, int y1, int y2, Piece eaten);
    void undoMove(Move move, Piece eaten);

    bool isKingLive(TEAM team) const
    {
        return team == RED ? this->isRedKingLive : this->isBlackKingLive;
    }

    TEAM team;
    Piece *pieceRedKing = nullptr;
    Piece *pieceBlackKing = nullptr;

    void print();

    void initEvaluate();

    int evaluate() const
    {
        return this->team == RED ? (vlRed - vlBlack) : (vlBlack - vlRed);
    }

    void doNullMove()
    {
        this->team = -this->team;
    }

    void undoNullMove()
    {
        this->team = -this->team;
    }

    bool nullOkay()
    {
        const int vlSelf = this->team == RED ? this->vlRed : this->vlBlack;
        return (vlSelf > 10000 + 600);
    }

    bool nullSafe()
    {
        const int vlSelf = this->team == RED ? this->vlRed : this->vlBlack;
        return (vlSelf > 10000 + 1200);
    }

    bool isChecking = false;

    // 和根节点的距离
    int distance = 0;
    // 评估相关
    int vlRed = 0;
    int vlBlack = 0;

private:
    // 棋盘相关
    PIECEID_MAP pieceidMap{};
    std::array<std::array<int, 10>, 9> pieceIndexMap{};
    std::vector<Piece> pieces{};
    std::vector<PIECE_INDEX> redPieces{};
    std::vector<PIECE_INDEX> blackPieces{};
    bool isRedKingLive = false;
    bool isBlackKingLive = false;
};

/// @brief 初始化棋盘
/// @param pieceidMap 棋子id位置表，一般传DEFAULT_PIECEID_MAP
Board::Board(PIECEID_MAP pieceidMap, int initTeam)
{
    initZobrist();
    this->distance = 0;
    this->team = initTeam;
    this->pieceidMap = pieceidMap;
    for (int x = 0; x < 9; x++)
    {
        for (int y = 0; y < 10; y++)
        {
            PIECEID pieceid = this->pieceidMap[x][y];
            if (pieceid != 0)
            {
                Piece piece{this->pieceidMap[x][y], x, y, (int)this->pieces.size()};
                this->pieces.emplace_back(piece);
                this->pieceIndexMap[x][y] = int(this->pieces.size()) - 1;
                if (pieceid > 0)
                {
                    this->redPieces.emplace_back(int(this->pieces.size()) - 1);
                }
                else
                {
                    this->blackPieces.emplace_back(int(this->pieces.size()) - 1);
                }
                if (pieceid == R_KING)
                {
                    this->isRedKingLive = true;
                }
                if (pieceid == B_KING)
                {
                    this->isBlackKingLive = true;
                }
            }
            else
            {
                this->pieceIndexMap[x][y] = -1;
            }
        }
    }
    initEvaluate();
    // 双方将帅的位置
    for (const Piece &piece : this->getAllLivePieces())
    {
        if (piece.pieceid == R_KING)
        {
            this->pieceRedKing = &(this->pieces[piece.pieceIndex]);
        }
        else if (piece.pieceid == B_KING)
        {
            this->pieceBlackKing = &(this->pieces[piece.pieceIndex]);
        }
    }
}

/// @brief 通过索引号查找piece
/// @param pieceIndex
/// @return
Piece Board::pieceIndex(PIECE_INDEX pieceIndex)
{
    return this->pieces[pieceIndex];
}

/// @brief 通过位置查找piece（若为空则返回一个index为-1的piece）
/// @param x
/// @param y
/// @return
Piece Board::piecePosition(int x, int y)
{
    if (x >= 0 && x <= 8 && y >= 0 && y <= 9)
    {
        PIECEID pieceid = this->pieceidMap[x][y];
        if (pieceid != 0)
        {
            PIECE_INDEX pieceIndex = this->pieceIndexMap[x][y];
            return this->pieceIndex(pieceIndex);
        }
        else
        {
            return Piece{EMPTY_PIECEID, -1, -1, -1};
        }
    }
    else
    {
        return Piece{OVERFLOW_PIECEID, -1, -1, -1};
    }
}

/// @brief 获取指定位置上的pieceid
/// @param x
/// @param y
/// @return
PIECEID Board::pieceidOn(int x, int y)
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

/// @brief 获取指定位置上的队伍
/// @param x
/// @param y
/// @return
TEAM Board::teamOn(int x, int y)
{
    if (x >= 0 && x <= 8 && y >= 0 && y <= 9)
    {
        PIECEID pieceid = this->pieceidMap[x][y];
        if (pieceid > 0)
        {
            return RED;
        }
        else if (pieceid < 0)
        {
            return BLACK;
        }
        else
        {
            return EMPTY_TEAM;
        }
    }
    else
    {
        return OVERFLOW_TEAM;
    }
}

/// @brief 获取棋盘上所有存活的棋子
/// @return
std::vector<Piece> Board::getAllLivePieces()
{
    std::vector<Piece> result{};
    for (Piece piece : this->pieces)
    {
        if (piece.isLive == true)
        {
            result.emplace_back(piece);
        }
    }
    return result;
}

/// @brief 获取指定队伍的所有存活的棋子
/// @param team
/// @return
std::vector<Piece> Board::getPiecesByTeam(TEAM team)
{
    std::vector<Piece> result{};
    std::vector<Piece> allPieces = this->getAllLivePieces();
    for (Piece piece : allPieces)
    {
        if (piece.getTeam() == team)
        {
            result.emplace_back(piece);
        }
    }
    return result;
}

void Board::initEvaluate()
{
    this->vlRed = this->vlBlack = 0;
    for (int x = 0; x < 9; x++)
    {
        for (int y = 0; y < 10; y++)
        {
            PIECEID pid = this->pieceidMap[x][y];
            if (pid > 0)
            {
                this->vlRed += pieceWeights[pid - 1][x][y];
            }
            else if (pid < 0)
            {
                this->vlBlack += pieceWeights[abs(pid) - 1][x][size_t(9) - y];
            }
        }
    }
}

/// @brief 步进
/// @param x1
/// @param y1
/// @param x2
/// @param y2
/// @return 被吃掉的子
Piece Board::doMove(int x1, int y1, int x2, int y2)
{
    Piece eaten = this->piecePosition(x2, y2);
    Piece attackStarter = this->piecePosition(x1, y1);

    // 维护棋盘的棋子追踪
    this->pieceidMap[x2][y2] = this->pieceidMap[x1][y1];
    this->pieceidMap[x1][y1] = 0;
    this->pieceIndexMap[x2][y2] = this->pieceIndexMap[x1][y1];
    this->pieceIndexMap[x1][y1] = -1;
    this->pieces[attackStarter.pieceIndex].x = x2;
    this->pieces[attackStarter.pieceIndex].y = y2;
    if (eaten.pieceIndex != -1)
    {
        this->pieces[eaten.pieceIndex].isLive = false;
    }
    if (eaten.pieceid == R_KING)
    {
        this->isRedKingLive = false;
    }
    if (eaten.pieceid == B_KING)
    {
        this->isBlackKingLive = false;
    }
    // 更新评估分
    if (attackStarter.getTeam() == RED)
    {
        int valNewPos = pieceWeights[attackStarter.pieceid][x2][y2];
        int valOldPos = pieceWeights[attackStarter.pieceid][x1][y1];
        this->vlRed += (valNewPos - valOldPos);
        if (eaten.pieceid != EMPTY_PIECEID)
        {
            this->vlRed += pieceWeights[eaten.pieceid][x2][size_t(9) - y2];
        }
    }
    else
    {
        int valNewPos = pieceWeights[attackStarter.pieceid][x2][size_t(9) - y2];
        int valOldPos = pieceWeights[attackStarter.pieceid][x1][size_t(9) - y1];
        this->vlBlack += (valNewPos - valOldPos);
        if (eaten.pieceid != EMPTY_PIECEID)
        {
            this->vlBlack += pieceWeights[eaten.pieceid][x2][y2];
        }
    }

    this->team = -this->team;

    this->distance += 1;
    return eaten;
}

/// @brief 步进
/// @param move
/// @return 被吃掉的子
Piece Board::doMove(Move move)
{
    return this->doMove(move.x1, move.y1, move.x2, move.y2);
}

/// @brief 撤销步进
/// @param x1
/// @param y1
/// @param x2
/// @param y2
/// @param eaten
void Board::undoMove(int x1, int y1, int x2, int y2, Piece eaten)
{
    this->distance -= 1;

    this->team = -this->team;

    Piece attackStarter = this->piecePosition(x2, y2);

    // 维护棋盘的棋子追踪
    this->pieceidMap[x1][y1] = this->pieceidMap[x2][y2];
    this->pieceidMap[x2][y2] = eaten.pieceid;
    this->pieceIndexMap[x1][y1] = this->pieceIndexMap[x2][y2];
    this->pieceIndexMap[x2][y2] = eaten.pieceIndex;
    this->pieces[attackStarter.pieceIndex].x = x1;
    this->pieces[attackStarter.pieceIndex].y = y1;
    if (eaten.pieceIndex != -1)
    {
        this->pieces[eaten.pieceIndex].isLive = true;
    }
    if (eaten.pieceid == R_KING)
    {
        this->isRedKingLive = true;
    }
    if (eaten.pieceid == B_KING)
    {
        this->isBlackKingLive = true;
    }
    // 更新评估分
    if (attackStarter.getTeam() == RED)
    {
        int valPos1 = pieceWeights[attackStarter.pieceid][x1][y1];
        int valPos2 = pieceWeights[attackStarter.pieceid][x2][y2];
        this->vlRed -= (valPos2 - valPos1);
        if (eaten.pieceid != EMPTY_PIECEID)
        {
            this->vlRed -= pieceWeights[eaten.pieceid][x2][size_t(9) - y2];
        }
    }
    else
    {
        int valPos1 = pieceWeights[attackStarter.pieceid][x1][size_t(9) - y1];
        int valPos2 = pieceWeights[attackStarter.pieceid][x2][size_t(9) - y2];
        this->vlBlack -= (valPos2 - valPos1);
        if (eaten.pieceid != EMPTY_PIECEID)
        {
            this->vlBlack -= pieceWeights[eaten.pieceid][x2][y2];
        }
    }
}

/// @brief 撤销步进
/// @param move
/// @param eaten
void Board::undoMove(Move move, Piece eaten)
{
    this->undoMove(move.x1, move.y1, move.x2, move.y2, eaten);
}

/// @brief 打印
void Board::print()
{
    for (int i = -1; i <= 8; i++)
    {
        for (int j = -1; j <= 9; j++)
        {
            if (i == -1)
            {
                if (j == -1)
                {
                    std::cout << "X ";
                }
                else
                {
                    std::cout << j << " ";
                }
            }
            else
            {
                if (j == -1)
                {
                    std::cout << i << " ";
                }
                else
                {

                    std::cout << getPieceName(this->pieceidOn(i, j));
                }
            }
        }
        std::cout << "\n";
    }
    std::cout << std::endl;
}
