#pragma once
#include "moves.hpp"
#include "heuristic.hpp"
#include "utils.hpp"
#include "book.hpp"

/// @brief 节点对象，存储分数 + 着法
class Node
{
public:
    Node(Move move, int score) : move(move), score(score) {}
    Move move{};
    int score = 0;
};

class Search
{
public:
    ~Search()
    {
        if (historyCache)
        {
            delete historyCache;
            historyCache = nullptr;
        }
        if (pHashTable)
        {
            delete pHashTable;
            pHashTable = nullptr;
        }
        if (pBookFileStruct)
        {
            delete pBookFileStruct;
            pBookFileStruct = nullptr;
        }
    }

public:
    void searchInit(Board &board, int initHashLevel = 25)
    {
        rootMoves.resize(0);
        this->historyCache->init();
        if (this->pHashTable->initDone())
        {
            this->pHashTable->reset();
        }
        else
        {
            this->pHashTable->init(initHashLevel);
        }
        board.distance = 0;
        board.initEvaluate();
        SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);
    }
    void searchStep(Move &bestMove)
    {
        for (auto &move : rootMoves)
        {
            if (bestMove == move)
            {
                move.val = INF;
            }
            else
            {
                move.val--;
            }
        }
    }
    void sortRootMoves()
    {
        std::sort(
            rootMoves.begin(), rootMoves.end(),
            [](Move &first, Move &second) -> bool
            {
                return first.val > second.val;
            });
    }

    Node searchMain(Board &board, int maxDepth, int maxTime);
    Node searchRoot(Board &board, int depth);
    int searchPV(Board &board, int depth, int alpha, int beta);
    int searchCut(Board &board, int depth, int beta, bool banNullMove = false);
    int searchQ(Board &board, int alpha, int beta, int maxDistance = maxSearchDistance);
    Move searchOpenBook(Board &board);

public:
    MOVES rootMoves;
    HistoryHeuristic *historyCache = new HistoryHeuristic();
    tt *pHashTable = new tt();
    BookFileStruct *pBookFileStruct = new BookFileStruct;
};

/// @brief 搜索开局库
/// @param board

Move Search::searchOpenBook(Board &board)
{
    BookStruct bk;
    if (!pBookFileStruct->Open("BOOK.DAT"))
    {
        return Move();
    }

    // 二分法查找开局库
    int nMid = 0;
    int32 hashLock = board.hashLock;
    int32 mirrorHashLock = 0;
    int32 mirrorHashKey = 0;
    board.getMirrorHashinfo(mirrorHashKey, mirrorHashLock);

    int nScan = 0;
    int32 nowHashLock = 0;
    for (nScan = 0; nScan < 2; nScan++)
    {
        int nHigh = pBookFileStruct->nLen - 1;
        int nLow = 0;
        nowHashLock = (nScan == 0) ? hashLock : mirrorHashLock;
        while (nLow <= nHigh)
        {
            nMid = (nHigh + nLow) / 2;
            pBookFileStruct->Read(bk, nMid);
            if (BOOK_POS_CMP(bk, nowHashLock) < 0)
            {
                nLow = nMid + 1;
            }
            else if (BOOK_POS_CMP(bk, nowHashLock) > 0)
            {
                nHigh = nMid - 1;
            }
            else
            {
                break;
            }
        }
        if (nLow <= nHigh)
        {
            break;
        }
    }

    if (nScan == 2)
    {
        pBookFileStruct->Close();
        return Move();
    }

    // 如果找到局面，则向前查找第一个着法
    for (nMid--; nMid >= 0; nMid--)
    {
        pBookFileStruct->Read(bk, nMid);
        if (BOOK_POS_CMP(bk, nowHashLock) < 0)
        {
            break;
        }
    }

    std::vector<Move> bookMoves;

    // 向后依次读入属于该局面的每个着法
    for (nMid++; nMid < pBookFileStruct->nLen; nMid++)
    {
        pBookFileStruct->Read(bk, nMid);
        if (BOOK_POS_CMP(bk, nowHashLock) > 0)
        {
            break;
        }
        else
        {
            int mv = bk.wmv;
            int src = mv & 255;
            int dst = mv >> 8;
            int xSrc = (src & 15) - 3;
            int ySrc = 12 - (src >> 4);
            int xDst = (dst & 15) - 3;
            int yDst = 12 - (dst >> 4);
            if (nScan != 0)
            {
                xSrc = 8 - xSrc;
                xDst = 8 - xDst;
            }
            int vl = bk.wvl;
            Move tMove = Move(xSrc, ySrc, xDst, yDst, vl);
            bookMoves.emplace_back(tMove);
        }
    }

    std::sort(bookMoves.begin(), bookMoves.end(), [](Move &a, Move &b)
              {
        // 这里定义比较规则：从大到小排序
        return a.val > b.val; });

    std::srand(unsigned(std::time(0)));

    int vlSum = 0;
    for (Move &move : bookMoves)
    {
        vlSum += move.val;
    }
    int vlRandom = std::rand() % vlSum;

    Move bookMove;
    for (Move &move : bookMoves)
    {
        vlRandom -= move.val;
        if (vlRandom < 0)
        {
            bookMove = move;
            break;
        }
    }

    pBookFileStruct->Close();
    return bookMove;
}

