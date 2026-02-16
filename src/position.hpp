#include "hash.hpp"

namespace {
MATRIX board {};
TEAM team { 0 };
}

void init(const MATRIX& matrix, TEAM team)
{
    board = matrix;
    team = team;
}
