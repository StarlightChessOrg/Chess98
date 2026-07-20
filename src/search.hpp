#pragma once
#include "evaluate.hpp"
#include "moves.hpp"

constexpr DEPTH Q_MAX_DISTANCE { 64 };
constexpr DEPTH Q_CHECKING_DEPTH { 8 };
constexpr VL FP_MARGIN { 120 };
constexpr bool NODE_PV { false };
constexpr bool NODE_CUT { true };

inline STATE g_searchstop { 0 };
inline DEPTH g_maxdepth { 60 };
inline DEPTH distance_ { 0 };

VL search_q_(VL a, VL b, DEPTH depth);
SEARCH_RET search();
template <bool CUT> VL search_vl_(DEPTH depth, VL a, VL b);
