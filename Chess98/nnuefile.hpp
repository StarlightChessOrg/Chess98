#pragma once

// #define NNUE

#ifdef NNUE
#include "base.hpp"

// 这里放配置
const std::string NNUE_OUTPUT_DIR = "../nnue/data/"; // 首先你需要创建这个目录, 才能写这个目录。后面要加尾随斜杠
const int NNUE_DEPTH = 6;                            // 最大搜索深度
const int NNUE_RANDOM_MOVE_COUNT = 5;
const int MAX_MOVES = 140;
#ifdef _WIN32
const std::string NNUE_RESTART_EXE_FILE = "./Chess98.exe"; // 跑完一局继续跑的exe文件路径
#elif __unix__
const std::string NNUE_RESTART_LINUX_FILE = "./a.out"; // 跑完一局继续跑的unix执行文件路径
#endif

template <typename T>
T getRandomFromVector(const std::vector<T> &vec)
{
    std::mt19937_64 engine(std::chrono::high_resolution_clock::now().time_since_epoch().count());
    if (vec.empty())
        return T();
    std::uniform_int_distribution<size_t> dist(0, vec.size() - 1);
    return vec[dist(engine)];
}

std::string replaceAll(std::string resource_str, std::string sub_str, std::string new_str)
{
    std::string dst_str = resource_str;
    std::string::size_type pos = 0;
    while ((pos = dst_str.find(sub_str)) != std::string::npos) // 替换所有指定子串
    {
        dst_str.replace(pos, sub_str.length(), new_str);
    }
    return dst_str;
}

std::string getUniqueRandomFilename()
{
    std::uniform_int_distribution<size_t> distA(65, 90);  // 大写字母
    std::uniform_int_distribution<size_t> distB(97, 122); // 小写字母
    std::uniform_int_distribution<size_t> distC(48, 57);  // 数字
    std::mt19937 engine(int(std::chrono::high_resolution_clock::now().time_since_epoch().count()));
    std::string filename = "";
    for (int i = 0; i < 16; i++)
    {
        size_t decision = std::uniform_int_distribution<size_t>(1, 3)(engine); // 决定
        if (decision == 1)
        {
            filename += char(distA(engine));
        }
        else if (decision == 2)
        {
            filename += char(distB(engine));
        }
        else
        {
            filename += char(distC(engine));
        }
    }
    return filename;
}

std::string NNUE_filecontent = "[";
bool NNUE_appexit = false;
std::string NNUE_filename = getUniqueRandomFilename();

void saveNNUE()
{
    if (NNUE_filecontent.back() == ',')
    {
        NNUE_filecontent.pop_back();
    }
    NNUE_filecontent = replaceAll(NNUE_filecontent, "}{", "},{");
    writeFile(NNUE_OUTPUT_DIR + NNUE_filename + ".json", NNUE_filecontent + "]");
}
#endif
