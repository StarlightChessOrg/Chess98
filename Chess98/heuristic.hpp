#pragma once
#include "board.hpp"
#include "movesgen.hpp"

// 历史启发
class HistoryTable
{
public:
    HistoryTable() = default;
    void reset() { this->historyTable = std::make_unique<HISTORY_TABLE>(); }

protected:
    using HISTORY_TABLE = std::array<std::array<std::array<int, 90>, 90>, 2>;
    std::unique_ptr<HISTORY_TABLE> historyTable = std::make_unique<HISTORY_TABLE>();

public:
    void add(Move move, int depth)
    {
        const int pos1 = 10 * move.x1 + move.y1;
        const int pos2 = 10 * move.x2 + move.y2;
        const int team = move.attacker.team == RED ? 0 : 1;
        this->historyTable->at(team)[pos1][pos2] += depth * depth;
    }

    void sort(MOVES& moves) const
    {
        for (Move& move : moves)
        {
            const int pos1 = 10 * move.x1 + move.y1;
            const int pos2 = 10 * move.x2 + move.y2;
            const int team = move.attacker.team == RED ? 0 : 1;
            move.moveType = HISTORY;
            move.val = this->historyTable->at(team)[pos1][pos2];
        }
        std::sort(moves.begin(), moves.end(), [](Move& m1, Move& m2) -> bool { return m1.val > m2.val; });
    }
};

// 杀手启发
class KillerTable
{
public:
    KillerTable() = default;
    void reset() { this->killerMoves = std::make_unique<KILLER_MOVES>(); }

protected:
    using KILLER_MOVES = std::array<std::array<Move, 2>, 64>;
    std::unique_ptr<KILLER_MOVES> killerMoves = std::make_unique<KILLER_MOVES>();

public:
    void set(Board& board, Move move)
    {
        std::array<Move, 2>& moves = this->killerMoves->at(board.distance);
        moves[1] = moves[0];
        moves[0] = move;
    }

    MOVES get(Board& board) const
    {
        MOVES results{};
        for (const Move& move : this->killerMoves->at(board.distance))
        {
            if (isValidMoveInSituation(board, move))
            {
                results.emplace_back(move);
            }
        }
        return results;
    }
};

// 置换表启发
class Tt
{
public:
    Tt(uint64 hashLevel = 16)
    {
        this->hashSize = 1 << hashLevel;
        this->hashMask = (1 << hashLevel) - 1;
        this->items.resize(1ULL << hashLevel);
    }
    void reset()
    {
        for (TransItem& item : this->items)
        {
            item = TransItem{};
        }
    }

protected:
    using HASH_ITEMS = std::vector<TransItem>;
    HASH_ITEMS items{};
    int hashMask = 0;
    int hashSize = 0;

protected:
    int vlAdjust(int vl, int nDistance) const { return vl + (vl <= -BAN ? nDistance : (vl >= BAN ? -nDistance : 0)); }

public:
    void set(Board& board, Move goodMove, int vl, NODE_TYPE type, int depth)
    {
        const int pos = static_cast<uint32_t>(board.hashKey) & static_cast<uint32_t>(this->hashMask);
        TransItem& t = this->items[pos];
        if (t.hashLock == 0)
        {
            t.hashLock = board.hashLock;
            if (type == EXACT_TYPE)
            {
                t.exactDepth = depth;
                t.vlExact = vl;
                t.exactMove = goodMove;
            }
            else if (type == BETA_TYPE)
            {
                t.betaDepth = depth;
                t.vlBeta = vl;
                t.betaMove = goodMove;
            }
            else if (type == ALPHA_TYPE)
            {
                t.alphaDepth = depth;
                t.vlAlpha = vl;
                t.alphaMove = goodMove;
            }
        }
        else if (t.hashLock == board.hashLock)
        {
            if (type == EXACT_TYPE && depth > t.exactDepth)
            {
                t.exactDepth = depth;
                t.vlExact = vl;
                t.exactMove = goodMove;
            }
            else if (type == BETA_TYPE && ((depth > t.betaDepth) || (depth == t.betaDepth && vl > t.vlBeta)))
            {
                t.betaDepth = depth;
                t.vlBeta = vl;
                t.betaMove = goodMove;
            }
            else if (type == ALPHA_TYPE && ((depth > t.alphaDepth) || (depth == t.alphaDepth && vl < t.vlAlpha)))
            {
                t.alphaDepth = depth;
                t.vlAlpha = vl;
                t.alphaMove = goodMove;
            }
        }
    }

    int getVl(Board& board, int vlApha, int vlBeta, int depth) const
    {
        const int pos = static_cast<uint32_t>(board.hashKey) & static_cast<uint32_t>(this->hashMask);
        const TransItem& t = this->items[pos];
        if (t.hashLock == board.hashLock)
        {
            if (t.exactDepth >= depth)
            {
                return vlAdjust(t.vlExact, board.distance);
            }
            else if (t.betaDepth >= depth && t.vlBeta >= vlBeta)
            {
                return t.vlBeta;
            }
            else if (t.alphaDepth >= depth && t.vlAlpha <= vlApha)
            {
                return t.vlAlpha;
            }
        }
        return -INF;
    }

    Move getMove(Board& board) const
    {
        const int pos = static_cast<uint32_t>(board.hashKey) & static_cast<uint32_t>(this->hashMask);
        const TransItem& t = this->items[pos];
        if (t.hashLock == board.hashLock)
        {
            if (isValidMoveInSituation(board, t.exactMove))
            {
                return t.exactMove;
            }
            else if (isValidMoveInSituation(board, t.betaMove))
            {
                return t.betaMove;
            }
            else if (isValidMoveInSituation(board, t.alphaMove))
            {
                return t.alphaMove;
            }
        }
        return Move{};
    }
};
