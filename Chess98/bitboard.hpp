#pragma once
#include "base.hpp"

using BITLINE = unsigned;
using BITARRAY_X = std::array<BITLINE, 9>;
using BITARRAY_Y = std::array<BITLINE, 10>;
using REGION_ROOK = std::array<int, 2>;   // 车的着法起始点终结点
using REGION_CANNON = std::array<int, 4>; // 炮的着法起始点终结点
using TYPE_ROOK_CACHE = std::map<int, std::map<BITLINE, REGION_ROOK>>;
using TYPE_CANNON_CACHE = std::map<int, std::map<BITLINE, REGION_CANNON>>;

// 炮的四个值分别对应eaten1, start, end, eaten2，若没有eaten则eaten = 端点

/// @brief 位棋盘(车、炮着法生成)
class BitBoard
{
public:
    BitBoard(PIECEID_MAP pieceidMap)
    {
        // 初始化棋盘
        for (int x = 0; x < 9; x++)
        {
            for (int y = 0; y < 10; y++)
            {
                if (pieceidMap[x][y] != EMPTY_PIECEID)
                    this->setBit(x, y);
            }
        }
        // 初始化车、炮的着法缓存
        for (BITLINE i = 1; i <= pow(2, 10); i++)
        {
            // num: 第几个1是车或炮的所在位置
            for (int num = 0; num <= floor(log2(i)); num++)
            {
                int index = -1;
                for (int j = 0, count = 0; j <= 9; j++)
                {
                    if (this->getBit(i, j) == 1)
                    {
                        if (count != num)
                            count++;
                        else
                            index = count;
                    }
                }
                this->rookCache[num][i] = this->generateRookRegion(i, index, i < pow(2, 9) ? 8 : 9);
                this->cannonCache[num][i] = this->generateCannonRegion(i, index, i < pow(2, 9) ? 8 : 9);
            }
        }
    }

    /// @brief 步进
    /// @param x1
    /// @param y1
    /// @param x2
    /// @param y2
    /// @return 是否有吃子
    void doMove(int x1, int y1, int x2, int y2)
    {
        bool ret = bool(this->getBit(x2, y2));
        this->deleteBit(x1, y1);
        this->setBit(x2, y2);
    }

    /// @brief 撤销步进
    /// @param x1
    /// @param y1
    /// @param x2
    /// @param y2
    /// @param eaten
    void undoMove(int x1, int y1, int x2, int y2, bool eaten)
    {
        this->setBit(x1, y1);
        if (!eaten) {
            this->deleteBit(x2, y2);
        }
    }

    TYPE_ROOK_CACHE rookCache{};
    TYPE_CANNON_CACHE cannonCache{};

    BITARRAY_X xBitBoard{0, 0, 0, 0, 0, 0, 0, 0, 0};
    BITARRAY_Y yBitBoard{0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

private:
    /// @brief 获取bitline二进制的第n位
    /// @param bitline
    /// @param index
    /// @return
    int getBit(BITLINE bitline, int index)
    {
        return (bitline >> index) & 1;
    }

    /// @brief 设置棋盘上x, y的数为1
    /// @param x
    /// @param y
    void setBit(int x, int y)
    {
        this->xBitBoard[x] |= (1 << y);
        this->yBitBoard[y] |= (1 << x);
    }

    /// @brief 设置棋盘上x, y的数为0
    /// @param x
    /// @param y
    void deleteBit(int x, int y)
    {
        this->xBitBoard[x] &= ~(1 << y);
        this->yBitBoard[y] &= ~(1 << x);
    }

    /// @brief 获取车的着法范围
    /// @param bitline
    /// @param index 车的所在位置
    /// @param end 截止点，接受8或9
    /// @return
    REGION_ROOK generateRookRegion(BITLINE bitline, int index, int last)
    {
        int beg = 0;
        int end = last;
        for (int pos = index - 1; pos >= 0; pos--)
        {
            if (this->getBit(bitline, pos) != 0)
            {
                beg = pos;
                break;
            }
        }
        for (int pos = index + 1; pos <= last; pos++)
        {
            if (this->getBit(bitline, pos) != 0)
            {
                end = pos;
                break;
            }
        }

        return REGION_ROOK{beg, end};
    }

    /// @brief 获取炮的着法范围
    /// @param bitline
    /// @param index 炮的所在位置
    /// @param end 截止点，接受9或10
    /// @return
    REGION_CANNON generateCannonRegion(BITLINE bitline, int index, int last)
    {
        int eaten1 = 0;
        int beg = 0;
        int end = last;
        int eaten2 = last;
        for (int pos = index - 1; pos >= 0; pos--)
        {
            if (this->getBit(bitline, pos) != 0)
            {
                beg = pos;
                eaten1 = pos;
                for (int pos2 = pos - 1; pos2 >= 0; pos2--)
                {
                    if (this->getBit(bitline, pos2) != 0)
                    {
                        eaten1 = pos2;
                        break;
                    }
                }
                break;
            }
        }
        for (int pos = index + 1; pos <= last; pos++)
        {
            if (this->getBit(bitline, pos) != 0)
            {
                end = pos;
                eaten2 = pos;
                for (int pos2 = pos + 1; pos2 <= last; pos2++)
                {
                    if (this->getBit(bitline, pos2) != 0)
                    {
                        eaten2 = pos2;
                        break;
                    }
                }
                break;
            }
        }

        return REGION_CANNON{eaten1, beg, end, eaten2};
    }
};
