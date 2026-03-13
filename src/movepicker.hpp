#pragma once
#include "movegen.hpp"
#include "heuristic.hpp"

constexpr char STATE_TT = 0;
constexpr char STATE_KILLER = 1;

class MovePicker {
    std::vector<Move> moves {};
    char status { STATE_TT };
};
