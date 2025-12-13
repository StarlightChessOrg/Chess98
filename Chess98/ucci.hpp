#include "search.hpp"

class UCCI
{
public:
    UCCI() = default;

public:
    using MSG = std::string;
    const MSG SUCCESS_MSG = "";
    const MSG UCCIOK_MSG = "ucciok";
    const MSG READY_MSG = "readyok";

public:
    MSG ucci();
    MSG isready() const;
    MSG setoption(const std::string& name, const std::string& value);
    MSG position(const std::string& fenCode, const MOVES& moves);
    MSG banmoves(const MOVES& moves);
    MSG go();
    MSG stop();
    MSG quit();

public:
    std::unique_ptr<Search> search = nullptr;
    int maxTime = 3;
    int maxDepth = 20;
    bool searchCompleted = false;
    Result searchResult{};
    std::thread searchThread;

public:
    std::string fen() const { return pieceidmapToFen(search->board.pieceidMap, search->board.team); }
    MOVES history() const { return search->board.historyMoves; }
    std::string standardMoveConvert(Move move) const
    {
        std::string ret = "";
        ret += char('a' + move.y1);
        ret += char('0' + (9 - move.x1));
        ret += char('a' + move.y2);
        ret += char('0' + (9 - move.x2));
        return ret;
    }
};

// ucci
UCCI::MSG UCCI::ucci()
{
    std::string defaultFen = "rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RNBAKABNR w - - 0 1";
    this->search = std::make_unique<Search>(fenToPieceidmap(defaultFen), RED);
    return UCCIOK_MSG;
}

// isready
UCCI::MSG UCCI::isready() const
{
    return READY_MSG;
}

// setoption my_option_name my_option_value
UCCI::MSG UCCI::setoption(const std::string& name, const std::string& value)
{
    if (name == "usebook")
    {
        search->useBook = (value == "true" || value == "1");
    }
    else if (name == "newgame")
    {
        ucci();
    }
    else if (name == "usemillisec")
    {
        return SUCCESS_MSG;
    }
    else
    {
        return "Unknown option name";
    }
    return SUCCESS_MSG;
}

// position my_startpos_fen my_moves
UCCI::MSG UCCI::position(const std::string& fenCode, const MOVES& moves)
{
    PIECEID_MAP pieceidMap = fenToPieceidmap(fenCode);
    TEAM team = (fenCode.find("w") != std::string::npos) ? RED : BLACK;
    search = std::make_unique<Search>(pieceidMap, team);
    for (const Move& move : moves)
    {
        search->board.doMove(move);
    }
    return SUCCESS_MSG;
}

// banmove my_banned_moves
UCCI::MSG UCCI::banmoves(const MOVES& moves)
{
    for (const Move& move : moves)
    {
        search->bannedMoves[move.id] = 1;
    }
    return SUCCESS_MSG;
}

// stop
UCCI::MSG UCCI::go()
{
    searchThread = std::thread([&]() {
        Result result = search->searchMain(maxDepth, maxTime);
        searchCompleted = true;
        searchResult = result;
    });
    searchThread.detach();
    while (!searchCompleted)
    {
        wait(50);
    }
    return standardMoveConvert(searchResult.move);
}

// stop
UCCI::MSG UCCI::stop()
{
    Result result = search->info.getBestResult();
    return standardMoveConvert(result.move);
}

// quit
UCCI::MSG UCCI::quit()
{
    exit(0);
    return SUCCESS_MSG;
}
