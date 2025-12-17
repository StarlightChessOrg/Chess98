# Chess98 DEV

Chess98的仓库内附带一些开发组件，便于测试。

## 自动测试工具

一个基于 https://play.xiangqi.com/ 使用 selenium 编写的自动测试工具。环境要求：

- Windows
- Node.js
- Selenium-Webdriver

请先到 https://developer.microsoft.com/en-us/microsoft-edge/tools/webdriver/ 下载 Webdriver, 添加到path，之后按以下步骤操作

1. 先运行一个 chess98 的 exe 实例
2. cd 到 `tools/Auto/`, 若第一次使用则先运行 `npm i selenium-webdriver`, 下载 webdriver
3. 终端运行 `node z`, 开始自动测试

可以在 `tools/auto/z.js` 的文件头部更改对方人机等级

### 跑谱器

生成NNUE训练数据的工具。在 `Chess98/nnuefile.hpp` 内加上 `#define GENFILES` 启用跑谱器, 输出 json 文件到指定目录下。

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

