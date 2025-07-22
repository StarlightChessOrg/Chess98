#pragma once
#include "hash.hpp"
#include "heuristic.hpp"
#include "moves.hpp"
#include "utils.hpp"

class Search
{
public:
    Search(PIECEID_MAP pieceidMap, TEAM team)
    {
        this->board = Board(pieceidMap, team);
    }

    void reset()
    {
        this->rootMoves = MOVES{};
        board.distance = 0;
        board.initEvaluate();
        this->pHistory->reset();
        this->pKiller->reset();
        this->pTransportation->reset();
        this->nodecount = 0;
    }

    Board &getBoard()
    {
        return this->board;
    }

protected:
    Board board{PIECEID_MAP{}, EMPTY_TEAM};
    MOVES rootMoves{};
    HistoryTable *pHistory = new HistoryTable{};
    KillerTable *pKiller = new KillerTable{};
    TransportationTable *pTransportation = new TransportationTable{};
    int nodecount = 0;

public:
    Result searchMain(int maxDepth, int maxTime);

protected:
    Result searchOpenBook();
    Result searchRoot(int depth);
    int searchPV(int depth, int alpha, int beta);
    int searchCut(int depth, int beta, bool banNullMove = false);
    int searchQ(int alpha, int beta, int leftDistance = QUIESCENCE_EXTEND_DEPTH);

protected:
    void setCheckingMove(bool mChecking)
    {
        if (mChecking && !board.historyMoves.empty())
        {
            board.historyMoves.back().isCheckingMove = true;
        }
    }

    TrickResult<int> nullAndDeltaPruning(bool mChecking, int& alpha, int& beta, int& vlBest) const
    {
        if (!mChecking)
        {
            int vl = board.evaluate();
            if (vl >= beta)
            {
                return TrickResult<int>{true, {vl}};
            }
            vlBest = vl;
            if (vl > alpha)
            {
                alpha = vl;
            }
        }
        return TrickResult<int>{false, {}};
    }

    TrickResult<int> mateDistancePruning(int alpha, int &beta) const
    {
        const int vlDistanceMate = INF - board.distance;
        if (vlDistanceMate < beta)
        {
            beta = vlDistanceMate;
            if (alpha >= vlDistanceMate)
            {
                return TrickResult<int>(true, {vlDistanceMate});
            }
        }
        return {false, {}};
    }

    TrickResult<int> futilityPruning(int alpha, int beta, int depth) const
    {
        if (depth == 1)
        {
            int vl = board.evaluate();
            if (vl <= beta - FUTILITY_PRUNING_MARGIN || vl >= beta + FUTILITY_PRUNING_MARGIN)
            {
                return TrickResult<int>{true, {vl}};
            }
        }
        return TrickResult<int>{false, {}};
    }

    TrickResult<int> multiProbCut(SEARCH_TYPE searchType, int alpha, int beta, int depth)
    {
        if ((depth % 4 == 0 && searchType == CUT) || searchType == PV)
        {
            const double vlScale = (double)vlPawn / 100.0;
            const double a = 1.02 * vlScale;
            const double b = 2.36 * vlScale;
            const double sigma = 82.0 * vlScale;
            const double t = 1.5;
            const int upperBound = int((t * sigma + beta - b) / a);
            const int lowerBound = int((-t * sigma + alpha - b) / a);
            if (this->searchCut(depth - 2, upperBound) >= upperBound)
            {
                return TrickResult<int>{true, {beta}};
            }
            else if (searchType == PV && this->searchCut(depth - 2, lowerBound + 1) <= lowerBound)
            {
                return TrickResult<int>{true, {alpha}};
            }
        }

        return TrickResult<int>{false, {}};
    }

