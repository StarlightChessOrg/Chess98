#pragma once
#include "base.hpp"
#include "hash.hpp"
#include "board.hpp"

// 历史启发
enum moveType
{
    normal = 0,
    capture = 1,
    history = 2,
    killer = 3,
    hash = 4
};

class HistoryHeuristic
{
public:
    void init() const;
    void sort(MOVES &moves) const;
    void add(Move move, int depth);

    static bool vlHisCompare(Move &first, Move &second)
    {
        if (first.moveType != second.moveType)
        {
            return first.moveType > second.moveType;
        }
        return first.val > second.val;
    }

    mutable int historyTable[90][90];
};

/// @brief 二维坐标转索引
/// @param x
/// @param y
/// @return
int toIndex(int x, int y)
{
    return 10 * x + y;
}

/// @brief 初始化
void HistoryHeuristic::init() const
{
    std::memset(this->historyTable, 0, sizeof(int) * 90 * 90);
}

/// @brief 历史表排序
/// @param moves
/// @return
void HistoryHeuristic::sort(MOVES &moves) const
{
    for (Move &move : moves)
    {
        if (move.moveType <= history)
        {
            int pos1 = toIndex(move.x1, move.y1);
            int pos2 = toIndex(move.x2, move.y2);
            move.moveType = history;
            move.val = historyTable[pos1][pos2];
        }
    }
    std::sort(moves.begin(), moves.end(), vlHisCompare);
}

/// @brief 在历史表中增加一个历史记录
/// @param move
/// @param depth
void HistoryHeuristic::add(Move move, int depth)
{
    int pos1 = toIndex(move.x1, move.y1);
    int pos2 = toIndex(move.x2, move.y2);
    historyTable[pos1][pos2] += depth * depth;
}

// 置换表启发
enum nodeType
{
    noneType = 0,
    alphaType = 1,
    betaType = 2,
    exactType = 3,
};

struct tItem
{
    nodeType type = noneType;
    int vl = 0;
    int depth = 0;
    int32 hashLock = 0;
};

class tt
{
public:
    ~tt();
    void init(int hashLevel = 16);
    bool initDone();
    void reset();
    void add(int32 hashKey, int32 hashLock, int vl, nodeType type, int depth);
    void get(int32 hashKey, int32 hashLock, int &vl, int vlApha, int vlBeta, int depth, int nDistance);
    int vlAdjust(int vl, int nDistance);

private:
    tItem *pList = nullptr;
    int hashMask = 0;
    int hashSize = 0;
};

void tt::init(int hashLevel)
{
    if (this->pList != nullptr)
    {
        delete[] pList;
        pList = nullptr;
    }
    this->hashSize = (1 << hashLevel);
    this->hashMask = this->hashSize - 1;
    pList = new tItem[this->hashSize];
}

bool tt::initDone()
{
    return (this->pList != nullptr);
}

void tt::reset()
{
    if (this->pList != nullptr)
    {
        memset(this->pList, 0, sizeof(tItem) * this->hashSize);
    }
}

tt::~tt()
{
    if (pList != nullptr)
    {
        delete[] pList;
        pList = nullptr;
    }
}

int tt::vlAdjust(int vl, int nDistance)
{
    if (std::abs(vl) >= BAN)
    {
        if (vl < 0)
        {
            return vl + nDistance;
        }
        if (vl > 0)
        {
            return vl - nDistance;
        }
    }
    return vl;
}

void tt::add(int32 hashKey, int32 hashLock, int vl, nodeType type, int depth)
{
    const int pos = static_cast<uint32_t>(hashKey) & static_cast<uint32_t>(this->hashMask);
    tItem &t = this->pList[pos];
    if (t.type == noneType)
    {
        t.depth = depth;
        t.vl = vl;
        t.hashLock = hashLock;
        t.type = type;
    }
    else if (depth >= t.depth)
    {
        t.depth = depth;
        t.vl = vl;
        t.hashLock = hashLock;
        t.type = type;
    }
}

void tt::get(int32 hashKey, int32 hashLock, int &vl, int vlApha, int vlBeta, int depth, int nDistance)
{
    const int pos = static_cast<uint32_t>(hashKey) & static_cast<uint32_t>(this->hashMask);
    tItem &t = this->pList[pos];
    if (t.type != noneType && t.hashLock == hashLock && t.depth >= depth)
    {
        if (t.type == exactType)
        {
            vl = this->vlAdjust(t.vl, nDistance);
        }
        else if (t.type == alphaType && t.vl <= vlApha)
        {
            vl = vlApha;
        }
        else if (t.type == betaType && t.vl >= vlBeta)
        {
            vl = vlBeta;
        }
    }
}
