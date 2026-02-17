#pragma once
#include "heuristic.hpp"
#include "position.hpp"

namespace {

// TODO: implement preloading function

}

MOVES king(POS pos)
{
    assert(pid_on(pos) == R_KING || pid_on(pos) == B_KING);
    MOVES ret {};
    ret.reserve(4);
    const PID pid = pid_on(pos);
    const TEAM team = pid > 0 ? R : B;

    if (pos % 9 == 5) {
        if (!same_team(team, pos + 1)) {
            ret.emplace_back(Move { pos, pos + 1 });
        }
        if (!same_team(team, pos - 1)) {
            ret.emplace_back(Move { pos, pos - 1 });
        }
    } else {
        if (!same_team(team, pos + 1) && pos % 9 == 4) {
            ret.emplace_back(Move { pos, pos + 1 });
        }
        if (!same_team(team, pos - 1)) {
            ret.emplace_back(Move { pos, pos - 1 });
        }
    }
    if (pos / 9 != 2 && pos / 9 != 9 && !same_team(team, pos + 9)) {
        ret.emplace_back(Move { pos, pos + 9 });
    }
    if (pos / 9 != 0 && pos / 9 != 7 && !same_team(team, pos - 9)) {
        ret.emplace_back(Move { pos, pos - 9 });
    }
    return ret;
}

MOVES advisor(POS pos)
{
    assert(pid_on(pos) == R_ADVISOR || pid_on(pos) == B_ADVISOR);
    MOVES ret {};
    ret.reserve(4);
    const PID pid = pid_on(pos);
    const TEAM team = pid > 0 ? R : B;

    if (pos == 76 || pos == 13) {
        if (!same_team(team, pos - 10)) {
            ret.emplace_back(Move { pos, pos - 10 });
        }
        if (!same_team(team, pos - 8)) {
            ret.emplace_back(Move { pos, pos - 8 });
        }
        if (!same_team(team, pos + 10)) {
            ret.emplace_back(Move { pos, pos + 10 });
        }
        if (!same_team(team, pos + 8)) {
            ret.emplace_back(Move { pos, pos + 8 });
        }
    } else if (team == R && !same_team(team, 76)) {
        ret.emplace_back(Move { pos, 76 });
    } else if (team == B && !same_team(team, 13)) {
        ret.emplace_back(Move { pos, 13 });
    }
    return ret;
}

MOVES bishop(POS pos)
{
    MOVES ret {};
    ret.reserve(4);
    const PID pid = pid_on(pos);
    const TEAM team = pid > 0 ? R : B;

    if (pos / 9 == 9 || pos / 9 == 4) {
        if (!pid_on(pos - 10) && !same_team(team, pos - 20)) {
            ret.emplace_back(Move { pos, pos - 20 });
        }
        if (!pid_on(pos - 8) && !same_team(team, pos - 16)) {
            ret.emplace_back(Move { pos, pos - 16 });
        }
    } else if (pos / 9 == 7 || pos / 9 == 2) {
        if (!pid_on(pos + 10) && !same_team(team, pos + 20)) {
            ret.emplace_back(Move { pos, pos + 20 });
        }
        if (!pid_on(pos + 8) && !same_team(team, pos + 16)) {
            ret.emplace_back(Move { pos, pos + 16 });
        }
        if (!pid_on(pos - 10) && !same_team(team, pos - 20)) {
            ret.emplace_back(Move { pos, pos - 20 });
        }
        if (!pid_on(pos - 8) && !same_team(team, pos - 16)) {
            ret.emplace_back(Move { pos, pos - 16 });
        }
    } else {
        if (!pid_on(pos + 10) && !same_team(team, pos + 20)) {
            ret.emplace_back(Move { pos, pos + 20 });
        }
        if (!pid_on(pos + 8) && !same_team(team, pos + 16)) {
            ret.emplace_back(Move { pos, pos + 16 });
        }
    }
    return ret;
}

MOVES knight(POS pos)
{
    MOVES ret {};
    ret.reserve(8);
    const PID pid = pid_on(pos);
    const TEAM team = pid > 0 ? R : B;

    if (pos + 1 < 90 && !pid_on(pos + 1)) {
        if (pos + 11 < 90 && !same_team(team, pos + 11)) {
            ret.emplace_back(Move { pos, pos + 11 });
        }
        if (pos - 7 >= 0 && !same_team(team, pos - 7)) {
            ret.emplace_back(Move { pos, pos - 7 });
        }
    }
    if (pos - 1 >= 0 && !pid_on(pos - 1)) {
        if (pos - 11 >= 0 && !same_team(team, pos - 11)) {
            ret.emplace_back(Move { pos, pos - 11 });
        }
        if (pos + 7 < 90 && !same_team(team, pos + 7)) {
            ret.emplace_back(Move { pos, pos + 7 });
        }
    }
    if (pos + 9 < 90 && !pid_on(pos + 9)) {
        if (pos + 19 < 90 && !same_team(team, pos + 19)) {
            ret.emplace_back(Move { pos, pos + 19 });
        }
        if (pos + 17 >= 0 && !same_team(team, pos + 17)) {
            ret.emplace_back(Move { pos, pos + 17 });
        }
    }
    if (pos - 9 >= 0 && !pid_on(pos - 9)) {
        if (pos - 19 >= 0 && !same_team(team, pos - 19)) {
            ret.emplace_back(Move { pos, pos - 19 });
        }
        if (pos - 17 < 90 && !same_team(team, pos - 17)) {
            ret.emplace_back(Move { pos, pos - 17 });
        }
    }
    return ret;
}
