#pragma once
#include "position.hpp"

// ============================================================================
// hand-crafted evaluation (policy layer)
//
//   eval = incremental PST (eval_vl_, red minus black)
//        + mobility (rook/cannon/knight, bitline based)
//        + open-file rooks
//        - hanging-piece threats (attacked and not defended)
//        + shape bonuses (guard integrity, hollow cannon, palace-heart
//          knight, river-crossing momentum, ...)
//        + tempo
//
// the pst tables live in base.hpp; eval_vl_ lives in position.hpp and is
// maintained in O(1) by position_init / position_move / position_undo.
// dynamic terms are computed from the piece lists. scores are red - black
// until the final side-to-move flip.
// ============================================================================

namespace eval_detail_ {

constexpr VL TEMPO = 8;

// hanging-piece penalty by piece type (attacked and undefended)
constexpr std::array<int, 8> HANG_PEN_ { 0, 0, 25, 25, 50, 110, 55, 8 };

// mobility weight per reachable square
constexpr int MOB_ROOK = 2;
constexpr int MOB_CANNON = 2;
constexpr int MOB_KNIGHT = 4;

struct SideInfo {
    int advisors { 0 };
    int bishops { 0 };
    int knights { 0 };
    int rooks { 0 };
    int cannons { 0 };
    int pawns { 0 };
    int over_river { 0 };
    int mobility { 0 };
    int open_file { 0 };
    int threat { 0 };
    UINT32 pawn_files { 0 };
    POS king { INVALID_POS };
    std::array<POS, 2> cannon_sq { INVALID_POS, INVALID_POS };
};

bool crossed_river_(TEAM team, POS pos)
{
    return team == R ? pos / 9 < 5 : pos / 9 > 4;
}

// is square `sq` attacked by team `by`? (kings' flying-general excluded)
bool attacks_sq_(TEAM by, POS sq)
{
    const int f = sq % 9;
    // pawn: forward, plus sideways once across the river
    if (piece_on(POS(sq + 9 * by)) * by == R_PAWN) return true;
    if ((sq / 9 < 5 && by == R) || (sq / 9 > 4 && by == B)) {
        if (f != 0 && piece_on(POS(sq - 1)) * by == R_PAWN) return true;
        if (f != 8 && piece_on(POS(sq + 1)) * by == R_PAWN) return true;
    }
    // advisor (diagonal one step, palace-confined by construction)
    if (f != 0) {
        if (piece_on(POS(sq - 10)) * by == R_ADVISOR) return true;
        if (piece_on(POS(sq + 8)) * by == R_ADVISOR) return true;
    }
    if (f != 8) {
        if (piece_on(POS(sq - 8)) * by == R_ADVISOR) return true;
        if (piece_on(POS(sq + 10)) * by == R_ADVISOR) return true;
    }
    // bishop (diagonal two steps with eye check, own-half-confined)
    if (f >= 2) {
        if (!piece_on(POS(sq - 10)) && piece_on(POS(sq - 20)) * by == R_BISHOP)
            return true;
        if (!piece_on(POS(sq + 8)) && piece_on(POS(sq + 16)) * by == R_BISHOP)
            return true;
    }
    if (f <= 6) {
        if (!piece_on(POS(sq - 8)) && piece_on(POS(sq - 16)) * by == R_BISHOP)
            return true;
        if (!piece_on(POS(sq + 10)) && piece_on(POS(sq + 20)) * by == R_BISHOP)
            return true;
    }
    // knight (leg checks grouped like in_check, with file guards)
    if (!piece_on(POS(sq - 10))) {
        if (f >= 1 && piece_on(POS(sq - 19)) * by == R_KNIGHT) return true;
        if (f <= 6 && piece_on(POS(sq - 11)) * by == R_KNIGHT) return true;
    }
    if (!piece_on(POS(sq + 10))) {
        if (f <= 7 && piece_on(POS(sq + 19)) * by == R_KNIGHT) return true;
        if (f >= 2 && piece_on(POS(sq + 11)) * by == R_KNIGHT) return true;
    }
    if (!piece_on(POS(sq + 8))) {
        if (f >= 1 && piece_on(POS(sq + 17)) * by == R_KNIGHT) return true;
        if (f <= 6 && piece_on(POS(sq + 7)) * by == R_KNIGHT) return true;
    }
    if (!piece_on(POS(sq - 8))) {
        if (f <= 7 && piece_on(POS(sq - 17)) * by == R_KNIGHT) return true;
        if (f >= 2 && piece_on(POS(sq - 7)) * by == R_KNIGHT) return true;
    }
    // rook and cannon along the bitlines (capture relation is symmetric)
    const auto [rl, rr] = rook_9(get_bl9(sq), sq);
    const auto [rt, rb] = rook_10(get_bl10(sq), sq);
    if (piece_on(rl) * by == R_ROOK || piece_on(rr) * by == R_ROOK
        || piece_on(rt) * by == R_ROOK || piece_on(rb) * by == R_ROOK)
        return true;
    const auto [cl, cr] = cannon_9(get_bl9(sq), sq);
    const auto [ct, cb] = cannon_10(get_bl10(sq), sq);
    if (piece_on(cl) * by == R_CANNON || piece_on(cr) * by == R_CANNON
        || piece_on(ct) * by == R_CANNON || piece_on(cb) * by == R_CANNON)
        return true;
    // king (adjacent, palace-confined)
    if (f != 0 && piece_on(POS(sq - 1)) * by == R_KING) return true;
    if (f != 8 && piece_on(POS(sq + 1)) * by == R_KING) return true;
    if (piece_on(POS(sq - 9)) * by == R_KING) return true;
    if (piece_on(POS(sq + 9)) * by == R_KING) return true;
    return false;
}

// reachable squares of a rook (empty targets plus enemy banners)
int rook_mobility_(TEAM team, POS pos)
{
    const auto [l, r] = rook_9(get_bl9(pos), pos);
    const auto [t, b] = rook_10(get_bl10(pos), pos);
    int mob = int(r) - int(l) - 2 + (int(b) - int(t)) / 9 - 2;
    if (piece_on(l) * team <= 0) ++mob;
    if (piece_on(r) * team <= 0) ++mob;
    if (piece_on(t) * team <= 0) ++mob;
    if (piece_on(b) * team <= 0) ++mob;
    return mob;
}

// reachable squares of a cannon (rook-like quiet moves plus capture banners)
int cannon_mobility_(TEAM team, POS pos)
{
    const auto [l, r] = rook_9(get_bl9(pos), pos);
    const auto [t, b] = rook_10(get_bl10(pos), pos);
    int mob = int(r) - int(l) - 2 + (int(b) - int(t)) / 9 - 2;
    if (piece_on(l) * team <= 0) ++mob;
    if (piece_on(r) * team <= 0) ++mob;
    if (piece_on(t) * team <= 0) ++mob;
    if (piece_on(b) * team <= 0) ++mob;
    const auto [cl, cr] = cannon_9(get_bl9(pos), pos);
    const auto [ct, cb] = cannon_10(get_bl10(pos), pos);
    if (cl < 90 && piece_on(cl) * team < 0) ++mob;
    if (cr < 90 && piece_on(cr) * team < 0) ++mob;
    if (ct < 90 && piece_on(ct) * team < 0) ++mob;
    if (cb < 90 && piece_on(cb) * team < 0) ++mob;
    return mob;
}

// reachable squares of a knight (leg and file guards)
int knight_mobility_(TEAM team, POS pos_)
{
    const int p = pos_;
    const int f = p % 9;
    int mob = 0;
    const auto ok = [&](int dst) {
        return dst >= 0 && dst < 90 && piece_on(POS(dst)) * team <= 0;
    };
    if (p >= 9 && !piece_on(POS(p - 9))) {
        if (f >= 1 && ok(p - 19)) ++mob;
        if (f <= 7 && ok(p - 17)) ++mob;
    }
    if (p <= 80 && !piece_on(POS(p + 9))) {
        if (f <= 7 && ok(p + 19)) ++mob;
        if (f >= 1 && ok(p + 17)) ++mob;
    }
    if (f >= 2 && !piece_on(POS(p - 1))) {
        if (ok(p - 11)) ++mob;
        if (ok(p + 7)) ++mob;
    }
    if (f <= 6 && !piece_on(POS(p + 1))) {
        if (ok(p + 11)) ++mob;
        if (ok(p - 7)) ++mob;
    }
    return mob;
}

// same-file cannon vs bare king lane (空头炮)
bool hollow_cannon_(POS cannon, POS king)
{
    if (cannon % 9 != king % 9) return false;
    const POS lo = std::min(cannon, king);
    const POS hi = std::max(cannon, king);
    for (POS p = POS(lo + 9); p < hi; p = POS(p + 9)) {
        if (g_board[p]) return false;
    }
    return true;
}

// pass 1: piece counts, king, pawn files, river-crossing momentum
SideInfo collect_side_(TEAM team)
{
    SideInfo s { };
    const auto& list = team == R ? pos_list_r_ : pos_list_b_;
    for (const POS pos : list) {
        const bool over = crossed_river_(team, pos);
        switch (std::abs(g_board[pos])) {
        case R_KING:
            s.king = pos;
            break;
        case R_ADVISOR:
            ++s.advisors;
            break;
        case R_BISHOP:
            ++s.bishops;
            break;
        case R_KNIGHT:
            ++s.knights;
            if (over) s.over_river += 2;
            break;
        case R_ROOK:
            ++s.rooks;
            if (over) s.over_river += 3;
            break;
        case R_CANNON:
            s.cannon_sq[size_t(std::min(s.cannons, 1))] = pos;
            ++s.cannons;
            if (over) s.over_river += 2;
            break;
        case R_PAWN:
            ++s.pawns;
            s.pawn_files |= UINT32(1) << (pos % 9);
            if (over) s.over_river += 1;
            break;
        default:
            break;
        }
    }
    return s;
}

// pass 2: mobility, open files, hanging-piece threats
void collect_positional_(TEAM team, SideInfo& me, const SideInfo& opp)
{
    const auto& list = team == R ? pos_list_r_ : pos_list_b_;
    for (const POS pos : list) {
        const int id = std::abs(g_board[pos]);
        switch (id) {
        case R_KNIGHT:
            me.mobility += MOB_KNIGHT * knight_mobility_(team, pos);
            break;
        case R_ROOK:
            me.mobility += MOB_ROOK * rook_mobility_(team, pos);
            if (!(me.pawn_files & (UINT32(1) << (pos % 9)))) {
                me.open_file += 10;
                if (!(opp.pawn_files & (UINT32(1) << (pos % 9))))
                    me.open_file += 8;
            }
            break;
        case R_CANNON:
            me.mobility += MOB_CANNON * cannon_mobility_(team, pos);
            break;
        default:
            break;
        }
        if (id != R_KING && attacks_sq_(TEAM(-team), pos)
            && !attacks_sq_(team, pos))
            me.threat += HANG_PEN_[size_t(id)];
    }
}

// shape bonuses of `me` against `opp`
int shape_bonus_(TEAM team, const SideInfo& me, const SideInfo& opp)
{
    int vl = 0;

    // 士象全：完整防守体系
    if (me.advisors == 2 && me.bishops == 2) vl += 15;

    // 缺士怕双车，缺象怕炮
    if (opp.advisors == 0 && me.rooks >= 2) vl += 30;
    else if (opp.advisors <= 1 && me.rooks >= 1) vl += 10;
    if (opp.bishops == 0 && me.cannons >= 1) vl += 20;
    else if (opp.bishops <= 1 && me.cannons >= 1) vl += 8;

    // 窝心马：滞碍双士，严重扣分
    const POS heart = team == R ? POS(76) : POS(13);
    if (g_board[heart] * team == R_KNIGHT) vl -= 50;

    // 空头炮：子力充足时价值高
    for (const POS c : me.cannon_sq) {
        if (c < 90 && opp.king < 90 && hollow_cannon_(c, opp.king)) {
            const int majors = me.rooks + me.knights + me.cannons;
            vl += 20 + majors * 8;
            break;
        }
    }

    // 过河势：鼓励多子过河取势
    if (me.over_river > opp.over_river)
        vl += std::min(24, 5 * (me.over_river - opp.over_river));

    return vl;
}

} // namespace eval_detail_

VL evaluate()
{
    using namespace eval_detail_;
    int vl = g_evaluation;
    // dynamic terms
    SideInfo red = collect_side_(R);
    SideInfo black = collect_side_(B);
    collect_positional_(R, red, black);
    collect_positional_(B, black, red);
    vl += red.mobility - black.mobility;
    vl += red.open_file - black.open_file;
    vl -= red.threat - black.threat;
    vl += shape_bonus_(R, red, black) - shape_bonus_(B, black, red);
    vl += g_team == R ? TEMPO : -TEMPO;
    return VL(g_team * vl);
}
