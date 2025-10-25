#pragma once
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
        this->history->reset();
        this->killer->reset();
        this->tt->reset();
    }

    static const int QUIESCENCE_EXTEND_DEPTH = 64;
    static const int QUIESCENCE_EXTEND_DEPTH_WHEN_FACE_CHECKING = 8;

public:
    Board board{};
    MOVES rootMoves{};
    std::unique_ptr<HistoryTable> history = std::make_unique<HistoryTable>();
    std::unique_ptr<KillerTable> killer = std::make_unique<KillerTable>();
    std::unique_ptr<Tt> tt = std::make_unique<Tt>();

public:
    Result searchMain(int maxDepth, int maxTime);

protected:
    Result searchOpenBook();
    Result searchRoot(int depth);
    int searchPV(int depth, int alpha, int beta);
    int searchCut(int depth, int beta, bool banNullMove = false);
    int searchQ(int alpha, int beta, int leftDistance = QUIESCENCE_EXTEND_DEPTH);

protected:
    void validateCheckingMove(bool mChecking)
    {
        if (mChecking && !board.historyMoves.empty())
        {
            board.historyMoves.back().isCheckingMove = true;
        }
    }

    Trick nullAndDeltaPruning(bool mChecking, int& alpha, int& beta, int& vlBest) const
    {
        if (!mChecking)
        {
            int vl = board.evaluate();
            if (vl >= beta)
            {
                return Trick{ vl };
            }
            vlBest = vl;
            if (vl > alpha)
            {
                alpha = vl;
            }
        }
        return {};
    }

    Trick mateDistancePruning(int alpha, int& beta) const
    {
        const int vlDistanceMate = INF - board.distance;
        if (vlDistanceMate < beta)
        {
            beta = vlDistanceMate;
            if (alpha >= vlDistanceMate)
            {
                return Trick{ vlDistanceMate };
            }
        }
        return {};
    }

    Trick futilityPruning(int alpha, int beta, int depth) const
    {
        if (depth == 1)
        {
            int vl = board.evaluate();
            if (vl <= beta - FUTILITY_PRUNING_MARGIN || vl >= beta + FUTILITY_PRUNING_MARGIN)
            {
                return Trick{ vl };
            }
        }
        return {};
    }

    Trick multiProbCut(SEARCH_TYPE searchType, int alpha, int beta, int depth)
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
                return { beta };
            }
            else if (searchType == PV && this->searchCut(depth - 2, lowerBound + 1) <= lowerBound)
            {
                return { alpha };
            }
        }

        return {};
    }

    bool repeatCheck() const
    {
        const Board& board = this->board;
        const MOVES& history = board.historyMoves;
        const size_t size = history.size();
        if (size >= 5)
        {
            const Move& ply1 = history[size_t(size - 1)];
            const Move& ply2 = history[size_t(size - 2)];
            const Move& ply3 = history[size_t(size - 3)];
            const Move& ply4 = history[size_t(size - 4)];
            const Move& ply5 = history[size_t(size - 5)];
            // 判断是否出现重复局面, 没有则直接false
            // 试想如下重复局面：（格式：plyX: x1y1x2y2）
            // ply1: 0001, ply2: 0908, ply3: 0100, ply4: 0809, ply5: 0001
            const bool isRepeat = (ply1 == ply5 &&
                ply1.startpos == ply3.endpos &&
                ply1.endpos == ply3.startpos &&
                ply2.startpos == ply4.endpos &&
                ply2.endpos == ply4.startpos);
            if (!isRepeat)
            {
                return false;
            }

            // 长将在任何情况下都会判负
            // 由于性能原因, isCheckingMove是被延迟设置的, ply1可能还没有被设成checkingMove
            // 但是若判定了循环局面, ply1必然等于ply5
            // 若ply5和ply3都是将军着法, 且出现循环局面, 则直接判定违规
            if (ply5.isCheckingMove == true && ply3.isCheckingMove == true)
            {
                return true;
            }
            // 长捉情况比较特殊
            // 只有车、马、炮能作为长捉的发起者
            // 发起者不断捉同一个子, 判负
            if (abs(ply1.attacker.pieceid) == R_ROOK ||
                abs(ply1.attacker.pieceid) == R_KNIGHT ||
                abs(ply1.attacker.pieceid) == R_CANNON)
            {
                const Piece& attacker = ply1.attacker;
                const Piece& captured = ply2.attacker;
                // 车
                if (abs(attacker.pieceid) == R_ROOK)
                {
                    if (ply5.x2 == ply4.x1)
                    {
                        UINT32 bitlineX = board.getBitLineX(ply5.x2);
                        REGION_ROOK regionX = board.bitboard->getRookRegion(bitlineX, attacker.y, 9);
                        if (board.piecePosition(ply5.x2, regionX[1]).pieceIndex == captured.pieceIndex ||
                            board.piecePosition(ply5.x2, regionX[0]).pieceIndex == captured.pieceIndex)
                        {
                            return true;
                        }
                    }
                    else if (ply5.y2 == ply4.y1)
                    {
                        UINT32 bitlineY = board.getBitLineY(ply5.y2);
                        REGION_ROOK regionY = board.bitboard->getRookRegion(bitlineY, attacker.x, 8);
                        if (board.piecePosition(regionY[0], ply5.y2).pieceIndex == captured.pieceIndex ||
                            board.piecePosition(regionY[1], ply5.y2).pieceIndex == captured.pieceIndex)
                        {
                            return true;
                        }
                    }
                }
                // 炮
                else if (abs(attacker.pieceid) == R_CANNON)
                {
                    if (ply5.x2 == ply4.x1)
                    {
                        UINT32 bitlineX = board.getBitLineX(ply5.x2);
                        REGION_CANNON regionX = board.bitboard->getCannonRegion(bitlineX, attacker.y, 9);
                        if (board.piecePosition(ply5.x2, regionX[1]).pieceIndex == captured.pieceIndex ||
                            board.piecePosition(ply5.x2, regionX[3]).pieceIndex == captured.pieceIndex)
                        {
                            return true;
                        }
                    }
                    else if (ply5.y2 == ply4.y1)
                    {
                        UINT32 bitlineY = board.getBitLineY(ply5.y2);
                        REGION_CANNON regionY = board.bitboard->getCannonRegion(bitlineY, attacker.x, 8);
                        if (board.piecePosition(regionY[0], ply5.y2).pieceIndex == captured.pieceIndex ||
                            board.piecePosition(regionY[3], ply5.y2).pieceIndex == captured.pieceIndex)
                        {
                            return true;
                        }
                    }
                }
                // 马
                else if (abs(attacker.pieceid) == R_KNIGHT)
                {
                    if (
                        (attacker.x + 1 == captured.x && attacker.y + 2 == captured.y) ||
                        (attacker.x - 1 == captured.x && attacker.y + 2 == captured.y) ||
                        (attacker.x + 1 == captured.x && attacker.y - 2 == captured.y) ||
                        (attacker.x - 1 == captured.x && attacker.y - 2 == captured.y) ||
                        (attacker.x + 2 == captured.x && attacker.y + 1 == captured.y) ||
                        (attacker.x - 2 == captured.x && attacker.y + 1 == captured.y) ||
                        (attacker.x + 2 == captured.x && attacker.y - 1 == captured.y) ||
                        (attacker.x - 2 == captured.x && attacker.y - 1 == captured.y)) // 这里有点担心, 但是我想不到什么局面
                    {
                        return true;
                    }
                }
            }
        }
        return false;
    }
};

