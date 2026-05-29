#pragma once
#include "search.hpp"
#include "utils.hpp"

const MATRIX matrix {
    B_ROOK, B_KNIGHT, B_BISHOP, B_ADVISOR, B_KING, B_ADVISOR, B_BISHOP, B_KNIGHT, B_ROOK,
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, B_CANNON, 0, 0, 0, 0, 0, B_CANNON, 0,
    B_PAWN, 0, B_PAWN, 0, B_PAWN, 0, B_PAWN, 0, B_PAWN,
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    R_PAWN, 0, R_PAWN, 0, R_PAWN, 0, R_PAWN, 0, R_PAWN,
    0, R_CANNON, 0, 0, 0, 0, 0, R_CANNON, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    R_ROOK, R_KNIGHT, R_BISHOP, R_ADVISOR, R_KING, R_ADVISOR, R_BISHOP, R_KNIGHT, R_ROOK
};

void movegen_1sec_count()
{
    position_init(matrix, R);
    history_init();
    killer_init();
    tt_init();
    Timer t { 1000 };
    uint64_t iterations = 0, num = 0;
    while (!t.time_up()) {
        for (auto move : gen_all_capture_moves()) {
            num += move.beg;
        }
        for (auto move : gen_all_quiet_moves()) {
            num += move.end;
        }
        iterations++;
    }
    std::cout << "[Move Generation 1 second] iterations: " << iterations;
    std::cout << " num: " << num;
    std::cout << std::endl;
}
