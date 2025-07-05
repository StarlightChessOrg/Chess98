#pragma once
#include "base.hpp"

using BITLINE = unsigned;
using BITARRAY_X = std::array<BITLINE, 9>;
using BITARRAY_Y = std::array<BITLINE, 10>;
using REGION_ROOK = std::array<int, 2>;
using REGION_CANNON = std::array<int, 4>;
using TYPE_ROOK_CACHE = std::array<std::array<REGION_ROOK, 10>, 1024>;
using TYPE_CANNON_CACHE = std::array<std::array<REGION_CANNON, 10>, 1024>;

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
        for (BITLINE bitline = 1; bitline <= pow(2, 10); bitline++)
        {
            for (int index = 0; index <= 9; index++)
            {
                if (this->getBit(bitline, index) == 1)
                {
                    this->rookCache[bitline][index] = this->generateRookRegion(bitline, index);
                    this->cannonCache[bitline][index] = this->generateCannonRegion(bitline, index);
                }
            }
        }
    }

    REGION_ROOK getRookRegion(BITLINE bitline, int index, int endpos)
    {
        REGION_ROOK result = this->rookCache[bitline][index];
        if (endpos == 8 && result[1] == 9)
        {
            result[1] = 8;
        }
        return result;
    }

    REGION_CANNON getCannonRegion(BITLINE bitline, int index, int endpos)
    {
        REGION_CANNON result = this->cannonCache[bitline][index];
        if (endpos == 8 && result[3] == 9)
        {
            result[2] = result[3] = 8;
        }
        return result;
    }

    TYPE_ROOK_CACHE rookCache{};
    TYPE_CANNON_CACHE cannonCache{};

    BITARRAY_X xBitBoard{0, 0, 0, 0, 0, 0, 0, 0, 0};
    BITARRAY_Y yBitBoard{0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    friend class Board;

    int getBit(BITLINE bitline, int index) const
    {
        return (bitline >> index) & 1;
    }
    void doMove(int x1, int y1, int x2, int y2)
    {
        bool ret = bool(this->getBit(x2, y2));
        this->deleteBit(x1, y1);
        this->setBit(x2, y2);
    }

    void undoMove(int x1, int y1, int x2, int y2, bool eaten)
    {
        this->setBit(x1, y1);
        if (!eaten)
        {
            this->deleteBit(x2, y2);
        }
    }

    void setBit(int x, int y)
    {
        this->xBitBoard[x] |= (1 << y);
        this->yBitBoard[y] |= (1 << x);
    }

    void deleteBit(int x, int y)
    {
        this->xBitBoard[x] &= ~(1 << y);
        this->yBitBoard[y] &= ~(1 << x);
    }

    REGION_ROOK generateRookRegion(BITLINE bitline, int index) const
    {
        int beg = 0;
        int end = 9;
        for (int pos = index - 1; pos >= 0; pos--)
        {
            if (this->getBit(bitline, pos) != 0)
            {
                beg = pos;
                break;
            }
        }
        for (int pos = index + 1; pos <= 9; pos++)
        {
            if (this->getBit(bitline, pos) != 0)
            {
                end = pos;
                break;
            }
        }

        return REGION_ROOK{beg, end};
    }

    REGION_CANNON generateCannonRegion(BITLINE bitline, int index) const
    {
        int eaten1 = 0;
        int beg = 0;
        int end = 9;
        int eaten2 = 9;
        for (int pos = index - 1; pos >= 0; pos--)
        {
            if (this->getBit(bitline, pos) != 0)
            {
                beg = pos + 1;
                eaten1 = pos + 1;
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
        for (int pos = index + 1; pos <= 9; pos++)
        {
            if (this->getBit(bitline, pos) != 0)
            {
                end = pos - 1;
                eaten2 = pos - 1;
                for (int pos2 = pos + 1; pos2 <= 9; pos2++)
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
