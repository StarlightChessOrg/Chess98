#pragma once
#include "ucci.hpp"
#include "ui.hpp"

void testByUI()
{
    TEAM team = RED;
#ifndef AI_FIRST
    bool aiFirst = true;
#else
    bool aiFirst = bool(AI_FIRST);
#endif
    int maxDepth = 20;
    int maxTime = 3000;
    std::string fenCode = "rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RNBAKABNR w - - 0 1";

    // 一些表现较差的局面
    // fenCode = "2bak4/3Ra4/3n5/p8/2b2PP1p/2NR5/P1r1N3P/1r2n4/4A4/2BK1A3 w - - 0 1";
    // fenCode = "5R3/C3k4/5a3/p1P4cp/2r3b2/3N5/P2n4P/B8/4A4/4KAB2 w - - 0 1";
    // fenCode = "1rbakabr1/9/n5n1c/p1p1p3p/6p2/9/P1cRP1P1P/1CN1B1NC1/5R3/3AKAB2 w - - 0 1";
    // fenCode = "1rbak4/4a4/4bc3/p3p3p/2p3P2/1C2P4/P1PN2nnP/2N6/1R3r3/1RBAKABC1 w - - 0 1";
    // fenCode = "3rkab2/2C1a4/4b1R2/p3p3p/3n3N1/9/P3P3P/4B4/9/1rCcKABR1 w - - 0 1";
    // fenCode = "2bak4/4a4/4b4/4R3N/pr4Pn1/2B6/1Cc1P4/1RN2n3/4K2r1/3A1A3 w - - 0 1";
    // fenCode = "2ba1a3/2Nk5/9/2P1P4/3N2p2/9/8P/4Bn3/3RK2c1/1r1A1AB2 w - - 0 1";
    // fenCode = "2b1k4/3Pa4/4b1C2/p1C1pR2p/5c3/5r3/P3P3P/2N2A2B/4AKn2/9 w - - 0 1";

    ui(team, aiFirst, maxDepth, maxTime, fenCode);
}

void testByUCCI()
{
    UCCI ucci;
}
