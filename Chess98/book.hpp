#pragma once
#include "hash.hpp"

const int MAX_GEN_MOVES = 128;

struct BookStruct
{
    union
    {
        uint32_t dwZobristLock;
        int nPtr;
    };
    uint16_t wmv, wvl;
}; // bk

inline int BOOK_POS_CMP(const BookStruct &bk, const int32 hashLock)
{
    uint32_t bookLock = bk.dwZobristLock;
    uint32_t boardLock = (uint32_t)hashLock;
    if (bookLock < boardLock)
        return -1;
    else if (bookLock > boardLock)
        return 1;
    return 0;
}

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
