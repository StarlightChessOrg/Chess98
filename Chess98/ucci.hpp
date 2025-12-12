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
    MOVES currentBannedMoves{};

public:
    std::string fen() const { return pieceidmapToFen(search->board.pieceidMap, search->board.team); }
    MOVES history() const { return search->board.historyMoves; }
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

UCCI::MSG UCCI::setoption(const std::string& name, const std::string& value)
{
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
    currentBannedMoves = moves;
    return SUCCESS_MSG;
}

UCCI::MSG UCCI::go()
{
    return SUCCESS_MSG;
}

UCCI::MSG UCCI::stop()
{
    return SUCCESS_MSG;
}

// quit
UCCI::MSG UCCI::quit()
{
    return SUCCESS_MSG;
}
