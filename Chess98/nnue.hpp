#pragma once

#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <cassert>
#include <algorithm>
#include <iostream>
#include <cmath>
#include "base.hpp"

class NNUE
{
public:
    static constexpr int INPUT_SIZE = 7 * 9 * 10;
    static constexpr int L1_SIZE = 256;
    static constexpr int L2_SIZE = 32;
    static constexpr int L3_SIZE = 32;
    static constexpr int OUTPUT_SIZE = 2;

    const int quantization_scale = 8;

    std::vector<std::vector<int>> w1;
    std::vector<int> b1;
    std::vector<std::vector<int>> w2;
    std::vector<int> b2;
    std::vector<std::vector<int>> w3;
    std::vector<int> b3;
    std::vector<std::vector<int>> w4;
    std::vector<int> b4;

    NNUE(const std::string& prefix = "")
    {
        loadLayer(prefix + "layer_1_weights.txt", prefix + "layer_1_biases.txt", w1, b1, INPUT_SIZE, L1_SIZE);
        loadLayer(prefix + "layer_2_weights.txt", prefix + "layer_2_biases.txt", w2, b2, L1_SIZE, L2_SIZE);
        loadLayer(prefix + "layer_3_weights.txt", prefix + "layer_3_biases.txt", w3, b3, L2_SIZE, L3_SIZE);
        loadLayer(prefix + "layer_4_weights.txt", prefix + "layer_4_biases.txt", w4, b4, L3_SIZE, OUTPUT_SIZE);
    }

    int evaluate(const PIECEID_MAP& pieceidMap, TEAM team)
    {
        std::vector<int> active_plus;
        std::vector<int> active_minus;
        buildInput(pieceidMap, team, active_plus, active_minus);

        std::vector<int> l1(L1_SIZE, 0);
        for (int i = 0; i < L1_SIZE; ++i) l1[i] = b1[i];
        for (int idx : active_plus)
            for (int j = 0; j < L1_SIZE; ++j) l1[j] += w1[idx][j];
        for (int idx : active_minus)
            for (int j = 0; j < L1_SIZE; ++j) l1[j] -= w1[idx][j];
        for (int& x : l1)
            if (x < 0) x = 0;

        std::vector<int> l2(L2_SIZE, 0);
        for (int j = 0; j < L2_SIZE; ++j)
        {
            int sum = b2[j];
            for (int i = 0; i < L1_SIZE; ++i) sum += l1[i] * w2[i][j];
            l2[j] = (sum > 0) ? sum : 0;
        }

        std::vector<int> l3(L3_SIZE, 0);
        for (int j = 0; j < L3_SIZE; ++j)
        {
            int sum = b3[j];
            for (int i = 0; i < L2_SIZE; ++i) sum += l2[i] * w3[i][j];
            l3[j] = (sum > 0) ? sum : 0;
        }

        std::vector<int> out(OUTPUT_SIZE, 0);
        for (int j = 0; j < OUTPUT_SIZE; ++j)
        {
            int sum = b4[j];
            for (int i = 0; i < L3_SIZE; ++i) sum += l3[i] * w4[i][j];
            out[j] = sum;
        }

        return out[0];
    }

    int evaluateFromL1PreActivation(const std::vector<int>& l1_pre)
    {
        assert(l1_pre.size() == L1_SIZE);
        std::vector<int> l1 = l1_pre;
        for (int& x : l1)
            if (x < 0) x = 0;

        std::vector<int> l2(L2_SIZE, 0);
        for (int j = 0; j < L2_SIZE; ++j)
        {
            int sum = b2[j];
            for (int i = 0; i < L1_SIZE; ++i) sum += l1[i] * w2[i][j];
            l2[j] = (sum > 0) ? sum : 0;
        }

        std::vector<int> l3(L3_SIZE, 0);
        for (int j = 0; j < L3_SIZE; ++j)
        {
            int sum = b3[j];
            for (int i = 0; i < L2_SIZE; ++i) sum += l2[i] * w3[i][j];
            l3[j] = (sum > 0) ? sum : 0;
        }

        std::vector<int> out(OUTPUT_SIZE, 0);
        for (int j = 0; j < OUTPUT_SIZE; ++j)
        {
            int sum = b4[j];
            for (int i = 0; i < L3_SIZE; ++i) sum += l3[i] * w4[i][j];
            out[j] = sum;
        }

        return out[0];
    }

protected:
    void buildInput(const PIECEID_MAP& pieceidMap, TEAM team, std::vector<int>& active_plus,
                    std::vector<int>& active_minus)
    {
        active_plus.clear();
        active_minus.clear();
        for (int x = 0; x < 9; ++x)
        {
            for (int y = 0; y < 10; ++y)
            {
                PIECEID pieceId = pieceidMap[x][y];
                if (pieceId != EMPTY_PIECEID)
                {
                    int pieceType = std::abs(pieceId) - 1;
                    int baseIndex = pieceType * 9 * 10 + x * 10 + y;
                    if ((pieceId > 0 && team == RED) || (pieceId < 0 && team == BLACK))
                    {
                        active_plus.push_back(baseIndex);
                    }
                    else
                    {
                        active_minus.push_back(baseIndex);
                    }
                }
            }
        }
    }

