#include <Windows.h>
#include "board.hpp"

using BOARD_CODE = std::string;

BOARD_CODE generateCode(Board board)
{
    BOARD_CODE code = "";
    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 10; j++)
        {
            PIECEID piece = board.pieceidOn(i, j);
            auto name = getPieceName(piece);
            if (name == "  ")
                name = "__";
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
        if (pieceName == "__")
            result[x][y] = 0;
        else if (pieceName == "RR")
            result[x][y] = R_ROOK;
        else if (pieceName == "RN")
            result[x][y] = R_KNIGHT;
        else if (pieceName == "RB")
            result[x][y] = R_BISHOP;
        else if (pieceName == "RG")
            result[x][y] = R_GUARD;
        else if (pieceName == "RK")
            result[x][y] = R_KING;
        else if (pieceName == "RC")
            result[x][y] = R_CANNON;
        else if (pieceName == "RP")
            result[x][y] = R_PAWN;
        else if (pieceName == "BR")
            result[x][y] = B_ROOK;
        else if (pieceName == "BN")
            result[x][y] = B_KNIGHT;
        else if (pieceName == "BB")
            result[x][y] = B_BISHOP;
        else if (pieceName == "BG")
            result[x][y] = B_GUARD;
        else if (pieceName == "BK")
            result[x][y] = B_KING;
        else if (pieceName == "BC")
            result[x][y] = B_CANNON;
        else if (pieceName == "BP")
            result[x][y] = B_PAWN;
        else
            std::cerr << "Invalid piece name: " << pieceName << std::endl;
    }
    return result;
}

const BOARD_CODE jsServerCode =
    "\
    const http = require('http');\n\
    const fs = require('fs')\n\
    \n\
    let boardCode = 'null'\n\
    let getBoardCode = () => boardCode\n\
    \n\
    let file = fs.openSync('./_move_.txt', 'w+')\n\
    fs.writeFileSync(file, '____')\n\
    fs.closeSync(file)\n\
    \n\
    http.createServer((request, response) => {\n\
        const { method } = request\n\
        response.setHeader('Access-Control-Allow-Origin', '*');\n\
        response.setHeader('Access-Control-Allow-Methods', 'GET, POST, PUT, OPTIONS');\n\
        response.setHeader('Access-Control-Allow-Headers', 'Content-Type');\n\
        \n\
        if (method === 'GET') { // 界面端获取当前棋盘局势图\n\
            response.writeHead(200, { 'Content-Type': 'text/plain' });\n\
            response.end(getBoardCode() + '\\n')\n\
        }\n\
        else if (method === 'PUT') { // 人机方面做出决策更改服务器棋盘局势图\n\
            response.writeHead(200, { 'Content-Type': 'text/plain' });\n\
            response.end('successful\\n')\n\
            boardCode = request.url.split('=')[1];\n\
        }\n\
        else if (method == 'POST') { // 玩家着法\n\
            response.writeHead(200, { 'Content-Type': 'text/plain' });\n\
            response.end('successful\\n')\n\
            let move = request.url.split('=')[1];\n\
            let file = fs.openSync('./_move_.txt', 'w+')\n\
            fs.writeFileSync(file, move)\n\
            fs.closeSync(file)\n\
        }\n\
    }).listen(9494)\n\
    \n\
    console.log('Server running at http://127.0.0.1:9494/')\n\
    ";
void setBoardCode(Board board)
{
    BOARD_CODE code = generateCode(board);

    const BOARD_CODE jsPutCode =
        "\
        const http = require('http')\n\
        const options = {\n\
            hostname: '127.0.0.1',\n\
            path : '/?boardcode=" +
        code + "',\n\
            port : 9494,\n\
            method : 'PUT'\n\
        }\n\
        http.request(options).end();\n\
    ";

    FILE *file = nullptr;
    errno_t result = fopen_s(&file, "./_put_.js", "w+");
    if (result == 0)
    {
        fprintf(file, jsPutCode.c_str());
        std::fclose(file);
        system("node _put_.js");
    }
    else
    {
        std::cerr << "CANNOT OPEN FILE!" << std::endl;
    }
}

void serverInit(Board board)
{
    FILE *file = nullptr;
    errno_t result = fopen_s(&file, "./_server_.js", "w+");
    if (result == 0)
    {
        fprintf(file, jsServerCode.c_str());
        std::fclose(file);
        system("start node _server_.js");
        Sleep(700);
        setBoardCode(board);
    }
}