Result Search::searchMain(int maxDepth, int maxTime = 3)
{
    // 预制条件检查
    this->reset();
    if (!board.isKingLive(RED) || !board.isKingLive(BLACK))
    {
        // 将帅是否在棋盘上
        exit(0);
    }
    else if (this->repeatCheck())
    {
        // 是否重复局面
        Move move = board.historyMoves[size_t(board.historyMoves.size() - 4)];
        return Result{ move, INF };
    }

    // 开局库
    Result openbookResult = Search::searchOpenBook();
    if (openbookResult.vl != -1)
    {
        std::cout << "Find a great move from OpenBook!" << std::endl;
        return openbookResult;
    }

    // 输出局面信息
    std::cout << "situation: " << pieceidmapToFen(board.pieceidMap, board.team) << std::endl;
    std::cout << "evaluate: " << board.evaluate() << std::endl;

    // 搜索
    this->rootMoves = MovesGenerate::getMoves(board);
    Result bestNode = Result(Move(), 0);
    auto start = std::chrono::high_resolution_clock::now();
    for (int depth = 1; depth <= maxDepth; depth++)
    {
        bestNode = searchRoot(depth);

        auto end = std::chrono::high_resolution_clock::now();
        int duration = int(std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());

        // log
        std::cout << " depth: " << depth;
        std::cout << " vl: " << bestNode.vl;
        std::cout << " moveid: " << bestNode.move.id;
        std::cout << " duration(ms): " << duration;
        std::cout << std::endl;

        // timeout break
        if (duration >= maxTime * 1000 / 3)
        {
            break;
        }
    }

    // 防止没有可行着法
    if (bestNode.move.id == -1)
    {
        const Piece& king = board.getPieceReg(board.team == RED ? R_KING : B_KING);
        bestNode.move = MovesGenerate::generateMovesOn(board, king.x, king.y)[0];
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
        int nLen = 0;
        bool isEdit = false;
        std::string filename;

        bool open(const char* szFileName, bool bEdit = false)
        {
            isEdit = bEdit;
            filename = szFileName;
            std::ifstream ifs(filename, std::ios::binary | std::ios::ate);
            if (!ifs.is_open())
                return false;
            nLen = int(ifs.tellg() / sizeof(BookStruct));
            return true;
        }

        void read(BookStruct& bk, int nMid) const
        {
            std::ifstream ifs(filename, std::ios::binary);
            if (!ifs.is_open())
                return;
            ifs.seekg(nMid * sizeof(BookStruct), std::ios::beg);
            ifs.read(reinterpret_cast<char*>(&bk), sizeof(BookStruct));
        }

        void write(const BookStruct& bk, int nMid) const
        {
            if (!isEdit)
                return;
            std::fstream fstr(filename, std::ios::in | std::ios::out | std::ios::binary);
            if (!fstr.is_open())
                return;
            fstr.seekp(nMid * sizeof(BookStruct), std::ios::beg);
            fstr.write(reinterpret_cast<const char*>(&bk), sizeof(BookStruct));
        }
    };

    std::function<int(BookStruct&, int32)> bookPosCmp = [](BookStruct& bk, int32 hashLock) -> int
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
        return Result{ Move{}, -1 };
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
        return Result{ Move{}, -1 };
    }

    // 如果找到局面, 则向前查找第一个着法
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
    std::sort(bookMoves.begin(), bookMoves.end(), [](Move& a, Move& b)
        { return a.val > b.val; });

    std::srand(unsigned(std::time(0)));

    int vlSum = 0;
    for (Move& move : bookMoves)
    {
        vlSum += move.val;
    }
    int vlRandom = std::rand() % vlSum;

    Move bookMove;
    for (Move& move : bookMoves)
    {
        vlRandom -= move.val;
        if (vlRandom < 0)
        {
            bookMove = move;
            break;
        }
    }

    bookMove.attacker = board.piecePosition(bookMove.x1, bookMove.y1);
    bookMove.captured = board.piecePosition(bookMove.x2, bookMove.y2);

    return isValidMoveInSituation(board, bookMove) ? Result{ bookMove, 1 } : Result{ Move{}, -1 };
}