/// @brief 迭代加深
/// @param board
/// @param maxDepth
/// @param maxTime
/// @return
Node Search::searchMain(Board &board, int maxDepth, int maxTime = 3)
{
    if (board.isKingLive(RED) == false || board.isKingLive(BLACK) == false)
    {
        std::cout << "===========================" << std::endl;
        std::cout << "     !!!!!SUCCESS!!!!!     " << std::endl;
        std::cout << "===========================" << std::endl;
        exit(0);
    }

    Move openBookMove = Search::searchOpenBook(board);

    if (openBookMove != Move())
    {
        std::cout << "find a great move from open book!" << std::endl;
        return Node(openBookMove, 0);
    }

    searchInit(board);
    this->rootMoves = Moves::getMoves(board);
    Node bestNode = Node(Move(), 0);
    clock_t start = clock();
    int depth = 0;

    std::cout << "search starts here!" << std::endl;
    while (depth <= maxDepth)
    {
        depth++;
        
        bestNode = searchRoot(board, depth);

        if (clock() - start >= maxTime * 1000 / 3)
        {
            break;
        }
    }

    std::cout << "search depth: " << depth << std::endl;
    std::cout << "search vl: " << bestNode.score << std::endl;
    std::cout << "used time: " << clock() - start << " ms" << std::endl;

    return bestNode;
}

/// @brief 根节点搜索
/// @param board
/// @param depth
/// @return
Node Search::searchRoot(Board &board, int depth)
{
    Move *pBestMove = nullptr;
    int vl = -INF;
    int vlBest = -INF;
    for (auto &move : rootMoves)
    {
        Piece eaten = board.doMove(move);
        if (vlBest == -INF)
        {
            vl = -searchPV(board, depth - 1, -INF, -vlBest);
        }
        else
        {
            vl = -searchCut(board, depth - 1, -vlBest);
            if (vl > vlBest)
            {
                vl = -searchPV(board, depth - 1, -INF, -vlBest);
            }
        }
        board.undoMove(move, eaten);

        if (vl > vlBest)
        {
            vlBest = vl;
            pBestMove = &move;
            searchStep(move);
        }
    }

    if (!pBestMove)
    {
        vlBest += board.distance;
    }
    else
    {
        this->historyCache->add(*pBestMove, depth);
    }

    this->pHashTable->add(board.hashKey, board.hashLock, vlBest, pBestMove ? exactType : alphaType, depth, false);

    Node result{!pBestMove ? Move{} : *pBestMove, vlBest};
    sortRootMoves();
    return result;
}

