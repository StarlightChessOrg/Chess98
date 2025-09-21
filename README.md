# 车九平八

一个借鉴了象眼引擎的中国象棋 AI, 正在持续开发中。

## 使用

环境要求：
- 操作系统：Windows, Linux (MacOS 暂未测试)
- 依赖：Node.js (要求终端能够使用node命令, 以启用UI)
- 编译器：目前 g++, MSVC 都能过编译

如果想要下棋, 请确保 `Chess98/nnuefile.hpp` 中没有 `#define GENFILES`, 否则会运行跑谱器

确保 localhost:9494 没有被占用, 这是 UI 的服务器端口。

编译 Chess98/main.cpp 后直接运行输出文件, 然后在 `tools/ui/ui.html` 下棋

## 开发

Chess98 项目的目录是这样的：

- Chess98 项目的源代码文件夹
- nnue 存放 nnue 相关开发内容
    - data 存放跑谱器生成的 json
- tools 存放项目相关的一些工具
    - auto 自动测试
    - chess_matrix_convert 象眼格式转换, 做开局库的时候用到
    - openbook 开局库文件
    - ui 界面
- .gitignore Git忽略文件
- LICENSE 许可证
- README.md 介绍文件
- Chess98.sln Visual Studio 项目文件
- CMakeLists.txt CLion 项目文件

### 使用自动测试工具

环境要求：
- 操作系统：Windows (Linux, MacOS 暂未测试)
- 依赖：Node.js, Selenium-Webdriver, Microsoft Edge, Edge Webdriver

一个基于 https://play.xiangqi.com/ 使用 selenium 编写的自动测试工具,

如果出现以下报错：

```
Error: Unable to obtain browser driver.
```

请到 https://developer.microsoft.com/en-us/microsoft-edge/tools/webdriver/ 下载 Webdriver, 然后添加到path

1. 先运行一个 chess98 的 exe 实例
2. cd 到 `tools/Auto/`, 若第一次使用则先运行 `npm i selenium-webdriver`, 下载 webdriver
3. 终端运行 `node z`, 开始自动测试

可以在 `tools/auto/z.js` 的文件头部更改对方人机等级

### 跑谱器

在 `Chess98/nnuefile.hpp` 内加上 `#define GENFILES` 启用跑谱器, 输出 json 文件到指定目录下

配置可以在 nnuefile.hpp 的开头调整几个常量

json 内容结构如下：

```
[
    {
        fen(string),
        history: [...(int)],
        data: [
            {
                depth(int),
                data: [
                    {
                        moveid(int),
                        fen_after_move(string),
                        vl(int)
                    },
                    ...
                ]
            },
            ...
        ]
    },
    ...
]
```

### NNUE

持续开发中
