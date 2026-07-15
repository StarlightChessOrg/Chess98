# pikafish

这篇文章提供`pikafish.py`的功能以及实现细节等

## 功能

皮卡鱼是目前中国象棋第一开源引擎，因此当前项目的开局库数据训练、自动化测试等
都需要用到皮卡鱼引擎作为参照对象，来获得更加优秀的开局库和测试准确度一类的东西

## 实现细节

模块路径为`<PROJECT>/kits/pikafish.py`，仅作为模块被导入，`__main__`直接退出

各种常量如下：

- PIKAFISH_URL: https://github.com/official-pikafish/Pikafish/releases/download/Pikafish-2026-01-02/Pikafish.2026-01-02.7z
- ZIP_PATH: 放在`__pycache__/pikafish.7z`
- EXTRACT_PATH: 放在`__pycache__/pikafish/`
- EXE_PATH: 验证完对于本机CPU合适的皮卡鱼程序之后，放在`__pycache__/pikafish.exe`

对外暴露以下方法：

- Pikafish.set_max_depth(max_depth: int) -> success_or_not: bool
- Pikafish.set_fen(fen: str) -> success_or_not: bool
- Pikafish.search(fen: str) -> result: tuple(move: str, vl: str)

内部实现由AI自己决定，大概流程是这样：

运行以上暴露的任意一个方法时，先校验本地有没有皮卡鱼文件，如果没有，就从PIKAFISH_URL下载

下载失败重新下载，最多再重复2次就报错，终止所有程序

下载成功就存到ZIP_PATH里面，解压放到EXTRACT_PATH里面，挑选出来适合当前CPU的最快的版本，放EXE_PATH中

完成后才能执行所运行的方法

要求有命令行信息提示、进度条等功能