/// @brief PV搜索
/// @param board
/// @param depth
/// @param alpha
/// @param beta
/// @return
int Search::searchPV(Board &board, int depth, int alpha, int beta)
{
    // probHash
    int vlHash = -INF;
    this->pHashTable->get(board.hashKey, board.hashLock, vlHash, alpha, beta, depth, board.distance);
    if (vlHash != -INF)
    {
        return vlHash;
    }

    if (depth <= 0)
    {
        return Search::searchQ(board, alpha, beta);
    }

    // mate distance pruning
    const int vlDistanceMate = INF - board.distance;
    if (vlDistanceMate < beta)
    {
        beta = vlDistanceMate;
        if (alpha >= vlDistanceMate)
        {
            this->pHashTable->add(board.hashKey, board.hashLock, vlDistanceMate, exactType, depth, false);
            return vlDistanceMate;
        }
    }

    const bool mChecking = inCheck(board);

    if (!mChecking)
    {
        // futility pruning
        if (depth == 1)
        {
            int vl = board.evaluate();
            if (vl <= alpha - futilityPruningMargin)
            {
                return vl;
            }
            if (vl >= beta + futilityPruningMargin)
            {
                return vl;
            }
        }

        // multi probCut
        if (depth % 4 == 0)
        {
            const double vlScale = (double)vlPawn / 100.0;
            const double a = 1.02 * vlScale;
            const double b = 2.36 * vlScale;
            const double sigma = 82.0 * vlScale;
            const double t = 1.5;
            const int upperBound = int((t * sigma + beta - b) / a);
            const int lowerBound = int((-t * sigma + alpha - b) / a);
            if (searchCut(board, depth - 2, upperBound) >= upperBound)
            {
                return beta;
            }
            else if (searchCut(board, depth - 2, lowerBound + 1) <= lowerBound)
            {
                return alpha;
            }
        }
    }

    nodeType type = alphaType;

    MOVES availableMoves = Moves::getMoves(board);
    captureHeuristic(board, availableMoves);
    this->historyCache->sort(availableMoves);
    Move *pBestMove = nullptr;
    int vl = -INF;
    int vlBest = -INF;
    for (auto &move : availableMoves)
    {
        Piece eaten = board.doMove(move);
        if (vlBest == -INF)
        {
            vl = -searchPV(board, depth - 1, -beta, -alpha);
        }
        else
        {
            vl = -searchCut(board, depth - 1, -alpha);
            if (vl > alpha && vl < beta)
            {
                vl = -searchPV(board, depth - 1, -beta, -alpha);
            }
        }
        board.undoMove(move, eaten);

        if (vl > vlBest)
        {
            vlBest = vl;
            pBestMove = &move;
            if (vl >= beta)
            {
                type = betaType;
                break;
            }
            if (vl > alpha)
            {
                type = exactType;
                alpha = vl;
            }
        }
    }

    if (!pBestMove)
    {
        vlBest += board.distance;
    }
    else
    {
        this->historyCache->add(*pBestMove, depth);
    }
    this->pHashTable->add(board.hashKey, board.hashLock, vlBest, type, depth, false);
    return vlBest;
}

