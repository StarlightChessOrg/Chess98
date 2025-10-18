#pragma once

// #define GENFILES

#ifdef GENFILES

#include "base.hpp"
#include "search.hpp"

// 基本配置
const std::string GENFILES_OUTPUT_DIR = "../nnue/data/"; // 首先你需要创建这个目录, 才能写这个目录。后面要加尾随斜杠
const int GENFILES_DEPTH = 6;                            // 最大搜索深度
const int GENFILES_RANDOM_MOVE_COUNT = 5;                // 每次随机走的步数
const int MAX_MOVES = 120;                               // 最多走多少步就认定为死循环局面, 直接判和

// 声明
Move getRandomMoveFromVector(const MOVES &vec);

// 工具函数
Move getRandomMoveFromVector(const MOVES &vec)
{
    std::mt19937_64 engine(std::chrono::high_resolution_clock::now().time_since_epoch().count());
    if (vec.empty())
        return {};
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

void saveGENFILES()
{
    if (GENFILES_filecontent.back() == ',')
    {
        GENFILES_filecontent.pop_back();
    }
    GENFILES_filecontent = replaceAll(GENFILES_filecontent, "}{", "},{");
    writeFile(GENFILES_OUTPUT_DIR + GENFILES_filename + ".json", GENFILES_filecontent + "]");
}

// 全局变量
std::string GENFILES_filecontent = "[";
bool GENFILES_appexit = false;
std::string GENFILES_filename = getUniqueRandomFilename();

// 搜索类
class SearchGenfiles : public Search
{
public:
    SearchGenfiles(PIECEID_MAP pieceidMap, TEAM team) : Search(pieceidMap, team) {};

public:
    std::vector<Result> rootresults{};

public:
    Result searchMain(int maxDepth, int maxTime);
};

Result SearchGenfiles::searchMain(int maxDepth, int maxTime)
{
    // 预制条件检查
    this->reset();
    if (!board.isKingLive(RED) || !board.isKingLive(BLACK))
    {
        // 将帅是否在棋盘上
        GENFILES_appexit = true;
        return Result{Move{}, 0};
    }
    else if (this->repeatCheck())
    {
        // 是否重复局面
        std::cout << " repeat situation!" << std::endl;
        return Result{Move{}, 0};
    }

    // 输出局面信息
    std::cout << "situation: " << pieceidmapToFen(board.pieceidMap, board.team) << std::endl;
    std::cout << "evaluate: " << board.evaluate() << std::endl;

    // 搜索
    this->rootMoves = MovesGenerate::getMoves(board);
    Result bestNode = Result(Move(), 0);
    auto start = std::chrono::high_resolution_clock::now();

    // nnue start
    std::string historyStr = "";
    for (const Move &move : board.historyMoves)
    {
        historyStr += std::to_string(move.id) + ",";
    }
    if (historyStr.size() > 0)
    {
        historyStr.pop_back();
    }
    std::string str = "";
    for (int depth = 1; depth <= maxDepth; depth++)
    {
        str = "{\"fen\":\"" + pieceidmapToFen(board.pieceidMap, board.team) + "\",\"history\":[" + historyStr + "],\"data\":[";
        bestNode = searchRoot(depth);

        auto end = std::chrono::high_resolution_clock::now();
        int duration = int(std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count());

        // log
        std::cout << " depth: " << depth;
        std::cout << " vl: " << bestNode.val;
        std::cout << " moveid: " << bestNode.move.id;
        std::cout << " duration(ms): " << duration;
        std::cout << std::endl;

        // nnue 记录根节点结果
        str += "{\"depth\":" + std::to_string(depth) + ",\"data\":[";
        for (const Result &result : this->rootresults)
        {
            str += "{";
            str += "\"moveid\":" + std::to_string(result.move.id);
            board.doMove(result.move);
            str += ",\"fen_after_move\":\"" + pieceidmapToFen(board.pieceidMap, board.team) + "\"";
            board.undoMove();
            str += ",\"vl\":" + std::to_string(result.val);
            str += "},";
        }
        if (str.back() == ',')
        {
            str.pop_back();
        }
        str += "]},";

        this->rootresults = {};

        // timeout break
        if (duration >= maxTime * 1000 / 3)
        {
            break;
        }
    }

    str.pop_back();
    str += "]},";
    GENFILES_filecontent += str;

    return bestNode;
}

// 生成函数
void genfiles()
{
    GENFILES_appexit = false;
    GENFILES_filename = getUniqueRandomFilename();
    int randomDepth = GENFILES_RANDOM_MOVE_COUNT;
    int maxDepth = GENFILES_DEPTH;

    const std::string fenCode = "rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RNBAKABNR w - - 0 1";
    SearchGenfiles *s = new SearchGenfiles(fenToPieceidmap(fenCode), RED);
    int count = 0;

    // 前几步随机
    for (int i = 0; i < randomDepth; i++)
    {
        count++;
        std::cout << count << "---------------" << std::endl;
        Result a = s->searchMain(maxDepth, 3);
        if (a.move.id != -1)
        {
            Move m = getRandomMoveFromVector(s->rootMoves);
            s->board.doMove(m);
        }
        else
        {
            GENFILES_appexit = true;
            break;
        }
        saveGENFILES();
    }
    while (s->board.historyMoves.size() < MAX_MOVES && GENFILES_appexit == false)
    {
        count++;
        std::cout << count << "---------------" << std::endl;
        Result a = s->searchMain(maxDepth, 3);
        if (a.move.id != -1)
        {
            s->board.doMove(a.move);
        }
        else
        {
            GENFILES_appexit = true;
            break;
        }
        saveGENFILES();
    }
    delete s;
    GENFILES_appexit = true;
    genfiles();
}

#endif