Result Search::searchRoot(int depth)
{
    Move bestMove{};
    int vl = -INF;
    int vlBest = -INF;

    // 搜索
    for (const Move& move : rootMoves)
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
        }

        board.undoMove();
    }

    // 记录数据, 为杀棋添加distance
    if (bestMove.id == -1)
    {
        vlBest += board.distance;
    }
    else
    {
        this->history->add(bestMove, depth);
        this->tt->set(board, bestMove, vlBest, EXACT_TYPE, depth);
    }

    this->history->sort(rootMoves);

    return Result{ bestMove, vlBest };
}

int Search::searchPV(int depth, int alpha, int beta)
{
    // 检查将帅是否在棋盘上
    if (!board.isKingLive(board.team))
    {
        return -INF + board.distance;
    }

    // 静态搜索
    if (depth <= 0)
    {
        int vl = Search::searchQ(alpha, beta);
        return vl;
    }

    // mate distance pruning
    Trick result = this->mateDistancePruning(alpha, beta);
    if (result.success)
    {
        return result.data;
    }

    // variables
    const bool mChecking = inCheck(board, board.team);
    this->validateCheckingMove(mChecking);

    if (!mChecking)
    {
        // null and delta pruning
        int vlBest = -INF;
        Trick trickResult = this->nullAndDeltaPruning(mChecking, alpha, beta, vlBest);
        if (trickResult.success)
        {
            return trickResult.data;
        }
        // futility pruning
        trickResult = this->futilityPruning(alpha, beta, depth);
        if (trickResult.success)
        {
            return trickResult.data;
        }
    }

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
    Move goodMove = this->tt->getMove(board);
    if (goodMove.id == -1 && depth >= 2)
    {
        if (searchPV(depth / 2, alpha, beta) <= alpha)
        {
            searchPV(depth / 2, -INF, beta);
        }
        goodMove = this->tt->getMove(board);
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
        MOVES killerAvailableMoves = this->killer->get(board);
        for (const Move& move : killerAvailableMoves)
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
        this->history->sort(availableMoves);

        for (const Move& move : availableMoves)
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
        this->history->add(bestMove, depth);
        this->tt->set(board, bestMove, vlBest, type, depth);
        if (type != ALPHA_TYPE)
        {
            this->killer->set(board, bestMove);
        }
    }

    return vlBest;
}

