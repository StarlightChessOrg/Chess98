#pragma once
#include "board.hpp"
#include "search.hpp"

using BOARD_CODE = std::string;

FILE *openFile(const std::string &filename, const char *mode, int retryCount = 0)
{
    FILE *file = nullptr;
    errno_t result = fopen_s(&file, filename.c_str(), mode);
    if (retryCount >= 5)
    {
        std::cerr << "Failed to open file: " << filename << std::endl;
        std::cerr << "UI cannot run, please check the file path and permissions." << std::endl;
        system("pause");
        throw std::runtime_error("Failed to open file after multiple attempts.");
    }
    if (result != 0)
    {
        wait(50);
        return openFile(filename, mode, retryCount + 1);
    }
    return file;
}

std::string readFile(const std::string &filename)
{
    FILE *file = openFile(filename, "r");
    if (!file)
    {
        return "";
    }

    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);

    std::string content(length, '\0');
    fread(&content[0], 1, length, file);
    fclose(file);
    return content;
}

void writeFile(const std::string &filename, const std::string &content)
{
    FILE *file = openFile(filename, "w+");
    if (!file)
    {
        return;
    }

    fwrite(content.c_str(), 1, content.size(), file);
    fclose(file);
}

BOARD_CODE generateCode(Board board)
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

void setBoardCode(const Board &board)
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

    wait(200);
    writeFile("./_put_.js", jsPutCode);
    system("node ./_put_.js");
}

void ui(std::string serverDir, TEAM team, int maxDepth, int maxTime, std::string fenCode)
{
    // 初始局面
    PIECEID_MAP pieceidMap = fenToPieceidMap(fenCode);

    // variables
    int count = 0;
    Search s = Search(pieceidMap, team);
    printPieceidMap(s.board.pieceidMap);

    // 界面
    std::string cmd = "powershell.exe -command \"& {Start-Process -WindowStyle hidden node " + serverDir + "}\"";
    system(cmd.c_str());
    setBoardCode(s.board);
    std::string moveFileContent = "____";
    while (true)
    {
        if (s.board.team == team)
        {
            count++;
            std::cout << count << "---------------------" << std::endl;

            // 人机做出决策
            Result node = s.searchMain(maxDepth, maxTime);
            s.board.doMove(node.move);
            if (inCheck(s.board) == true)
                s.board.historyMoves.back().isCheckingMove = true;

            setBoardCode(s.board);
            moveFileContent = readFile("./_move_.txt");
        }
        else
        {
            // 读取文件
            std::string content = readFile("./_move_.txt");

            // 悔棋
            if (content == "undo" && s.board.historyMoves.size() > 1)
            {
                count--;
                std::cout << "undo" << std::endl;
                s.board.undoMove();
                s.board.undoMove();

                setBoardCode(s.board);
                writeFile("./_move_.txt", "wait");
                moveFileContent = "wait";
            }

            // 如果内容和上次内容不一致，则执行步进
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
                    s.board.doMove(move);
                }
                catch (std::exception &e)
                {
                    // 避免转换失败导致崩溃
                    std::cerr << "Invalid move: " << moveFileContent << std::endl;
                    system("pause");
                    throw e;
                }
            }
        }
        wait(50);
    }
}
