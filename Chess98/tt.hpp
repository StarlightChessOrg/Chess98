#include "base.hpp"

namespace {

struct TtItem {
    TtItem() = default;

    int hash_lock { 0 };
    int vlExact { 0 };
    int vlBeta { 0 };
    int vlAlpha { 0 };
    int exactDepth { 0 };
    int betaDepth { 0 };
    int alphaDepth { 0 };
    Move exact_move {};
    Move beta_move {};
    Move alpha_move {};
};

}
