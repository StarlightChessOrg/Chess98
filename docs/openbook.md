# openbook

这篇文章提供`openbook.py`的功能以及实现细节等。

## 功能

开局库是每个象棋引擎的必备工具，因此我想要通过皮卡鱼蒸馏出一个开局库
使用`SQLite3`存储开局库文件，跟随主流选择，且更加通用、高效
对于常见走法分支有更深的记录，对于其它分支也有一定的掌握
开局库训练实现断点续练、多线程训练、查看db开局信息等内容
并且自动处理镜像局面问题防止文件体积爆炸

## 实现细节

模块路径为`<PROJECT>/kits/openbook.py`，作为命令行工具用`python -m kits.openbook`使用

有以下常量：

- OPENBOOK_PATH: 放在`__pycache__/openbook.db`下
- MAX_DEPTH: 皮卡鱼的搜索深度，默认为13
- INIT_FEN: rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RNBAKABNR w
- PRUNING_BEG_PLY: 在对局进行到这个ply之前，开局库进行完全搜索；这个ply之后，开局库只进行剪枝搜索，防止文件体积暴涨，默认为2
- BEST_MOVES_NUM: 剪枝搜索需要搜索排名前n个的bestmove，默认为4
- SCORE_PRUNE_CP: 评估分剪枝阈值（单位与皮卡鱼 `score cp` 一致），默认`200`。仅在`ply >= PRUNING_BEG_PLY`时生效：若该局面评估分`< -SCORE_PRUNE_CP`或`> SCORE_PRUNE_CP`（或为杀棋分），则不再向下扩展子节点，只保留本局面检索结果，避免劣势/优势一边倒分支继续指数膨胀

实现一个命令行交互功能，以下参数：

- `python -m kits.openbook` 列出可用的命令
- `python -m kits.openbook --train <depth>` 开始训练或断点续练多少层深度的开局库
- `python -m kits.openbook --fen <fen_str>` 查看局面是否在开局库里面
- `python -m kits.openbook --info` 查看已经训练的开局库的信息，如局面数量，深度->局面数量映射

暴露以上命令行接口即可，内部实现由AI自己决定，大致流程如下

调用`Pikafish.set_max_depth`设置最大深度（这个过程若pikafish不存在则pikafish.py会自动下载）

之后pikafish默认局面就是INIT_FEN

然后遍历所有可行的着法，开约`物理核心数 - 2`个进程同时运行皮卡鱼的不同着法分支
（`os.cpu_count()` 多为逻辑处理器，实现里按 `cpu_count // 2 - 2` 估算，避免超线程把进程开爆）

搜索达到`PRUNING_BEG_PLY`之后就不进行完全搜索了，只进行`BEST_MOVES_NUM`的剪枝搜索；并且若局面评估超出`±SCORE_PRUNE_CP`则直接剪掉后续分支

最后达到命令行的depth深度就可以停止了

要求有命令行信息提示，进度条等功能，支持断点续练，输出着法时转换成中文的中国象棋着法规则

### 关于 SQLite 体积

当前实现未开启 SQLite 压缩：标准 SQLite 内核本身没有透明行压缩；本模块只用了`PRAGMA journal_mode=WAL`与`synchronous=NORMAL`（写性能/耐久折中），没有启用`sqlite-compression`等扩展，也没有定期`VACUUM`压碎片。控制体积主要靠镜像归并、MultiPV 剪枝与`SCORE_PRUNE_CP`评估剪枝。
