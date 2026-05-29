#pragma once
#include "heuristic.hpp"
#include "movegen.hpp"

constexpr char STATUS_TT = 0;
constexpr char STATUS_GOOD_CAPTURES = 1;
constexpr char STATUS_KILLER = 2;
constexpr char STATUS_QUIET = 3;
constexpr char STATUS_BAD_CAPTURES = 4;

class MovePicker {
    DEPTH depth { 0 };

public:
    MovePicker(DEPTH depth) : depth(depth) { }
    Move next()
    {
        return Move{};
        // TODO
    }

protected:
};
