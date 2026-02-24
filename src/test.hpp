#pragma once
#include "search.hpp"
#include "utils.hpp"

void test()
{
    const std::string fen = "rheakaehr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RHEAKAEHR w - - 0 1";
    search::init(utils::fen_to_matrix(fen), R);
    search::search(3, -INF, INF, PV);
    std::cout << search::node_number << std::endl;
    system("pause");
}
