#include "search.hpp"

class UCCI
{
public:
    UCCI()
    {
        std::string defaultFen = "rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RNBAKABNR w - - 0 1";
        this->search = std::make_unique<Search>(fenToPieceidmap(defaultFen), RED);
        cli();
    };

public:
    void cli();

public:
    void ucci() const;
    void isready() const;
    void setoption(const std::string& name, const std::string& value);
    void position(const std::string& fenCode, const MOVES& moves);
    void banmoves(const MOVES& moves);
    void go(int time, int depth);
    void stop();
    void quit();

public:
    std::unique_ptr<Search> search = nullptr;
    int maxTime = 3000;
    int maxDepth = 20;
    bool ready = false;
    bool searchCompleted = true;
    Result searchResult{};
    std::thread searchThread{};

public:
    std::string fen() const { return pieceidmapToFen(search->board.pieceidMap, search->board.team); }
    MOVES history() const { return search->board.historyMoves; }
    std::string convertToUCCIMove(Move move) const
    {
        std::string ret = "";
        ret += char('a' + move.x1);
        ret += char('0' + move.y1);
        ret += char('a' + move.x2);
        ret += char('0' + move.y2);
        return ret;
    }
    Move convertToEngineMove(std::string movestr) const
    {
        int x1 = movestr[0] - 'a';
        int y1 = movestr[1] - '0';
        int x2 = movestr[2] - 'a';
        int y2 = movestr[3] - '0';
        return Move(x1, y1, x2, y2);
    }
    MOVES parseMovesInput(std::string moves) const
    {
        if (moves[moves.length() - 1] == ' ')
        {
            moves += " ";
        }
        // 按空格将moves切开
        MOVES moveList;
        size_t start = 0;
        size_t end = moves.find(' ');
        while (end != std::string::npos)
        {
            std::string moveStr = moves.substr(start, end - start);
            if (moveStr.length() == 4)
            {
                moveList.emplace_back(convertToEngineMove(moveStr));
            }
            start = end + 1;
            end = moves.find(' ', start);
        }
        return moveList;
    }
};

// cli
void UCCI::cli()
{
    while (true)
    {
        // 不断获取输入值
        std::string cmd;
        std::getline(std::cin, cmd);

        // 任何状态下都可以进行的指令
        if (cmd == "ucci")
        {
            ucci();
        }
        else if (cmd == "isready")
        {
            isready();
        }
        else if (cmd.substr(0, 9) == "setoption")
        {
            size_t name_pos = cmd.find("name");
            size_t value_pos = cmd.find("value");
            std::string name = cmd.substr(name_pos + 5, value_pos - name_pos - 6);
            std::string value = cmd.substr(value_pos + 6);
            setoption(name, value);
        }
        else if (cmd == "quit")
        {
            quit();
        }
        if (!searchCompleted) // 搜索中才可以进行的指令
        {
            if (cmd == "stop")
            {
                stop();
            }
        }
        else // 没有进入搜索状态才可以进行的指令
        {
            if (cmd.substr(0, 2) == "go")
            {
                // AI 生成的一个movetime和depth解析的代码
                int timeArg = maxTime;
                int depthArg = maxDepth;

                size_t pos = 2;
                auto nextToken = [&](size_t& p) -> std::string {
                    p = cmd.find_first_not_of(' ', p);
                    if (p == std::string::npos) return "";
                    size_t q = cmd.find(' ', p);
                    std::string tok = (q == std::string::npos) ? cmd.substr(p) : cmd.substr(p, q - p);
                    p = (q == std::string::npos) ? std::string::npos : q + 1;
                    return tok;
                };

                std::string token = nextToken(pos);
                while (!token.empty())
                {
                    if (token == "depth")
                    {
                        std::string val = nextToken(pos);
                        if (!val.empty())
                        {
                            depthArg = std::stoi(val);
                        }
                    }
                    else if (token == "movetime")
                    {
                        std::string val = nextToken(pos);
                        if (!val.empty())
                        {
                            timeArg = std::stoi(val);
                        }
                    }
                    token = nextToken(pos);
                }

                go(timeArg, depthArg);
            }
            else if (cmd.substr(0, 12) == "position fen")
            {
                std::string fen = "";
                std::string moves = "";
                size_t moves_pos = cmd.find("moves");
                std::cout << cmd.substr(0, 12) << std::endl;

                if (moves_pos == std::string::npos) // 没有moves参数的情况
                {
                    fen = cmd.substr(13);
                    position(fen, MOVES{});
                }
                else // 有moves参数的情况
                {
                    fen = cmd.substr(13, moves_pos - 10);
                    moves = cmd.substr(moves_pos + 6) + " ";
                    MOVES moveList = parseMovesInput(moves);
                    position(fen, moveList);
                }
                continue;
            }
            else if (cmd.substr(0, 8) == "banmoves")
            {
                std::string moves = cmd.substr(9);
                MOVES moveList = parseMovesInput(moves);
                banmoves(moveList);
            }
        }
    }
}

// ucci
void UCCI::ucci() const
{
    std::cout << "ucciok" << std::endl;
}

// isready
void UCCI::isready() const
{
    std::cout << "readyok" << std::endl;
}

// setoption my_option_name my_option_value
void UCCI::setoption(const std::string& name, const std::string& value)
{
    if (name == "usebook")
    {
        search->useBook = (value == "true" || value == "1");
    }
    else if (name == "usemillisec")
    {
        return;
    }
}

// position my_startpos_fen my_moves
void UCCI::position(const std::string& fenCode, const MOVES& moves)
{
    PIECEID_MAP pieceidMap = fenToPieceidmap(fenCode);
    TEAM team = (fenCode.find("w") != std::string::npos) ? RED : BLACK;
    search = std::make_unique<Search>(pieceidMap, team);
    for (const Move& move : moves)
    {
        search->board.doMove(move);
    }
}

// banmove my_banned_moves
void UCCI::banmoves(const MOVES& moves)
{
    for (const Move& move : moves)
    {
        search->bannedMoves[move.id] = 1;
    }
}

// stop
void UCCI::go(int time, int depth)
{
    maxTime = time;
    maxDepth = depth;
    searchThread = std::thread([&]() {
        searchCompleted = false;
        Result result = search->searchMain(maxDepth, maxTime);
        // 如果search进程没有被强行终止
        if (!searchCompleted)
        {
            std::cout << "bestmove " << convertToUCCIMove(result.move) << std::endl;
            searchCompleted = true;
            searchResult = result;
        }
    });
    searchThread.detach();
}

// stop
void UCCI::stop()
{
    // 强行终止search进程
    searchCompleted = true;
    search->stop = true;
    // 获取目前搜索到的最佳结果
    searchResult = search->info.getBestResult();
    // 输出结果
    std::cout << "bestmove " << convertToUCCIMove(searchResult.move) << std::endl;
}

// quit
void UCCI::quit()
{
    std::exit(0);
}
