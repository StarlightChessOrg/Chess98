---
title: 车炮着法生成
author: 夜水
lang: zh
date: 2026/3/8
---

车炮着法生成这一块对于性能提升比较重要一些，这里写思路。

首先，定义`PREGEN_DATA`为`unsigned char`，其中前4位为左（上）边吃子，后四位为右（下）边吃子。

生成车、炮的预生成吃子着法，定义`ROOK_PREGEN`为一个1024 * 10 * 1byte大小的数组，定义`CANNON_PREGEN`为同样的数组。

生成车的不吃子着法，定义`LINEAR_NONCAPTURE`为一个1024 * 10 * 1byte大小的数组，里面存着索引。

定义预生成的索引位置为一个`vector<vector<>>`形状的容器，名为`NONCAPTURE_MOVES`，访问方式为[pos_index][capture_pos]。

车、炮生成吃子着法就直接用对应的预生成表。生成非吃子着法则直接用车的capture token访问moves就行了。

这样算法复杂度基本就是O(1)级别的了，生成速度极端情况下能快50%左右吧~