    bool repeatCheck() const
    {
        // 这个函数只判断对方有没有违规，违规返回INF
        const Board& board = this->board;
        const MOVES& history = board.historyMoves;
        const size_t size = history.size();
        // 前面加一些快速判断的方法
        const bool quickCheck = (
            false // TODO
        );
        // 总历史着法数5个及以上才可判定是否重复
        if (quickCheck == false && size >= 5)
        {
            const Move& ply1 = history[size_t(size - 1)];
            const Move& ply2 = history[size_t(size - 2)];
            const Move& ply3 = history[size_t(size - 3)];
            const Move& ply4 = history[size_t(size - 4)];
            const Move& ply5 = history[size_t(size - 5)];
            // 判断是否出现重复局面，没有则直接false
            // 试想如下重复局面：（格式：plyX: x1y1x2y2）
            // ply1: 0001, ply2: 0908, ply3: 0100, ply4: 0809, ply5: 0001
            const bool isRepeat = (
                ply1 == ply5 &&
                ply1.startpos == ply3.endpos &&
                ply1.endpos == ply3.startpos &&
                ply2.startpos == ply4.endpos &&
                ply2.endpos == ply4.startpos
            );
            if (!isRepeat)
            {
                return false;
            }
            // 长将在任何情况下都会判负
            // 由于性能原因，isCheckingMove是被延迟设置的，ply1可能还没有被设成checkingMove
            // 但是若判定了循环局面，ply1必然等于ply5
            // 若ply5和ply3都是将军着法，且出现循环局面，则直接判定违规
            if (ply5.isCheckingMove == true && ply3.isCheckingMove == true)
            {
                return false;
            }
            // 长捉情况比较特殊
            // 只有车、马、炮能作为长捉的发起者
            // 发起者不断捉同一个子，判负
            const bool condition1 = ( // 是否是车马炮
                abs(ply1.attacker.pieceid) == R_ROOK ||
                abs(ply1.attacker.pieceid) == R_KNIGHT ||
                abs(ply1.attacker.pieceid) == R_CANNON
            );
            const bool condition2 = (
                false // TODO
            );
            if (condition1 == true && condition2 == true)
            {
                return true;
            }
        }
        return false;
    }
};

Result Search::searchMain(int maxDepth, int maxTime = 3)
{
    nodecount++;

    if (!board.isKingLive(RED) || !board.isKingLive(BLACK))
    {
        exit(0);
    }

    // openbook search
    Result openbookResult = Search::searchOpenBook();
    if (openbookResult.val != -1)
    {
        std::cout << "Find a great move from OpenBook!" << std::endl;
        return openbookResult;
    }

    this->reset();

    // situation info
    std::cout << "situation: " << boardToFen(board) << std::endl;
    std::cout << "evaluate: " << board.evaluate() << std::endl;

    this->rootMoves = MovesGenerate::getMoves(board);

    Result bestNode = Result(Move(), 0);
    clock_t start = clock();

    for (int depth = 1; depth <= maxDepth; depth++)
    {
        bestNode = searchRoot(depth);
        // log
        std::cout << " depth: " << depth;
        std::cout << " vl: " << bestNode.val;
        std::cout << " moveid: " << bestNode.move.id;
        std::cout << " duration(ms): " << clock() - start;
        std::cout << " count: " << nodecount;
        std::cout << " nps: " << nodecount / (clock() - start + 1) * 1000;
        std::cout << std::endl;

        // timeout break
        if (clock() - start >= maxTime * 1000 / 3)
        {
            break;
        }
    }

    return bestNode;
}