    void loadLayer(const std::string& weightFile, const std::string& biasFile, std::vector<std::vector<int>>& weights,
                   std::vector<int>& biases, int inSize, int outSize)
    {
        std::ifstream wf(weightFile);
        std::ifstream bf(biasFile);
        assert(wf && bf);

        weights.resize(inSize, std::vector<int>(outSize));
        biases.resize(outSize);

        std::string line;
        for (int i = 0; i < inSize; ++i)
        {
            std::getline(wf, line);
            std::istringstream iss(line);
            for (int j = 0; j < outSize; ++j)
            {
                float val;
                iss >> val;
                weights[i][j] = static_cast<int>(std::round(val * quantization_scale));
            }
        }

        for (int j = 0; j < outSize; ++j)
        {
            float val;
            bf >> val;
            biases[j] = static_cast<int>(std::round(val * quantization_scale));
        }
    }
};

void testNNUE()
{
    NNUE* pModel = new NNUE("E:\\Projects_chess\\Chess98\\nnue\\models\\epoch_2025_9_20\\");

    std::vector<std::vector<std::string>> fens = {
        {"rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C2C4/9/RNBAKABNR b - - 0 1", "炮二平五"},
        {"rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/4C2C1/9/RNBAKABNR w - - 0 1", "炮八平五"},
        {"rnbakab1r/9/1c5cn/p1p1p1p1p/9/9/P1P1P1P1P/1C2C4/9/RNBAKABNR w - - 0 1", "炮二平五->马8进9"},
        {"rnbakab1r/9/1c4nc1/p1p1p1p1p/9/9/P1P1P1P1P/1C2C4/9/RNBAKABNR w - - 0 1", "炮二平五->马8进7"},
        {"rnbakab1r/9/1c5cn/p1p1C1p1p/9/9/P1P1P1P1P/1C7/9/RNBAKABNR b - - 0 1", "炮二平五->马8进9->炮五进四"},
        {"rCbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/7C1/9/RNBAKABNR b - - 0 1", "炮八进七"},
        {"1rbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/7C1/9/RNBAKABNR w - - 0 1", "炮八进七->车2平1"},
        {"rnbakabnr/1N1R1R3/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/2BAKABN1 w - - 0 1",
         "红方双车卡黑方九宫肋道，辅以红马叫杀"},
        {"2bakabn1/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/1n1r1r3/RNBAKABNR b - - 0 1",
         "黑方双车卡红方九宫肋道，辅以黑马叫杀"},
        {"1Cbak4/9/3a5/p1p1c1p1p/5r3/2P1P4/P7P/9/4A4/2BK1Ar2 w - - 0 1", "红方沉底炮，黑方双车带炮占大优"},
        {"2bak4/9/3a5/p1p1c1p1p/5r3/2P1P4/P7P/9/4A4/2BK1Ar2 w - - 0 1", "去掉红方沉底炮，黑方双车带炮占大优"},
        {"r1bakabnr/9/1cn4c1/p1p1p1p1p/9/2P6/P3P1P1P/1C5C1/9/RNBAKABNR w - - 0 1", "兵七进一->马2进3"},
        {"rnbakab1r/9/1c4nc1/p1p1p1p1p/9/2P6/P3P1P1P/1C5C1/9/RNBAKABNR w - - 0 1", "兵七进一->马8进7"},
        {"rnbakabnr/9/1c5c1/p1p1p3p/6p2/2P6/P3P1P1P/1C5C1/9/RNBAKABNR w - - 0 1", "兵七进一->兵7进1"},
        {"rnbakabnr/9/1c7/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/1NBAKABN1 w - - 0 1", "开局红方让双车，黑方让单炮"},
        {"3k5/9/9/9/9/9/9/4K4/4A4/4C4 w - - 0 1", "残局，红帅升天居中，帅后店士，士后垫炮，除黑将外再无任何子力"},
        {"3k5/9/9/9/9/9/9/5K3/4A4/4C4 w - - 0 1", "残局，红帅升天居右，帅后店士，士后垫炮，除黑将外再无任何子力"},
        {"3k5/9/9/9/9/9/9/9/4A4/4CK3 w - - 0 1", "残局，红帅居起始位置右侧，帅后店士，士后垫炮，除黑将外再无任何子力"},
        {"rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RNBAKABNR w - - 0 1", "开局尚未动任何一子"},
        {"rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/4K4/RNBA1ABNR b - - 0 1", "帅五进一"},
        {"rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RNBA1ABNR w - - 0 1", "红缺帅"},
        {"rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/BC5C1/9/RN1AKABNR b - - 0 1", "相七进九"},
        {"rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/BC5C1/9/RN1AKABNR w - - 0 1", "相七进九"},
    };
    for (int i = 0; i < fens.size(); i++)
    {
        auto res =
            pModel->evaluate(fenToPieceidmap(fens[i][0]), fens[i][0].find('w') != std::string::npos ? RED : BLACK);
        std::cout << fens[i][1] << ": " << res << std::endl;
    }
    delete pModel;
}