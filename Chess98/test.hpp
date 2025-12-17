#pragma once
#include "ui.hpp"
#include "ucci.hpp"

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
    
    // test situations

    // fenCode = "2bak4/3Ra4/3n5/p8/2b2PP1p/2NR5/P1r1N3P/1r2n4/4A4/2BK1A3 w - - 0 1"; // 漏招 未解决
    // fenCode = "3ckab2/2r1a4/2n1bc3/1RN1p3p/P5p2/9/3n2P1P/C3C3N/4A4/2B1KAB2 w - - 0 1"; // 漏招 未解决
    // fenCode = "5R3/C3k4/5a3/p1P4cp/2r3b2/3N5/P2n4P/B8/4A4/4KAB2 w - - 0 1"; // 漏招 未解决
    // fenCode = "r1baka3/6R2/1c2b4/pC2p3p/2n3P2/9/P3r3P/3C5/4N4/R1BAKAB2 w - - 0 1"; // 漏招 - 深度问题 未解决
    // fenCode = "3k1ab1C/4a4/4Cc3/p3N4/7c1/P1B6/9/3AB4/4A1n2/3K5 w - - 0 1"; // 漏招 - 未解决
    // fenCode = "1rbakabr1/9/n5n1c/p1p1p3p/6p2/9/P1cRP1P1P/1CN1B1NC1/5R3/3AKAB2 w - - 0 1";
    // fenCode = "1rbak4/4a4/4bc3/p3p3p/2p3P2/1C2P4/P1PN2nnP/2N6/1R3r3/1RBAKABC1 w - - 0 1";
    // fenCode = "3rkab2/2C1a4/4b1R2/p3p3p/3n3N1/9/P3P3P/4B4/9/1rCcKABR1 w - - 0 1";
    // fenCode = "2bak4/4a4/4b4/4R3N/pr4Pn1/2B6/1Cc1P4/1RN2n3/4K2r1/3A1A3 w - - 0 1";
    // fenCode = "2ba1a3/2Nk5/9/2P1P4/3N2p2/9/8P/4Bn3/3RK2c1/1r1A1AB2 w - - 0 1";

    ui(team, aiFirst, maxDepth, maxTime, fenCode);
}

void testByUCCI()
{
    UCCI ucci;
}
