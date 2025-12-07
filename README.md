# 车九平八

一个中国象棋项目，借鉴了象眼等引擎，正在持续开发中。

## 使用

通过项目的 GitHub Releases 包含的 exe 文件以及 ui 界面使用发行版。截至目前（2025/12/7）还没有支持 UCCI 界面

## 开发

Chess98 已经实现的：

- search:
    - pvs
    - searchq
    - ndp
    - mdp
    - fp
    - multiProbcut
    - openbook
    - bitboard
- heuristic
    - transportation
    - killer
    - history
- evaluate
    - classical vl matrix evaluation

### 自动测试工具

环境要求：
- 操作系统：Windows
- 依赖：Node.js, Selenium-Webdriver, Edge Webdriver

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

神经网络优化评估的技术，正在开发中