/// @brief 截断节点搜索
/// @param board
/// @param depth
/// @param beta
/// @return
int Search::searchCut(Board &board, int depth, int beta, bool banNullMove)
{

    if (depth <= 0)
    {
        return Search::searchQ(board, beta - 1, beta, 64);
    }

    // mate distance pruning
    const int vlDistanceMate = INF - board.distance;
    const int vlOriginAlpha = beta - 1;
    if (vlDistanceMate < beta)
    {
        beta = vlDistanceMate;
        if (vlOriginAlpha >= vlDistanceMate)
        {
            this->pHashTable->add(board.hashKey, board.hashLock, vlDistanceMate, exactType, depth, false);
            return vlDistanceMate;
        }
    }

    const bool mChecking = inCheck(board);

    if (!mChecking)
    {
        // futility pruning
        if (depth == 1)
        {
            int vl = board.evaluate();
            if (vl <= beta - futilityPruningMargin)
            {
                return vl;
            }
            if (vl >= beta + futilityPruningMargin)
            {

                return vl;
            }
        }

        // multi probCut and null pruning
        if (!banNullMove)
        {
            if (board.nullOkay())
            {
                board.doNullMove();
                int vl = -searchCut(board, depth - 3, -beta + 1, true);
                board.undoNullMove();
                if (vl >= beta)
                {
                    if (board.nullSafe())
                    {
                        return vl;
                    }
                    else if (searchCut(board, depth - 2, beta, true) >= beta)
                    {
                        return vl;
                    }
                }
            }
        }
        else if (depth % 4 == 0)
        {
            const double vlScale = (double)vlPawn / 100.0;
            const double a = 1.02 * vlScale;
            const double b = 2.36 * vlScale;
            const double sigma = 82.0 * vlScale;
            const double t = 1.5;
            const int upperBound = int((t * sigma + beta - b) / a);
            if (searchCut(board, depth - 2, upperBound) >= upperBound)
            {
                return beta;
            }
        }
    }
    nodeType type = alphaType;

    MOVES availableMoves = Moves::getMoves(board);
    captureHeuristic(board, availableMoves);
    this->historyCache->sort(availableMoves);
    Move *pBestMove = nullptr;
    int vlBest = -INF;
    int searchedCnt = 0;
    for (auto &move : availableMoves)
    {
        Piece eaten = board.doMove(move);
        int vl = -INF;
        // lmr pruning
        if (!mChecking &&
            eaten.pieceid == EMPTY_PIECEID &&
            depth >= 3 &&
            searchedCnt >= 4)
        {
            vl = -searchCut(board, depth - 2, -beta + 1);
            if (vl >= beta)
            {
                vl = -searchCut(board, depth - 1, -beta + 1);
            }
        }
        else
        {
            vl = -searchCut(board, depth - 1, -beta + 1);
        }
        board.undoMove(move, eaten);

        if (vl > vlBest)
        {

            vlBest = vl;
            pBestMove = &move;
            if (vl >= beta)
            {
                type = betaType;
                break;
            }
        }

        searchedCnt++;
    }

    if (!pBestMove)
    {
        vlBest += board.distance;
    }
    else
    {
        this->historyCache->add(*pBestMove, depth);
    }
    this->pHashTable->add(board.hashKey, board.hashLock, vlBest, type, depth, false);
    return vlBest;
}

/// @brief 静态搜索函数
/// @param board
/// @param alpha
/// @param beta
/// @return
int Search::searchQ(Board &board, int alpha, int beta, int maxDistance)
{
    if (board.distance >= maxDistance)
    {
        return board.evaluate();
    }

    // mate distance pruning
    const int vlDistanceMate = INF - board.distance;
    if (vlDistanceMate < beta)
    {
        beta = vlDistanceMate;
        if (alpha >= vlDistanceMate)
        {
            return vlDistanceMate;
        }
    }

    // null and delta pruning
    const bool mChecking = inCheck(board);
    int leftDistance = mChecking ? std::min<int>(4, maxDistance - 1) : maxDistance - 1;
    int vlBest = -INF;
    if (!mChecking)
    {
        int vl = board.evaluate();
        if (vl >= beta)
        {
            return vl;
        }
        // delta pruning
        if (vl <= alpha - deltaPruningMargin)
        {
            return alpha;
        }

        vlBest = vl;
        alpha = std::max<int>(alpha, vl);
    }

    MOVES availableMoves = mChecking ? Moves::getMoves(board) : Moves::getGoodCaptures(board);

    for (const Move &move : availableMoves)
    {
        Piece eaten = board.doMove(move);
        int vl = -Search::searchQ(board, -beta, -alpha, leftDistance);
        board.undoMove(move, eaten);
        if (vl > vlBest)
        {
            if (vl >= beta)
            {
                return vl;
            }
            vlBest = vl;
            alpha = std::max<int>(alpha, vl);
        }
    }

    if (vlBest == -INF)
    {
        vlBest += board.distance;
    }

    return vlBest;
}
