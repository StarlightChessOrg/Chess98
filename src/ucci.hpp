#pragma once
#include "search.hpp"
#include <sstream>
#include <thread>

constexpr const char* START_FEN_
    = "rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RNBAKABNR w";

std::vector<Move> g_banmoves { };
std::thread search_thread_ { };
UINT32 opt_duration_max_ { 1000 };
DEPTH opt_depth_max_ { 20 };

void ucci_loop();
void ucci_hello_(bool uci);
void ucci_newgame_();
void ucci_position_(const std::string& line);
void ucci_go_(const std::string& line);
void ucci_setoption_(const std::string& line);
void ucci_banmoves_(const std::string& line);
void ucci_join_();
void ucci_stop_();
void ucci_stop_join_();
TEAM fen_team_(const std::string& fen);
void apply_moves_(std::istringstream& in);

void ucci_join_()
{
    if (search_thread_.joinable()) search_thread_.join();
}

void ucci_stop_()
{
    g_searchstop = 1;
}

void ucci_stop_join_()
{
    ucci_stop_();
    ucci_join_();
}

TEAM fen_team_(const std::string& fen)
{
    const auto sp = fen.find(' ');
    if (sp == std::string::npos || sp + 1 >= fen.size()) return R;
    const char c = fen[sp + 1];
    return (c == 'b' || c == 'B') ? B : R;
}

void apply_moves_(std::istringstream& in)
{
    std::string tok;
    while (in >> tok) {
        if (tok == "moves") continue;
        const Move m = ucimove_to_move(tok);
        if (m) position_move(m);
    }
}

void ucci_hello_(bool uci)
{
    g_uci = uci;
    std::cout << "id name Chess98" << std::endl;
    std::cout << "id author Chess98" << std::endl;
    const char* prefix = uci ? "option name " : "option ";
    std::cout << prefix << "search duration max type spin default "
              << opt_duration_max_ << " min 1 max 86400000" << std::endl;
    std::cout << prefix << "search depth max type spin default "
              << int(opt_depth_max_) << " min 1 max 64" << std::endl;
    std::cout << (uci ? "uciok" : "ucciok") << std::endl;
}

void ucci_newgame_()
{
    ucci_stop_join_();
    history_init();
    killer_init();
    tt_init();
    g_banmoves.clear();
    g_searchstop = 0;
    position_init(fen_to_matrix(START_FEN_), R);
}

void ucci_position_(const std::string& line)
{
    ucci_stop_join_();
    std::istringstream in(line);
    std::string tok;
    in >> tok; // position
    if (!(in >> tok)) return;

    g_banmoves.clear();
    if (tok == "startpos") {
        position_init(fen_to_matrix(START_FEN_), R);
        apply_moves_(in);
        return;
    }

    if (tok == "fen") in >> tok;
    std::string fen = tok;
    std::string part;
    while (in >> part) {
        if (part == "moves") break;
        fen += ' ';
        fen += part;
    }
    position_init(fen_to_matrix(fen), fen_team_(fen));
    if (part == "moves") apply_moves_(in);
}

void ucci_banmoves_(const std::string& line)
{
    g_banmoves.clear();
    std::istringstream in(line);
    std::string tok;
    in >> tok;
    while (in >> tok) {
        const Move m = ucimove_to_move(tok);
        if (m) g_banmoves.push_back(m);
    }
}

void ucci_setoption_(const std::string& line)
{
    std::istringstream in(line);
    std::string tok;
    in >> tok; // setoption
    if (!(in >> tok)) return;
    if (tok == "name" && !(in >> tok)) return;
    std::string name = tok;
    while (in >> tok && tok != "value") {
        name += ' ';
        name += tok;
    }
    int vl { 0 };
    const bool has_vl = tok == "value" && bool(in >> vl);
    if (name == "newgame") {
        ucci_newgame_();
        return;
    }
    if (!has_vl) return;
    if (name == "search duration max") {
        opt_duration_max_ = UINT32(std::clamp(vl, 1, 86400000));
    } else if (name == "search depth max") {
        opt_depth_max_ = DEPTH(std::clamp(vl, 1, 64));
    }
}

void ucci_go_(const std::string& line)
{
    std::istringstream in(line);
    std::string tok;
    in >> tok; // go

    int depth { -1 };
    int movetime { -1 };
    int time_left { -1 };
    int wtime { -1 };
    int btime { -1 };
    int winc { 0 };
    int binc { 0 };
    int inc { 0 };
    bool infinite { false };

    while (in >> tok) {
        if (tok == "depth") in >> depth;
        else if (tok == "movetime")
            in >> movetime;
        else if (tok == "time")
            in >> time_left;
        else if (tok == "opptime")
            in >> tok; // reserved
        else if (tok == "increment" || tok == "inc")
            in >> inc;
        else if (tok == "oppincrement")
            in >> tok; // reserved
        else if (tok == "wtime")
            in >> wtime;
        else if (tok == "btime")
            in >> btime;
        else if (tok == "winc")
            in >> winc;
        else if (tok == "binc")
            in >> binc;
        else if (tok == "infinite")
            infinite = true;
        else if (tok == "ponder" || tok == "draw") {
        } // reserved
        else if (tok == "nodes" || tok == "mate" || tok == "movestogo")
            in >> tok;
    }

    if (wtime >= 0 || btime >= 0) {
        time_left = g_team == R ? wtime : btime;
        inc = g_team == R ? winc : binc;
    }

    g_maxdepth = opt_depth_max_;
    g_searchduration = opt_duration_max_;
    if (depth >= 0) {
        g_maxdepth = DEPTH(std::min(std::max(depth, 1), int(opt_depth_max_)));
    }
    if (movetime >= 0) {
        g_searchduration = UINT32(std::min(std::max(movetime, 1), int(opt_duration_max_)));
    } else if (time_left >= 0) {
        const int slice = std::max(time_left / 30 + inc, 20);
        g_searchduration = UINT32(std::min(slice, int(opt_duration_max_)));
    }

    ucci_stop_join_();
    g_searchstop = 0;
    distance_ = 0;
    search_thread_ = std::thread([] {
        const SEARCH_RET ret = search();
        if (ret.first) {
            std::cout << "bestmove " << move_to_ucimove(ret.first) << std::endl;
        } else {
            std::cout << (g_uci ? "bestmove none" : "nobestmove") << std::endl;
        }
    });
}

void ucci_loop()
{
    ucci_newgame_();
    std::string line;
    while (std::getline(std::cin, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (line.empty()) continue;
        if (line == "ucci") ucci_hello_(false);
        else if (line == "uci")
            ucci_hello_(true);
        else if (line == "isready")
            std::cout << "readyok" << std::endl;
        else if (line == "quit") {
            ucci_stop_join_();
            std::cout << "bye" << std::endl;
            break;
        } else if (line == "stop")
            ucci_stop_();
        else if (line == "ucinewgame")
            ucci_newgame_();
        else if (line == "ponderhit") {
        } // reserved
        else if (line.rfind("position", 0) == 0)
            ucci_position_(line);
        else if (line.rfind("go", 0) == 0)
            ucci_go_(line);
        else if (line.rfind("setoption", 0) == 0)
            ucci_setoption_(line);
        else if (line.rfind("banmoves", 0) == 0)
            ucci_banmoves_(line);
    }
}