Result Search::searchOpenBook()
{
    struct BookStruct
    {
        union
        {
            uint32_t dwZobristLock;
            int nPtr;
        };
        uint16_t wmv, wvl;
    };

    struct BookFileStruct
    {
        FILE *fp = nullptr;
        int nLen = 0;

        bool open(const char *szFileName, bool bEdit = false)
        {
            errno_t result = fopen_s(&fp, szFileName, bEdit ? "r+b" : "rb");
            if (result == 0)
            {
                fseek(fp, 0, SEEK_END);
                nLen = ftell(fp) / sizeof(BookStruct);
                return true;
            }
            return false;
        }

        void close(void) const
        {
            fclose(fp);
        }

        void read(BookStruct &bk, int nMid) const
        {
            fseek(fp, nMid * sizeof(BookStruct), SEEK_SET);
            fread(&bk, sizeof(BookStruct), 1, fp);
        }

        void write(const BookStruct &bk, int nMid) const
        {
            fseek(fp, nMid * sizeof(BookStruct), SEEK_SET);
            fwrite(&bk, sizeof(BookStruct), 1, fp);
        }
    };

    std::function<int(BookStruct &, int32)> bookPosCmp = [](BookStruct &bk, int32 hashLock) -> int
    {
        uint32_t bookLock = bk.dwZobristLock;
        uint32_t boardLock = (uint32_t)hashLock;
        if (bookLock < boardLock)
            return -1;
        else if (bookLock > boardLock)
            return 1;
        return 0;
    };

    BookStruct bk{};
    BookFileStruct pBookFileStruct{};

    if (!pBookFileStruct.open("BOOK.DAT"))
    {
        return Result{Move{}, -1};
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
        int nHigh = pBookFileStruct.nLen - 1;
        int nLow = 0;
        nowHashLock = (nScan == 0) ? hashLock : mirrorHashLock;
        while (nLow <= nHigh)
        {
            nMid = (nHigh + nLow) / 2;
            pBookFileStruct.read(bk, nMid);
            if (bookPosCmp(bk, nowHashLock) < 0)
            {
                nLow = nMid + 1;
            }
            else if (bookPosCmp(bk, nowHashLock) > 0)
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
        pBookFileStruct.close();
        return Result{Move{}, -1};
    }

    // 如果找到局面，则向前查找第一个着法
    for (nMid--; nMid >= 0; nMid--)
    {
        pBookFileStruct.read(bk, nMid);
        if (bookPosCmp(bk, nowHashLock) < 0)
        {
            break;
        }
    }

    std::vector<Move> bookMoves;

    // 向后依次读入属于该局面的每个着法
    for (nMid++; nMid < pBookFileStruct.nLen; nMid++)
    {
        pBookFileStruct.read(bk, nMid);
        if (bookPosCmp(bk, nowHashLock) > 0)
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

    // 从大到小排序
    std::sort(bookMoves.begin(), bookMoves.end(), [](Move &a, Move &b)
              { return a.val > b.val; });

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

    pBookFileStruct.close();

    bookMove.attacker = board.piecePosition(bookMove.x1, bookMove.y1);
    bookMove.captured = board.piecePosition(bookMove.x2, bookMove.y2);

    return isValidMoveInSituation(board, bookMove) ? Result{bookMove, 1} : Result{Move{}, -1};
}

Result Search::searchRoot(int depth)
{
    Move bestMove{};
    int vl = -INF;
    int vlBest = -INF;

    for (const Move &move : rootMoves)
    {
        board.doMove(move);
        if (vlBest == -INF)
        {
            vl = -searchPV(depth - 1, -INF, -vlBest);
        }
        else
        {
            vl = -searchCut(depth - 1, -vlBest);
            if (vl > vlBest)
            {
                vl = -searchPV(depth - 1, -INF, -vlBest);
            }
        }
        if (vl > vlBest)
        {
            vlBest = vl;
            bestMove = move;
            // search step
            for (Move &_move : rootMoves)
            {
                if (bestMove == _move)
                {
                    _move.val = INF;
                }
                else
                {
                    _move.val--;
                }
            }
        }
        board.undoMove();
    }

    if (bestMove.id == -1)
    {
        vlBest += board.distance;
    }
    else
    {
        this->pHistory->add(bestMove, depth);
        this->pTransportation->add(board, bestMove, vlBest, EXACT_TYPE, depth);
    }

    // 历史启发调整根节点似乎更快
    this->pHistory->sort(rootMoves);

    if (bestMove.id != -1)
    {
        return Result{bestMove, vlBest};
    }
    else
    {
        return Result{MovesGenerate::getMoves(board)[0], vlBest};
    }
}

int Search::searchPV(int depth, int alpha, int beta)
{
    nodecount++;

    // 检查将帅是否在棋盘上
    if (!board.isKingLive(board.team))
    {
        return -INF + board.distance;
    }

    // 置换表分数
    const int vlHash = this->pTransportation->getValue(board, alpha, beta, depth);
    if (vlHash != -INF)
    {
        return vlHash;
    }

    // 静态搜索
    if (depth <= 0)
    {
        int vl = Search::searchQ(alpha, beta);
        return vl;
    }

    // mate distance pruning
    TrickResult<int> result = this->mateDistancePruning(alpha, beta);
    if (result.isSuccess)
    {
        return result.data[0];
    }

    // variables
    const bool mChecking = inCheck(board, board.team);

    // 验证上一步是否是将军着法
    this->setCheckingMove(mChecking);

    // 重复检测
    bool repeatResult = this->repeatCheck();
    if (repeatResult == true)
    {
        return INF - board.distance;
    }

    // variables
    int vlBest = -INF;
    Move bestMove{};
    NODE_TYPE type = ALPHA_TYPE;
    MOVES availableMoves;

    // 置换表着法
    Move goodMove = this->pTransportation->getMove(board);
    if (goodMove.id == -1 && depth >= 2)
    {
        if (searchPV(depth / 2, alpha, beta) <= alpha)
        {
            searchPV(depth / 2, -INF, beta);
        }
        goodMove = this->pTransportation->getMove(board);
    }
    if (goodMove.id != -1)
    {
        board.doMove(goodMove);
        vlBest = -searchPV(depth - 1, -beta, -alpha);
        board.undoMove();
        bestMove = goodMove;
        if (vlBest >= beta)
        {
            type = BETA_TYPE;
        }
        if (vlBest > alpha)
        {
            type = EXACT_TYPE;
            alpha = vlBest;
        }
    }

    // 杀手启发
    if (type != BETA_TYPE)
    {
        int vl = -INF;
        MOVES killerAvailableMoves = this->pKiller->get(board);
        availableMoves = MovesGenerate::getMoves(board);
        for (const Move &move : killerAvailableMoves)
        {
            board.doMove(move);
            if (vlBest == -INF)
            {
                vl = -searchPV(depth - 1, -beta, -alpha);
            }
            else
            {
                vl = -searchCut(depth - 1, -alpha);
                if (vl > alpha && vl < beta)
                {
                    vl = -searchPV(depth - 1, -beta, -alpha);
                }
            }
            board.undoMove();
            if (vl > vlBest)
            {
                vlBest = vl;
                bestMove = move;
                if (vl >= beta)
                {
                    type = BETA_TYPE;
                    break;
                }
                if (vl > alpha)
                {
                    type = EXACT_TYPE;
                    alpha = vl;
                }
            }
        }
    }

    // 搜索
    if (type != BETA_TYPE)
    {
        int vl = -INF;
        if (availableMoves.size() == 0)
        {
            availableMoves = MovesGenerate::getMoves(board);
        }

        // 历史启发
        this->pHistory->sort(availableMoves);

        for (const Move &move : availableMoves)
        {
            board.doMove(move);

            if (vlBest == -INF)
            {
                vl = -searchPV(depth - 1, -beta, -alpha);
            }
            else
            {
                vl = -searchCut(depth - 1, -alpha);
                if (vl > alpha && vl < beta)
                {
                    vl = -searchPV(depth - 1, -beta, -alpha);
                }
            }

            board.undoMove();

            // 更新最佳值
            if (vl > vlBest)
            {
                vlBest = vl;
                bestMove = move;
                if (vl >= beta)
                {
                    type = BETA_TYPE;
                    break;
                }
                if (vl > alpha)
                {
                    type = EXACT_TYPE;
                    alpha = vl;
                }
            }
        }
    }

    // 结果
    if (bestMove.id == -1)
    {
        vlBest += board.distance;
    }
    else
    {
        this->pHistory->add(bestMove, depth);
        this->pTransportation->add(board, bestMove, vlBest, type, depth);
        if (type != ALPHA_TYPE)
        {
            this->pKiller->add(board, bestMove);
        }
    }

    return vlBest;
}

int Search::searchCut(int depth, int beta, bool banNullMove)
{
    nodecount++;

    // 检查将帅是否在棋盘上
    if (!board.isKingLive(board.team))
    {
        return -INF + board.distance;
    }

    // 置换表分数
    int vlHash = this->pTransportation->getValue(board, -INF, beta, depth);
    if (vlHash >= beta)
    {
        return vlHash;
    }

    // 静态搜索
    if (depth <= 0)
    {
        return Search::searchQ(beta - 1, beta);
    }

    // mate distance pruning
    TrickResult<int> trickResult = this->mateDistancePruning(beta - 1, beta);
    if (trickResult.isSuccess)
    {
        return trickResult.data[0];
    }

    // variables
    const bool mChecking = inCheck(board, board.team);

    // 验证上一步是否是将军着法
    this->setCheckingMove(mChecking);

    // 重复检测
    bool repeatResult = this->repeatCheck();
    if (repeatResult == true)
    {
        return INF - board.distance;
    }

    // tricks
    if (!mChecking)
    {
        // multi probCut and null pruning
        if (!banNullMove)
        {
            if (board.nullOkay())
            {
                board.doNullMove();
                int vl = -searchCut(depth - 3, -beta + 1, true);
                board.undoNullMove();
                if (vl >= beta)
                {
                    if (board.nullSafe())
                    {
                        return vl;
                    }
                    else if (searchCut(depth - 2, beta, true) >= beta)
                    {
                        return vl;
                    }
                }
            }
        }
    }

    // variables
    int vlBest = -INF;
    Move bestMove{};
    NODE_TYPE type = ALPHA_TYPE;
    int searchedCnt = 0;
    MOVES availableMoves;

    // 置换表着法
    Move goodMove = this->pTransportation->getMove(board);
    if (goodMove.id != -1)
    {
        board.doMove(goodMove);
        int vl = -searchCut(depth - 1, -beta + 1);
        board.undoMove();
        bestMove = goodMove;
        if (vl > vlBest)
        {
            vlBest = vl;
            if (vl >= beta)
            {
                type = BETA_TYPE;
            }
        }
    }

    // 搜索
    if (type != BETA_TYPE)
    {
        // 获取所有可行着法
        if (availableMoves.size() == 0)
        {
            availableMoves = MovesGenerate::getMoves(board);
        }

        // 历史启发
        this->pHistory->sort(availableMoves);

        for (const Move &move : availableMoves)
        {
            board.doMove(move);
            int vl = -searchCut(depth - 1, -beta + 1);
            board.undoMove();

            // 更新最佳值
            if (vl > vlBest)
            {
                vlBest = vl;
                bestMove = move;
                if (vl >= beta)
                {
                    type = BETA_TYPE;
                    break;
                }
            }
            searchedCnt++;
        }
    }

    // 结果
    if (bestMove.id == -1)
    {
        vlBest += board.distance;
    }
    else
    {
        this->pHistory->add(bestMove, depth);
        this->pTransportation->add(board, bestMove, vlBest, type, depth);
        if (type != ALPHA_TYPE)
        {
            this->pKiller->add(board, bestMove);
        }
    }

    return vlBest;
}

int Search::searchQ(int alpha, int beta, int leftDistance)
{
    // 暂时关闭
    nodecount++;

    // 检查将帅是否在棋盘上
    if (!board.isKingLive(board.team))
    {
        return -INF + board.distance;
    }

    // 返回评估结果
    if (leftDistance <= 0)
    {
        return board.evaluate();
    }

    // mate distance pruning
    TrickResult<int> trickresult = this->mateDistancePruning(alpha, beta);
    if (trickresult.isSuccess)
    {
        return trickresult.data[0];
    }

    // variables
    const bool mChecking = inCheck(board, board.team);
    int vlBest = -INF;

    // 验证上一步是否是将军着法
    this->setCheckingMove(mChecking);

    // null and delta pruning
    TrickResult<int> nullDeltaResult = this->nullAndDeltaPruning(mChecking, alpha, beta, vlBest);
    if (nullDeltaResult.isSuccess)
    {
        return nullDeltaResult.data[0];
    }

    // 重复检测
    bool repeatResult = this->repeatCheck();
    if (repeatResult == true)
    {
        return INF - board.distance;
    }

    // 搜索
    MOVES availableMoves;
    if (mChecking)
    {
        availableMoves = MovesGenerate::getMoves(board);
        pHistory->sort(availableMoves);
        leftDistance = std::min<int>(leftDistance, QUIESCENCE_EXTEND_DEPTH_WHEN_FACE_CHECKING);
    }
    else
    {
        MOVES unordered = MovesGenerate::getCaptureMoves(board);
        CaptureSort::sortCaptureMoves(board, availableMoves);
    }

    // variables
    Move bestMove{};

    // 搜索
    for (const Move &move : availableMoves)
    {
        board.doMove(move);
        int vl = -Search::searchQ(-beta, -alpha, leftDistance - 1);
        board.undoMove();
        if (vl > vlBest)
        {
            if (vl >= beta)
            {
                return vl;
            }
            vlBest = vl;
            bestMove = move;
            if (vl > alpha)
            {
                alpha = vl;
            }
        }
    }

    // 结果
    if (vlBest == -INF)
    {
        vlBest += board.distance;
    }
    return vlBest;
}
