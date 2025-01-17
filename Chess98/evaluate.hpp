#pragma once

#include "base.hpp"

using WEIGHT_MAP = const std::array<std::array<int, 10>, 9>;

// 权重表所记录的数值都是红方位置 -> 分数，黑方评分则需要倒置这张表，我的习惯是红在上，横放

WEIGHT_MAP WEIGHT_KING{
    {
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {9900, 9800, 9700, 0, 0, 0, 0, 0, 0, 0},
        {10000, 9800, 9700, 0, 0, 0, 0, 0, 0, 0},
        {9900, 9800, 9700, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    }};

WEIGHT_MAP WEIGHT_GUARD{
    {
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {100, 0, 90, 0, 0, 0, 0, 0, 0, 0},
        {0, 130, 0, 0, 0, 0, 0, 0, 0, 0},
        {100, 0, 90, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    }};

WEIGHT_MAP WEIGHT_BISHOP{
    {
        {0, 0, 90, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {100, 0, 0, 0, 90, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 130, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {100, 0, 0, 0, 90, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 90, 0, 0, 0, 0, 0, 0, 0},
    }};

WEIGHT_MAP WEIGHT_KNIGHT{
    {
        {250, 270, 300, 300, 300, 290, 290, 310, 280, 305},
        {300, 300, 320, 310, 320, 340, 360, 350, 350, 270},
        {290, 300, 310, 310, 315, 315, 315, 320, 370, 300},
        {280, 300, 305, 300, 320, 330, 360, 360, 320, 300},
        {250, 250, 290, 300, 310, 310, 320, 330, 250, 250},
        {280, 300, 305, 300, 320, 330, 360, 360, 320, 270},
        {290, 300, 310, 310, 315, 315, 315, 320, 370, 320},
        {300, 300, 320, 310, 320, 340, 360, 350, 350, 270},
        {250, 270, 300, 300, 300, 290, 290, 310, 280, 305},
    }};

WEIGHT_MAP WEIGHT_ROOK{
    {
        {580, 608, 604, 606, 612, 612, 612, 612, 616, 614},
        {610, 604, 608, 610, 616, 614, 618, 612, 620, 614},
        {606, 608, 606, 608, 614, 612, 616, 612, 618, 612},
        {614, 616, 614, 614, 620, 618, 622, 618, 624, 618},
        {612, 608, 612, 614, 620, 618, 622, 618, 626, 616},
        {614, 616, 614, 614, 620, 618, 622, 618, 624, 618},
        {606, 608, 606, 608, 614, 612, 616, 612, 618, 612},
        {610, 604, 608, 610, 616, 614, 618, 612, 620, 614},
        {580, 608, 604, 606, 612, 612, 612, 612, 616, 614},
    }};

WEIGHT_MAP WEIGHT_CANNON{
    {
        {300, 300, 304, 300, 298, 300, 300, 302, 302, 306},
        {300, 302, 300, 300, 300, 300, 300, 302, 302, 304},
        {302, 304, 308, 300, 302, 300, 298, 300, 300, 300},
        {306, 306, 310, 302, 304, 302, 304, 290, 296, 290},
        {306, 306, 320, 304, 306, 308, 310, 292, 286, 288},
        {306, 306, 310, 302, 304, 302, 304, 290, 296, 290},
        {302, 304, 308, 300, 302, 300, 298, 300, 300, 300},
        {300, 302, 300, 300, 300, 300, 300, 302, 302, 304},
        {300, 300, 304, 300, 298, 300, 300, 302, 302, 306},
    }};

WEIGHT_MAP WEIGHT_PAWN{
    {
        {30, 30, 30, 30, 32, 36, 40, 44, 48, 30},
        {30, 30, 30, 30, 30, 42, 50, 56, 66, 33},
        {30, 30, 30, 28, 38, 48, 60, 72, 86, 36},
        {30, 30, 30, 30, 30, 48, 64, 90, 110, 39},
        {30, 30, 30, 34, 38, 50, 70, 110, 150, 42},
        {30, 30, 30, 30, 30, 48, 64, 90, 110, 39},
        {30, 30, 30, 28, 38, 48, 60, 72, 86, 36},
        {30, 30, 30, 30, 30, 42, 50, 56, 66, 33},
        {30, 30, 30, 30, 32, 36, 40, 44, 48, 30},
    }};

std::map<PIECEID, WEIGHT_MAP> pieceWeights{
    {R_KING, WEIGHT_KING},
    {R_GUARD, WEIGHT_GUARD},
    {R_BISHOP, WEIGHT_BISHOP},
    {R_KNIGHT, WEIGHT_KNIGHT},
    {R_ROOK, WEIGHT_ROOK},
    {R_CANNON, WEIGHT_CANNON},
    {R_PAWN, WEIGHT_PAWN},
    {B_KING, WEIGHT_KING},
    {B_GUARD, WEIGHT_GUARD},
    {B_BISHOP, WEIGHT_BISHOP},
    {B_KNIGHT, WEIGHT_KNIGHT},
    {B_ROOK, WEIGHT_ROOK},
    {B_CANNON, WEIGHT_CANNON},
    {B_PAWN, WEIGHT_PAWN}};
