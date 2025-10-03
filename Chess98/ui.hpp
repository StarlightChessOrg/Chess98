#pragma once
#include "board.hpp"
#include "search.hpp"

using BOARD_CODE = std::string;

const char SERVER_CODE[] = "\
const http = require('http')\n\
const fs = require('fs')\n\
\n\
let boardCode = 'null'\n\
let computerMove = 'null'\n\
let getBoardCode = () => boardCode\n\
\n\
let file = fs.openSync('./_move_.txt', 'w+')\n\
fs.writeFileSync(file, '____')\n\
fs.closeSync(file)\n\
\n\
const server = http.createServer((request, response) => {\n\
    const { method, url } = request\n\
    response.setHeader('Access-Control-Allow-Origin', '*')\n\
    response.setHeader('Access-Control-Allow-Methods', 'GET, DELETE, PATCH, OPTIONS')\n\
    response.setHeader('Access-Control-Allow-Headers', 'Content-Type')\n\
\n\
    if (method === 'GET' && url === '/boardcode') { // 界面端获取当前棋盘局势图\n\
        response.writeHead(200, { 'Content-Type': 'text/plain' })\n\
        response.end(getBoardCode() + '\\n')\n\
    }\n\
    else if (method === 'PUT' && url.match('boardcode')) { // 人机方面做出决策更改服务器棋盘局势图\n\
        response.writeHead(200, { 'Content-Type': 'text/plain' })\n\
        response.end('successful\\n')\n\
        boardCode = request.url.split('=')[1]\n\
    }\n\
    else if (method === 'PUT' && url.match('move')) { // 获取玩家着法\n\
        response.writeHead(200, { 'Content-Type': 'text/plain' })\n\
        response.end('successful\\n')\n\
        computerMove = request.url.split('=')[1]\n\
    }\n\
    else if (method == 'GET' && url.match('move')) { // 执行玩家着法\n\
        response.writeHead(200, { 'Content-Type': 'text/plain' })\n\
        response.end('successful\\n')\n\
        let move = request.url.split('=')[1]\n\
        const fileWrite = () => {\n\
            setTimeout(() => {\n\
                try {\n\
                    let file = fs.openSync('./_move_.txt', 'w+')\n\
                    fs.writeFileSync(file, move)\n\
                    fs.closeSync(file)\n\
                }\n\
                catch (e) {\n\
                    fileWrite()\n\
                }\n\
            }, 50)\n\
        }\n\
        fileWrite()\n\
    }\n\
    else if (method === 'GET' && url.match('computer')) { // 界面端获取电脑着法\n\
        response.writeHead(200, { 'Content-Type': 'text/plain' })\n\
        response.end(computerMove + '\\n')\n\
    }\n\
    else if (method == 'GET' && url.match('undo')) { // 悔棋\n\
        response.writeHead(200, { 'Content-Type': 'text/plain' })\n\
        response.end('successful\\n')\n\
        const fileWrite = () => {\n\
            setTimeout(() => {\n\
                try {\n\
                    let file = fs.openSync('./_move_.txt', 'w+')\n\
                    fs.writeFileSync(file, 'undo')\n\
                    fs.closeSync(file)\n\
                }\n\
                catch (e) {\n\
                    fileWrite()\n\
                }\n\
            }, 50)\n\
        }\n\
        fileWrite()\n\
    }\n\
})\n\
server.on('error', () => { })\n\
server.listen(9494)\n\
";

BOARD_CODE generateCode(Board &board)
{
    BOARD_CODE code = "";
    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            PIECEID pieceid = board.pieceidOn(i, j);
            std::string name = PIECE_NAME_PAIRS.at(pieceid);
            code += name;
        }
    }
    return code;
}

PIECEID_MAP decode(BOARD_CODE code)
{
    PIECEID_MAP result{};
    for (int i = 0; i < 90; i++)
    {
        size_t x = i / 10;
        size_t y = i % 10;
        char c1 = code[i * size_t(2) - size_t(1)];
        char c2 = code[i * size_t(2)];
        std::string pieceName{c1, c2};
        result[x][y] = NAME_PIECE_PAIRS.at(pieceName);
    }
    return result;
}

void setBoardCode(Board &board)
{
    const BOARD_CODE code = generateCode(board);
    const std::string historyMovesBack =
        board.historyMoves.size() > 0 ? std::to_string(board.historyMoves.back().id) : "null";
    const std::string jsPutCode = "\
        const http = require('http')\n\
        const options = {\n\
            hostname: '127.0.0.1',\n\
            path: '/?boardcode=" + code +
                                  "',\n\
            port: 9494,\n\
            method : 'PUT'\n\
        }\n\
        http.request(options).end();\n\
        const options2 = {\n\
            hostname: '127.0.0.1',\n\
            path: '/?move=" + historyMovesBack +
                                  "',\n\
            port: 9494,\n\
            method : 'PUT'\n\
        }\n\
        http.request(options2).end();\n\
            ";

    sleep_(200);
    writeFile("./_put_.js", jsPutCode);
    system_("node ./_put_.js");
}

void ui(TEAM team, bool aiFirst, int maxDepth, int maxTime, std::string fenCode)
{
    // 初始局面
    PIECEID_MAP pieceidMap = fenToPieceidMap(fenCode);

    // variables
    int count = 0;
    Search s = Search(pieceidMap, team);
    Board &board = s.getBoard();

    // 界面
    writeFile("./_server_.js", SERVER_CODE);
    system_("powershell.exe -command \"& {Start-Process -WindowStyle hidden node _server_.js}\"");
    setBoardCode(board);
    board.print();
    std::string moveFileContent = "____";

    while (true)
    {
        if (board.team == (aiFirst ? team : -team))
        {
            count++;
            std::cout << count << "---------------------" << std::endl;

            // 人机做出决策
            Result node = s.searchMain(maxDepth, maxTime);
            board.doMove(node.move);
            if (inCheck(board, board.team))
                board.historyMoves.back().isCheckingMove = true;

            setBoardCode(board);
            moveFileContent = readFile("./_move_.txt");
        }
        else
        {
            // 读取文件
            std::string content = readFile("./_move_.txt");

            // 悔棋
            if (content == "undo" && board.historyMoves.size() > 1)
            {
                count--;
                board.undoMove();
                board.undoMove();

                setBoardCode(board);
                writeFile("./_move_.txt", "wait");
                moveFileContent = "wait";
            }

            // 如果内容和上次内容不一致, 则执行步进
            if (content != "wait" && content != "undo" && content != moveFileContent)
            {
                try
                {
                    moveFileContent = content;
                    int x1 = std::stoi(content.substr(0, 1));
                    int y1 = std::stoi(content.substr(1, 1));
                    int x2 = std::stoi(content.substr(2, 1));
                    int y2 = std::stoi(content.substr(3, 1));
                    Move move{x1, y1, x2, y2};
                    board.doMove(move);
                }
                catch (std::exception &e)
                {
                    // 避免转换失败导致崩溃
                    std::cerr << "Invalid move: " << moveFileContent << std::endl;
                    system_("pause");
                    throw e;
                }
            }
        }
        sleep_(50);
    }
}
