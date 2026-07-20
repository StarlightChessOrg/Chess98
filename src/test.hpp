#pragma once
#include "base.hpp"

constexpr MATRIX INIT_MATRIX {
    B_ROOK, B_KNIGHT, B_BISHOP, B_ADVISOR,
    B_KING, B_ADVISOR, B_BISHOP, B_KNIGHT, B_ROOK,
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, B_CANNON, 0, 0, 0, 0, 0, B_CANNON, 0,
    B_PAWN, 0, B_PAWN, 0, B_PAWN, 0, B_PAWN, 0, B_PAWN,
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    R_PAWN, 0, R_PAWN, 0, R_PAWN, 0, R_PAWN, 0, R_PAWN,
    0, R_CANNON, 0, 0, 0, 0, 0, R_CANNON, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    R_ROOK, R_KNIGHT, R_BISHOP, R_ADVISOR,
    R_KING, R_ADVISOR, R_BISHOP, R_KNIGHT, R_ROOK
};

void move_validation();
void movepicker_1sec_count();
void movegen_1sec_count();
