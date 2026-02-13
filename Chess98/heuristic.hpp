#pragma once
#include "board.hpp"
#include "movesgen.hpp"

using HISTORY_TABLE = std::array<std::array<std::array<int, 90>, 90>, 2>;
using KILLER_TABLE = std::array<std::array<Move, 2>, 64>;
using TRANS_TABLE = std::vector<TtItem>;

// 历史启发
class HistoryTable {
public:
    HistoryTable() = default;
    void reset()
    {
        history_table = std::make_unique<HISTORY_TABLE>();
    }

private:
    std::unique_ptr<HISTORY_TABLE> history_table = std::make_unique<HISTORY_TABLE>();

public:
    void add(Move& move, int& depth)
    {
        const int pos1 = 10 * move.x1 + move.y1;
        const int pos2 = 10 * move.x2 + move.y2;
        const int team = move.attacker.team == RED ? 0 : 1;
        history_table->at(team)[pos1][pos2] += history_logic(depth);
    }

    void sort(MOVES& moves) const
    {
        for (Move& move : moves) {
            const int pos1 = 10 * move.x1 + move.y1;
            const int pos2 = 10 * move.x2 + move.y2;
            const int team = move.attacker.team == RED ? 0 : 1;
            move.move_type = HISTORY;
            move.val = (*history_table)[team][pos1][pos2];
        }
        std::sort(moves.begin(), moves.end(), [](Move& a, Move& b) { return a.val > b.val; });
    }

private:
    int history_logic(int& depth) const
    {
        return depth * depth;
    }
};

// 杀手启发
class KillerTable {
public:
    KillerTable() = default;

    void reset()
    {
        killer_table = std::make_unique<KILLER_TABLE>();
    }

protected:
    std::unique_ptr<KILLER_TABLE> killer_table = std::make_unique<KILLER_TABLE>();

public:
    void set(Board& board, Move move)
    {
        std::array<Move, 2>& moves = (*killer_table)[board.distance];
        moves[1] = moves[0];
        moves[0] = move;
    }

    MOVES get(Board& board) const
    {
        MOVES results {};
        for (const Move& move : (*killer_table)[board.distance]) {
            if (board.is_valid_move(move)) {
                results.emplace_back(move);
            }
        }
        return results;
    }
};

// 置换表启发
class Tt {
public:
    Tt(int hash_level = 16)
    {
        hash_size = pow(2, hash_level);
        hash_mask = pow(2, hash_level) - 1;
        trans_table.resize(hash_size);
    }

    void reset()
    {
        for (TtItem& item : trans_table) {
            item = TtItem {};
        }
    }

protected:
    TRANS_TABLE trans_table {};
    int hash_mask = 0;
    int hash_size = 0;

public:
    void set(Board& board, Move& goodMove, int vl, NODE_TYPE type, int depth)
    {
        const int pos = static_cast<uint32>(board.hashKey) & static_cast<uint32>(this->hash_mask);
        TtItem& item = trans_table[pos];
        if (item.hash_lock == 0) {
            item.hash_lock = board.hash_lock;
            if (type == EXACT_TYPE) {
                item.exactDepth = depth;
                item.vlExact = vl;
                item.exact_move = goodMove;
            } else if (type == BETA_TYPE) {
                item.betaDepth = depth;
                item.vlBeta = vl;
                item.beta_move = goodMove;
            } else if (type == ALPHA_TYPE) {
                item.alphaDepth = depth;
                item.vlAlpha = vl;
                item.alpha_move = goodMove;
            }
        } else if (item.hash_lock == board.hash_lock) {
            if (type == EXACT_TYPE && depth > item.exactDepth) {
                item.exactDepth = depth;
                item.vlExact = vl;
                item.exact_move = goodMove;
            } else if (type == BETA_TYPE && ((depth > item.betaDepth) || (depth == item.betaDepth && vl > item.vlBeta))) {
                item.betaDepth = depth;
                item.vlBeta = vl;
                item.beta_move = goodMove;
            } else if (type == ALPHA_TYPE && ((depth > item.alphaDepth) || (depth == item.alphaDepth && vl < item.vlAlpha))) {
                item.alphaDepth = depth;
                item.vlAlpha = vl;
                item.alpha_move = goodMove;
            }
        }
    }

    int getVl(Board& board, int vlApha, int vlBeta, int depth) const
    {
        const int pos = static_cast<uint32>(board.hashKey) & static_cast<uint32>(this->hash_mask);
        const TtItem& t = this->trans_table[pos];
        if (t.hash_lock == board.hash_lock) {
            if (t.exactDepth >= depth) {
                return vl_adjust(t.vlExact, board.distance);
            } else if (t.betaDepth >= depth && t.vlBeta >= vlBeta) {
                return t.vlBeta;
            } else if (t.alphaDepth >= depth && t.vlAlpha <= vlApha) {
                return t.vlAlpha;
            }
        }
        return -INF;
    }

    Move getMove(Board& board) const
    {
        const int pos = static_cast<int>(board.hashKey) & static_cast<int>(this->hash_mask);
        const TtItem& t = this->trans_table[pos];
        if (t.hash_lock == board.hash_lock) {
            if (board.is_valid_move(t.exact_move)) {
                return t.exact_move;
            } else if (board.is_valid_move(t.beta_move)) {
                return t.beta_move;
            } else if (board.is_valid_move(t.alpha_move)) {
                return t.alpha_move;
            }
        }
        return Move {};
    }

protected:
    int vl_adjust(int vl, int nDistance) const
    {
        return vl + (vl <= -BAN ? nDistance : (vl >= BAN ? -nDistance : 0));
    }
};
