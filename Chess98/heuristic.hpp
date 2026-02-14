#include "base.hpp"

namespace history {

namespace {

    std::array<std::array<int, 256>, 256> table_r {};
    std::array<std::array<int, 256>, 256> table_b {};

    int strategy(int depth)
    {
        return depth * depth;
    }

    int score_of_r(Move move)
    {
        return table_r[move.beg][move.end];
    }

    int score_of_b(Move move)
    {
        return table_b[move.beg][move.end];
    }

} // namespace

void init()
{
    table_r = {};
    table_b = {};
}

void update(Move move, TEAM team, int depth)
{
    assert(team == R || team == B);
    assert(valid_pos(move.beg) && valid_pos(move.end));
    if (team == R) {
        table_r[move.beg][move.end] += strategy(depth);
    } else {
        table_b[move.beg][move.end] += strategy(depth);
    }
}

void sort(PID_MATRIX pid_matrix, MOVES& moves)
{
    const size_t n = moves.size();
    if (n <= 1)
        return;

    const bool is_r = pid_matrix[moves[0].beg] > 0;
    std::sort(moves.begin(), moves.end(), [&](const Move& a, const Move& b) {
        const int& va = is_r ? score_of_r(a) : score_of_b(a);
        const int& vb = is_r ? score_of_r(b) : score_of_b(b);
        return va > vb; // 降序
    });
}

} // namsapce history

namespace killer {

namespace {

    std::array<std::array<Move, 2>, 128> table {};

} // namespace

void init()
{
    table = {};
}

void update(int distance, Move move)
{
    table[distance][1] = table[distance][0];
    table[distance][0] = move;
}

MOVES moves()
{
    // TODO
    return MOVES {};
}

} // namespace killer

namespace tt {

namespace {

    struct Item {
        int lock { 0 };
        int vl { 0 };
        char depth { 0 };
        Move move {};
    };

    std::vector<Item> table {};
    int size { 0 };

} // namespace

void init(int hash_size = 12)
{
    size = hash_size;
    table.clear();
    table.resize(size);
}

void update(int key, int lock, Move& good_move, NODE_TYPE type, int depth)
{
    // TODO
}

int get_vl(int lock, int alpha, int beta, int depth)
{
    // TODO
    return 0;
}

Move get_move(int key, int lock)
{
    // TODO
    return Move{};
}

} // namespace tt
