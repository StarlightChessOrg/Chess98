#pragma once
#include "search.hpp"
#include "utils.hpp"

constexpr MATRIX matrix {
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
    uint64_t iterations = 0, id = 0, num = 0;
    while (!t.time_up()) {
        MovePicker a { 1 };
        for (Move m = a.next(); m; m = a.next()) {
            id += m.beg;
            id += m.end;
            num++;
        }
        iterations++;
    }
    std::cout << "[Move Generation 1 second] iterations: " << iterations;
    std::cout << " num: " << num;
    std::cout << " id: " << id;
    std::cout << std::endl;
}