int Search::searchCut(int depth, int beta, bool banNullMove)
{
    // 检查将帅是否在棋盘上
    if (!board.isKingLive(board.team))
    {
        return -INF + board.distance;
    }

    // 置换表分数
    int vlHash = this->tt->getVl(board, -INF, beta, depth);
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
    Trick trickResult = this->mateDistancePruning(beta - 1, beta);
    if (trickResult.success)
    {
        return trickResult.data;
    }

    // variables
    const bool mChecking = inCheck(board, board.team);

    // 验证上一步是否是将军着法
    this->validateCheckingMove(mChecking);

    // 重复检测
    bool repeatResult = this->repeatCheck();
    if (repeatResult == true)
    {
        return INF - board.distance;
    }

    if (!mChecking)
    {
        // null pruning
        if (!banNullMove)
        {
            if (board.nullOkay())
            {
                board.doNullMove();
                int vl = -searchCut(depth - 2, -beta + 1, true);
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
    Move goodMove = this->tt->getMove(board);
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
        this->history->sort(availableMoves);

        for (const Move& move : availableMoves)
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
        this->history->add(bestMove, depth);
        this->tt->set(board, bestMove, vlBest, type, depth);
        if (type != ALPHA_TYPE)
        {
            this->killer->set(board, bestMove);
        }
    }

    return vlBest;
}

int Search::searchQ(int alpha, int beta, int leftDistance)
{
    // 检查将帅是否在棋盘上
    if (!board.isKingLive(board.team))
    {
        return -INF + board.distance;
    }

    // evaluate
    if (leftDistance <= 0)
    {
        return board.evaluate();
    }

    // mdp
    Trick trickresult = this->mateDistancePruning(alpha, beta);
    if (trickresult.success)
    {
        return trickresult.data;
    }

    const bool mChecking = inCheck(board, board.team);
    this->validateCheckingMove(mChecking);
    int vlBest = -INF;

    // null and delta pruning
    Trick nullDeltaResult = this->nullAndDeltaPruning(mChecking, alpha, beta, vlBest);
    if (nullDeltaResult.success)
    {
        return nullDeltaResult.data;
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
        history->sort(availableMoves);
        leftDistance = std::min<int>(leftDistance, Search::QUIESCENCE_EXTEND_DEPTH_WHEN_FACE_CHECKING);
    }
    else
    {
        availableMoves = MovesGenerate::getCaptureMoves(board);
    }
    Move bestMove{};

    // 搜索
    for (const Move& move : availableMoves)
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
