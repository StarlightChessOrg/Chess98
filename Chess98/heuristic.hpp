#pragma once
#include "base.hpp"
#include "board.hpp"
#include "hash.hpp"
#include "utils.hpp"
#include "moves.hpp"

// 历史启发
class HistoryHeuristic
{
public:
    HistoryHeuristic() = default;
    void sort(MOVES &moves) const;
    void add(Move move, int depth);
    void reset();

    std::array<std::array<std::array<int, 90>, 90>, 2> historyTable{};

    int toIndex(int x, int y) const
    {
        return 10 * x + y;
    }
};

void HistoryHeuristic::sort(MOVES &moves) const
{
    for (Move &move : moves)
    {
        if (move.moveType <= HISTORY)
        {
            int pos1 = this->toIndex(move.x1, move.y1);
            int pos2 = this->toIndex(move.x2, move.y2);
            int teamID = (move.attacker.team() + 1) >> 1;
            assert(teamID >= 0 && teamID <= 1);
            move.moveType = HISTORY;
            move.val = historyTable[teamID][pos1][pos2];
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
}

void HistoryHeuristic::add(Move move, int depth)
{
    int pos1 = this->toIndex(move.x1, move.y1);
    int pos2 = this->toIndex(move.x2, move.y2);
    int teamID = (move.attacker.team() + 1) >> 1;
    assert(teamID >= 0 && teamID <= 1);
    assert(depth >= 0);
    this->historyTable.at(teamID).at(pos1).at(pos2) += (depth << 1);
}

void HistoryHeuristic::reset()
{
    std::memset(this->historyTable.data(), 0, sizeof(this->historyTable));
}

// 杀手启发
class KillerTable
{
public:
    KillerTable() = default;
    void reset();
    void add(Board &board, Move move);
    MOVES get(Board &board);

    std::array<std::array<Move, 2>, 128> killerMoves{};
};

void KillerTable::reset()
{
    this->killerMoves.fill({});
}

MOVES KillerTable::get(Board &board)
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

void KillerTable::add(Board &board, Move move)
{
    std::array<Move, 2> &moves = this->killerMoves[board.distance];
    moves[1] = moves[0];
    moves[0] = move;
}

// 吃子启发
void captureSort(Board &board, MOVES &moves)
{
    std::sort(moves.begin(), moves.end(), [](Move &first, Move &second) -> bool
              {
        if (first.moveType != second.moveType)
        {
            return first.moveType > second.moveType;
        }
        return first.val > second.val; });
}

// 置换表启发
class TransportationTable
{
public:
    TransportationTable(int hashLevel = 23)
    {
        this->hashSize = (1 << hashLevel);
        this->hashMask = this->hashSize - 1;
        this->pList.resize(this->hashSize);
    }

    void reset();
    void add(Board &board, Move goodMove, int vl, int type, int depth);
    int getValue(Board &board, int vlApha, int vlBeta, int depth);
    Move getMove(Board &board);
    int vlAdjust(int vl, int nDistance);

    std::vector<TransItem> pList{};
    int hashMask = 0;
    int hashSize = 0;
};

void TransportationTable::reset()
{
    this->pList = std::vector<TransItem>{};
    this->pList.resize(this->hashSize);
}

int TransportationTable::vlAdjust(int vl, int nDistance)
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
}

void TransportationTable::add(Board &board, Move goodMove, int vl, int type, int depth)
{
    const int pos = static_cast<uint32_t>(board.hashKey) & static_cast<uint32_t>(this->hashMask);
    TransItem &t = this->pList.at(pos);
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

int TransportationTable::getValue(Board &board, int vlApha, int vlBeta, int depth)
{
    const int pos = static_cast<uint32_t>(board.hashKey) & static_cast<uint32_t>(this->hashMask);
    TransItem &t = this->pList.at(pos);
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

Move TransportationTable::getMove(Board &board)
{
    const int pos = static_cast<uint32_t>(board.hashKey) & static_cast<uint32_t>(this->hashMask);
    TransItem &t = this->pList.at(pos);
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

// SEE
MOVES SEE(Board &board, MOVES &moves)
{
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
