#pragma once
#include "base.hpp"
#include "board.hpp"
#include "hash.hpp"
#include "utils.hpp"
#include "moves.hpp"

// 历史启发
class HistoryTable
{
public:
    HistoryTable() = default;
    void reset()
    {
        std::memset(this->historyTable.data(), 0, sizeof(this->historyTable));
    };

protected:
    std::array<std::array<std::array<int, 90>, 90>, 2> historyTable{};

public:
    void add(Move move, int depth)
    {
        int pos1 = 10 * move.x1 + move.y1;
        int pos2 = 10 * move.x2 + move.y2;
        int teamID = (move.attacker.team() + 1) >> 1;
        this->historyTable.at(teamID).at(pos1).at(pos2) += (depth << 1);
    };

    void sort(MOVES &moves) const
    {
        for (Move &move : moves)
        {
            if (move.moveType <= HISTORY)
            {
                int pos1 = 10 * move.x1 + move.y1;
                int pos2 = 10 * move.x2 + move.y2;
                int teamID = (move.attacker.team() + 1) >> 1;
                move.moveType = HISTORY;
                move.val = this->historyTable[teamID][pos1][pos2];
            }
        }
        // vl history compare
        std::sort(moves.begin(), moves.end(), [](Move &first, Move &second) -> bool
                  {
        if (first.moveType != second.moveType)
        {
            return first.moveType > second.moveType;
        }
        return first.val > second.val; });
    };
};

// 杀手启发
class KillerTable
{
public:
    KillerTable() = default;
    void reset()
    {
        this->killerMoves.fill({});
    }

protected:
    std::array<std::array<Move, 2>, 128> killerMoves{};

public:
    void add(Board &board, Move move)
    {
        std::array<Move, 2> &moves = this->killerMoves[board.distance];
        moves[1] = moves[0];
        moves[0] = move;
    }

    MOVES get(Board &board) const
    {
        MOVES results{};
        for (const Move &move : this->killerMoves[board.distance])
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
class TransportationTable
{
public:
    TransportationTable(int hashLevel = 16)
    {
        this->hashSize = (1 << hashLevel);
        this->hashMask = this->hashSize - 1;
        this->items.resize(this->hashSize);
    }

    void reset()
    {
        std::memset(this->items.data(), 0, sizeof(this->items));
    }

protected:
    std::vector<TransItem> items{};
    int hashMask = 0;
    int hashSize = 0;

public:
    void add(Board &board, Move goodMove, int vl, int type, int depth)
    {
        const int pos = static_cast<uint32_t>(board.hashKey) & static_cast<uint32_t>(this->hashMask);
        TransItem &t = this->items.at(pos);
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

    int getValue(Board &board, int vlApha, int vlBeta, int depth) const
    {
        const int pos = static_cast<uint32_t>(board.hashKey) & static_cast<uint32_t>(this->hashMask);
        const TransItem &t = this->items.at(pos);
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

    Move getMove(Board &board) const
    {
        const int pos = static_cast<uint32_t>(board.hashKey) & static_cast<uint32_t>(this->hashMask);
        const TransItem &t = this->items.at(pos);
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

    int vlAdjust(int vl, int nDistance) const
    {
        if (vl <= -BAN)
        {
            return vl + nDistance;
        }
        if (vl >= BAN)
        {
            return vl - nDistance;
        }
        return vl;
    };
};

// 吃子启发
class CaptureSort
{
public:
    static void sort(Board &board, MOVES &moves)
    {
        MOVES result{};
        result.reserve(64);

        const std::map<PIECEID, int> weightPairs{
            {R_KING, 5},
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

        moves = result;
    }
};
