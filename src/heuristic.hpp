#pragma once
#include "base.hpp"
#include "position.hpp"

/// @brief 红方的历史表
std::array<std::array<UINT32, 90>, 90> history_table_r_ { };
/// @brief 黑方的历史表
std::array<std::array<UINT32, 90>, 90> history_table_b_ { };
/// @brief 杀手表
std::array<std::array<Move, 2>, 128> killer_table_ { };
/// @brief 置换表
std::vector<TTEntry> tt_table_ { };
/// @brief 置换表大小
std::uint32_t tt_size_ { 0 };

/// @brief 初始化历史表
void history_init()
{
    history_table_r_.fill({ });
    history_table_b_.fill({ });
}

/// @brief 在历史表中增加分值
/// @param move 历史表着法
/// @param team 要设置的队伍
/// @param depth 传入当前搜索的深度
void history_set(Move move, TEAM team, DEPTH depth)
{
    if (team == R) {
        history_table_r_[move.beg][move.end] += depth * depth;
    } else {
        history_table_b_[move.beg][move.end] += depth * depth;
    }
}

/// @brief 历史启发式排序
/// @tparam It 迭代器类型，需满足随机访问迭代器要求
/// @param first 待排序走法序列的begin迭代器
/// @param last  待排序走法序列的end迭代器
/// @param team  当前行棋方
template <class It>
void history_sort(It first, It last, TEAM team)
{
    const auto& t = team == R ? history_table_r_ : history_table_b_;
    std::sort(first, last, [&](Move a, Move b) {
        const UINT32 ha = t[a.beg][a.end];
        const UINT32 hb = t[b.beg][b.end];
        if (ha != hb) return ha > hb;
        if (a.beg != b.beg) return a.beg < b.beg;
        return a.end < b.end;
    });
}

/// @brief 初始化杀手表
void killer_init()
{
    killer_table_.fill({ });
}

/// @brief 记录当前深度的杀手着法
/// @param move 杀手着法
/// @param depth 当前深度
void killer_set(Move move, DEPTH depth)
{
    if (move == killer_table_[depth][0]) return;
    killer_table_[depth][1] = killer_table_[depth][0];
    killer_table_[depth][0] = move;
}

/// @brief 获取当前深度的杀手着法，可能含有空着法
/// @param depth 当前深度
/// @return 杀手着法列表，若不足则补空着法
std::array<Move, 2> killer_get(DEPTH depth)
{
    return killer_table_[depth];
}

/// @brief 初始化置换表
/// @param size 置换表大小
void tt_init(int size = 20)
{
    tt_table_.clear();
    tt_table_.resize(1ll << size);
    tt_size_ = size;
}

/// @brief 获取一个根据局面哈希置换表项目
/// @param hashkey 局面哈希
/// @return 置换表项目
TTEntry& tt_get_entry_(HASH hashkey) {
    return tt_table_[hashkey & ((1ll << tt_size_) - 1)];
}
    /// @brief 设置置换表项目
/// @param hashkey 当前局面的哈希值
/// @param flag 要设置的项目的标签是alpha, beta还是exact
/// @param depth 当前深度
/// @param move 要设置的着法
/// @param vl 要设置的分数
void tt_set(HASH hashkey, HASH_FLAG flag, DEPTH depth, Move move, VL vl)
{
    assert(move && vl != INVALID_VL && !tt_table_.empty());
    assert(flag == EXACT || flag == ALPHA || flag == BETA);
    TTEntry& e = tt_get_entry_(hashkey);
    if (e.key == 0) { // empty set
        e.key = hashkey;
        e.flag = flag;
        e.depth = depth;
        e.move = move;
        e.vl = vl;
    } else if (e.key == hashkey) { // same position replace
        if (depth >= e.depth) { // only deeper
            e.flag = flag;
            e.depth = depth;
            e.move = move;
            e.vl = vl;
        }
    } else { // collision
        e.key = hashkey;
        e.flag = flag;
        e.depth = depth;
        e.move = move;
        e.vl = vl;
    }
}

/// @brief 获取置换表分数，没有则返回INVALID_VL
/// @param hashkey 局面哈希
/// @param depth 局面深度
/// @param alpha 局面alpha
/// @param beta 局面beta
/// @return 置换表分数或INVALID_VL
VL tt_get_vl(HASH hashkey, DEPTH depth, VL alpha, VL beta)
{
    const TTEntry& e = tt_get_entry_(hashkey);
    if (e.key != hashkey || e.depth < depth) {
        return INVALID_VL;
    } else if (e.flag == EXACT) {
        return e.vl;
    } else if (e.flag == ALPHA && e.vl <= alpha) {
        return e.vl;
    } else if (e.flag == BETA && e.vl >= beta) {
        return e.vl;
    }
    return INVALID_VL;
}

/// @brief 获取置换表着法，没有则为空着法
/// @return 置换表着法或空着法
Move tt_get_move()
{
    const TTEntry& entry = tt_get_entry_(g_hashkey);
    return entry.key == g_hashkey ? entry.move : Move{ };
}

/// @brief 判断一个着法的吃子评估划算值是否大于某个阈值
/// @param move 着法
/// @param threshold 阈值
/// @return 是否大于这个阈值
bool see_ge(Move move, VL threshold)
{
    const VL victim = WEIGHTS[std::size_t(std::abs(piece_on(move.end)))];
    const VL attacker = WEIGHTS[std::size_t(std::abs(piece_on(move.beg)))];
    if (victim >= attacker) return true;
    threshold = 0; // placeholder
    position_move(move);
    const bool protected_ = get_protector(move.end) < 90;
    position_undo();
    return !protected_;
}

/// @brief 根据最小子吃最大子的原则对一个吃子着法列表进行排序
/// @tparam It 可迭代对象
/// @param first 
/// @param last 
template <class It>
void mvvlva_sort(It first, It last)
{
    std::sort(first, last, [](Move a, Move b) {
        const VL a_beg = WEIGHTS[std::size_t(std::abs(piece_on(a.beg)))];
        const VL a_end = WEIGHTS[std::size_t(std::abs(piece_on(a.end)))];
        const VL b_beg = WEIGHTS[std::size_t(std::abs(piece_on(b.beg)))];
        const VL b_end = WEIGHTS[std::size_t(std::abs(piece_on(b.end)))];
        const VL sa = VL(a_beg - a_end);
        const VL sb = VL(b_beg - b_end);
        if (sa != sb) return sa < sb;
        if (a.beg != b.beg) return a.beg < b.beg;
        return a.end < b.end;
    });
}
