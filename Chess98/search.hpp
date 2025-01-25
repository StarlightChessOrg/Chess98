#pragma once
#include "moves.hpp"
#include "heuristic.hpp"
#include "utils.hpp"

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
    void searchInit(Board &board)
    {
        rootMoves.resize(0);
        this->historyCache->init();
        board.distance = 0;
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
    int searchQ(Board &board, int alpha, int beta, int maxDistance);

    HistoryHeuristic *historyCache = new HistoryHeuristic();
    MOVES rootMoves;
};

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
    searchInit(board);
    this->rootMoves = Moves::getMoves(board);
    Node bestNode = Node(Move(), 0);
    clock_t start = clock();
    int depth = 1;

    std::cout << "search starts here!" << std::endl;
    while (depth <= maxDepth)
    {
        bestNode = searchRoot(board, depth);

        if (clock() - start >= maxTime * 1000 / 3)
        {
            break;
        }
        depth++;
    }

    std::cout << "search depth: " << depth << std::endl;
    std::cout << "search vl: " << bestNode.score << std::endl;

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
    if (!board.isKingLive(RED) || !board.isKingLive(BLACK))
    {
        return board.evaluate();
    }

    if (depth <= 0)
    {
        return Search::searchQ(board, alpha, beta, 64);
    }

    // probCut
    const bool mChecking = inCheck(board);

    if (depth % 4 == 0 && !mChecking)
    {
        const float a = 3;
        const float b = 7;
        const float t = 1.5;
        const float sigma = 25;
        const int upperBound = (t * sigma + beta - b) / a;
        const int lowerBound = (-t * sigma + alpha - b) / a;
        if (searchCut(board, depth - 2, upperBound) >= upperBound)
        {
            return beta;
        }
        else if (searchCut(board, depth - 2, lowerBound + 1) <= lowerBound)
        {
            return alpha;
        }
    }

    MOVES availableMoves = Moves::getMoves(board);
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
                break;
            }
            if (vl > alpha)
            {
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
    return vlBest;
}

/// @brief 截断节点搜索
/// @param board
/// @param depth
/// @param beta
/// @return
int Search::searchCut(Board &board, int depth, int beta, bool banNullMove)
{
    if (!board.isKingLive(RED) || !board.isKingLive(BLACK))
    {
        return board.evaluate();
    }

    if (depth <= 0)
    {
        return Search::searchQ(board, beta - 1, beta, 64);
    }

    // probCut
    const bool mChecking = inCheck(board);

    if (!mChecking)
    {
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
            const float a = 3;
            const float b = 7;
            const float t = 1.5;
            const float sigma = 25;
            const int upperBound = (t * sigma + beta - b) / a;
            if (searchCut(board, depth - 2, upperBound) >= upperBound)
            {
                return beta;
            }
        }
    }

    MOVES availableMoves = Moves::getMoves(board);
    this->historyCache->sort(availableMoves);
    Move *pBestMove = nullptr;
    int vlBest = -INF;
    for (auto &move : availableMoves)
    {
        Piece eaten = board.doMove(move);
        int vl = -searchCut(board, depth - 1, -beta + 1);
        board.undoMove(move, eaten);

        if (vl > vlBest)
        {
            vlBest = vl;
            pBestMove = &move;
            if (vl >= beta)
            {
                break;
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
    return vlBest;
}

/// @brief 静态搜索函数
/// @param board
/// @param alpha
/// @param beta
/// @return
int Search::searchQ(Board &board, int alpha, int beta, int maxDistance)
{
    if (board.distance >= maxDistance || !board.isKingLive(RED) || !board.isKingLive(BLACK))
    {
        return board.evaluate();
    }

    const bool mChecking = inCheck(board);
    int leftDistance = mChecking ? std::min<int>(4, maxDistance - 1) : maxDistance - 1;
    int vlBest = -INF;
    if (!mChecking)
    {
        int vl = board.evaluate();
        vlBest = vl;
        if (vl >= beta)
        {
            return vl;
        }
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
            vlBest = vl;
            if (vl >= beta)
            {
                return vl;
            }
            alpha = std::max<int>(alpha, vl);
        }
    }

    if (vlBest == -INF)
    {
        vlBest += board.distance;
    }

    return vlBest;
}
