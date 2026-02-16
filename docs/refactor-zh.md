---
title: 中象引擎项目重构文档
author: 夜水
lang: zh
date: 2026/2/16
---

最近准备把我的象棋引擎重构一遍，重新设计并加入一些新的算法，提升搜索性能和评估准确度，为此翻阅了一些论文，参考了AI给出的设计，写下本文。

其实我之前的架构并没有那么差，除了棋盘部分的设计之外甚至还比较优秀，没有照抄论文实现，总体来看还算可以，但是很多地方还是非常屎山。各种功能全都堆棋盘类里，导致本应轻量的棋盘极为臃肿；单例的对象还要学Java给装到类里，致使结构复杂。因此重新设计是相当有必要的。

## 设计理念

网上的很多代码教程都是二十年前的了，那个年代基础设施还没有长足发展，因此，主流实现往往是混合各种复杂的奇技淫巧，比如256棋盘数组、位操作加速、魔法数等。

好处是当今最多有10%的性能提升，经过简单的测试；代价则是可读性几乎完全被牺牲，导致一般个人几乎没有精力去维护，得不偿失。这些东西在现代的设施下基本上是没有什么必要的。现代的编译器和CPU已经足够强大，只要你的代码意图清晰、缓存友好，运行速度甚至能够超过那些复杂的实现。

如今，可维护性才是第一真神。

## 架构计划

新设计准备提高可维护性，去除之前的一些冗余设计。

文件依赖树将采用新的架构，尽量避免陷入循环依赖问题，大致如下：

```plaintext
核心层
base.hpp
  存储公用的全局常量
hash.hpp 依赖 base.hpp
  存储预生成的局面哈希，并提供接口获取哈希。
heuristic.hpp 依赖 base.hpp
  实现启发算法，只缓存，不负责合理性检验
position.hpp 依赖 hash.hpp
  实现局面记录
movepicker.hpp 依赖 heristic.hpp position.hpp
  实现着法生成，负责合理性和顺序验证
evaluate.hpp 依赖 position.hpp
  实现评估算法
search.hpp 依赖 movepicker.hpp evaluate.hpp
  实现搜索算法

边缘层
utils.hpp 依赖 base.hpp
  实现各种非核心的方法
ucci.hpp 依赖 search.hpp
  实现ucci协议
test.hpp 依赖 search.hpp
  实现test测试，若0.3s内没有接收ucci输入则启用
main.cpp 依赖 ucci.hpp test.hpp
  实现主进程
```

核心依赖树如下：

```plaintext
        base.hpp
       /        \
    hash.hpp heuristic.hpp
     |          /
 position.hpp -------+
     |        /      |
movepicker.hpp    evaluate.hpp
         \          /
          search.hpp
```

## 具体方案

在`base.hpp`内，加入棋子id，队伍id，各种define等。

- 定义POS位置类型为char，范围0~89。
- 定义PID棋子id类型为char，范围-7~7，直接-pid就可以表示对方队伍的棋子。
- 定义具体PID，命名如下：
  - R_KING, R_ADVISOR, R_BISHOP, ..., R_PAWN
  - B_KING, B_ADVISOR, B_BISHOP, ..., B_PAWN
- 定义PINDEX棋子唯一编号类型为char，范围0~31，用于精确地指示棋子。
- 定义TEAM队伍id类型为char，范围-1~1。
- 定义具体TEAM，命名为R和B。
- 定义MATRIX棋盘矩阵类型为`std::array<char, 90>`，0~8为黑方的最底列，即有车的一列。
- 定义FEN类型为std::string。
- 定义DEPTH类型为unsigned char，定义VL类型为short。
- 定义INF, BAN分别为30000和20000，INF到short边界去没有什么必要性。
- 定义MOVES, PIECES为各自类型的std::vector。
- 棋子id按照将、士、象、马、车、炮、兵从一到七，红方为正数，黑方为负数，空位为0，溢出棋盘也为0。
- 队伍id按照红1黑-1设计，空位为0，溢出棋盘也为0。
- Move结构包含beg，end，等号运算符重载，占2字节。
- Piece结构包含pid，pindex，pos，live，占4字节。
- 定义SEARCH_RET为`std::pair<Move, short>`，存储搜索结果。
- 定义TRICK_RET为`std::pair<bool, short>`，存储mdp和probCut等技巧的结果，便于封装。

在`utils.hpp`内加入各种杂项功能，放在命名空间`utils`中。

- `fen_to_matrix(FEN fen)->MATRIX` 棋盘fen码转矩阵。
- `matrix_to_fen(const MATRIX& matrix)->FEN` 矩阵转棋盘fen码。
- `read_file(std::string name)->std::string` 流操作读取整个文件。
- `write_file(std::string name, std::string` content)->void 流操作w模式写文件。
- `gen_random_fen(int num)->std::vector<FEN>` 快速生成伪随机不重复的合法fen局面。

在`hash.hpp`内加入祖传的哈希矩阵，封装相关功能。

- 在匿名namespace中放入R_KING_KEY，R_KING_LOCK等祖传代码。
- 整理为KEYS和LOCKS数组，能使用`xx[pid + 7][pos]`访问。
- 在匿名namespace中放入变量：
  - `int key` 当前局面的key值，由当前组件管理。
  - `int lock` 当前局面的lock值。
  - `std::vector<int> key_history`
  - `std::vector<int> lock_history`
- 在namespace hash内加入以下内容：
  - `init(const MATRIX& matrix)->void` 初始化，算法如下：
    - 设key和lock为0。
    - 遍历matrix，若v不为0则每次都`key ^= KEYS[v + 7]`，lock同理。
  - `update(const MATRIX& matrix, Move move, PID captured)->void` 更新步进
  - `undo_update()->void` 撤销更新

在`movepicker.hpp`内加入各类着法生成函数，且采用新的设计。

- 在匿名namespace里放入棋子的预生成着法，编译时直接计算，具体策略如下：
  - TODO: 这个算法较为复杂，我还在考虑。
- 不应当一次性生成所有着法，

在`heuristic.hpp`内加入启发函数，各自有各自的匿名namespace和封装的接口。

- 历史启发
- 杀手启发
- 置换表启发
- MVV/LVA
- SEE


