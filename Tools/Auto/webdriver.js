/////////////////////////////
const OPPONENT_LEVEL = 9
const CPPFILE_RELATIVE_PATH_DIRECTORY = "../../Chess98/"
/////////////////////////////

const { Builder, By, Key, until, WebDriver } = require("selenium-webdriver")
const edge = require("selenium-webdriver/edge")
const http = require("http")
const { exec } = require("child_process")
const { get } = require("selenium-webdriver/http")

let chess98LastMove = "null"
let webLastBoard = [
    [1, 1, 1, 1, 1, 1, 1, 1, 1],
    [0, 0, 0, 0, 0, 0, 0, 0, 0],
    [0, 1, 0, 0, 0, 0, 0, 1, 0],
    [1, 0, 1, 0, 1, 0, 1, 0, 1],
    [0, 0, 0, 0, 0, 0, 0, 0, 0],
    [0, 0, 0, 0, 0, 0, 0, 0, 0],
    [1, 0, 1, 0, 1, 0, 1, 0, 1],
    [0, 1, 0, 0, 0, 0, 0, 1, 0],
    [0, 0, 0, 0, 0, 0, 0, 0, 0],
    [1, 1, 1, 1, 1, 1, 1, 1, 1],
]
let state = 0

async function isEndGame(driver) {
    const elements = await driver.findElements(By.css(".game-end-widget"))
    if (elements.length > 0) {
        console.log("游戏结束")
        while (true);
    }
}

// 获取Chess98的走法
async function getChess98LastMove(driver) {
    isEndGame(driver)
    http.get("http://localhost:9494/computer", async (res) => {
        let data = ""
        res.on("data", (chunk) => {
            data += chunk
        })
        res.on("end", async () => {
            data = data.padStart(5, "0").slice(0, 4)
            if (data !== chess98LastMove) {
                console.log("我方步进成功")
                chess98LastMove = data

                await doMoveOnWeb(driver)
                chess98LastMove = data

                const wait = async () => {
                    const currentBoard = await getWebBoard(driver)
                    if (currentBoard.toString() != webLastBoard.toString()) {
                        return
                    }
                    else {
                        await driver.sleep(200)
                        await wait()
                    }
                }
                await wait()

                // 分析两次棋盘的差异
                const currentBoard = await getWebBoard(driver)
                const lastBoard = webLastBoard
                const move = await getWebMove(driver, lastBoard, currentBoard)
                console.log("对方步进成功")

                webLastBoard = currentBoard
                // 发送
                await updateXiangqiaiChangeToChess98UI(driver, move)

                state++
                console.log("==========================")
            }
            else {
                await driver.sleep(200)
                await getChess98LastMove(driver)
            }
        })
    })
}

// 在网页上执行步进
async function doMoveOnWeb(driver) {
    const x1 = Number(chess98LastMove.charAt(1))
    const y1 = Number(chess98LastMove.charAt(0))
    const x2 = Number(chess98LastMove.charAt(3))
    const y2 = Number(chess98LastMove.charAt(2))
    const _x1 = 10 - x1
    const _y1 = y1 + 1
    const _x2 = 10 - x2
    const _y2 = y2 + 1
    console.error("步进坐标", _x1, _y1, _x2, _y2)

    webLastBoard[9 - x1][y1] = 0
    webLastBoard[9 - x2][y2] = 1

    const actions = driver.actions()

    let start, end
    start = await driver.findElement(By.css(`#game-grid > div:nth-child(${_x1}) > div:nth-child(${_y1}) > div`))
    end = await driver.findElement(By.css(`#game-grid > div:nth-child(${_x2}) > div:nth-child(${_y2}) > div`))
    if (_x1 === 1)
        start = await driver.findElement(By.css(`#game-grid > div:nth-child(${_x1}) > div:nth-child(${_y1}) > div:last-child`))
    if (_x2 === 1)
        end = await driver.findElement(By.css(`#game-grid > div:nth-child(${_x2}) > div:nth-child(${_y2}) > div:last-child`))
    if (_x1 === 10)
        start = await driver.findElement(By.css(`#game-grid > div:nth-child(${_x1}) > div:nth-child(${_y1}) > div:first-child`))
    if (_x2 === 10)
        end = await driver.findElement(By.css(`#game-grid > div:nth-child(${_x2}) > div:nth-child(${_y2}) > div:first-child`))

    // 高亮
    await driver.executeScript(
        "arguments[0].style.border='3px solid red';",
        start
    )
    await driver.executeScript(
        "arguments[0].style.border='3px solid red';",
        end
    )

    const startRect = await start.getRect()
    const endRect = await end.getRect()
    const startX = Math.ceil(startRect.x + startRect.width / 2)
    const startY = Math.ceil(startRect.y + startRect.height / 2)
    const endX = Math.ceil(endRect.x + endRect.width / 2)
    const endY = Math.ceil(endRect.y + endRect.height / 2)
    await driver.actions({ bridge: true })
        .move({ x: startX, y: startY })
        .click()
        .move({ x: endX, y: endY, duration: 300 })
        .click()
        .perform()

    // 移除高亮
    await driver.executeScript(
        "arguments[0].style.border='';",
        start
    )
    await driver.executeScript(
        "arguments[0].style.border='';",
        end
    )

    if ((await getWebBoard(driver)).toString() != webLastBoard.toString()) {
        webLastBoard[9 - x1][y1] = 1
        webLastBoard[9 - x2][y2] = 0
        console.error("步进失败，尝试重新步进")
        await driver.sleep(400)
        await doMoveOnWeb(driver)
    }
}

// 获取网页的象棋棋盘
async function getWebBoard(driver) {
    let currentBoard = []
    for (let i = 1; i <= 10; i++) {
        for (let j = 1; j <= 9; j++) {
            currentBoard[i - 1] = currentBoard[i - 1] || []
            try {
                const square = await driver.findElement(By.css(`#game-grid > div:nth-child(${i}) > div:nth-child(${j}) > div.square-has-piece`))
                currentBoard[i - 1][j - 1] = 1
            } catch (error) {
                currentBoard[i - 1][j - 1] = 0
            }
        }
    }
    return currentBoard
}

// 获取网页的走法（坐标0开头）
async function getWebMove(driver, lastBoard, currentBoard) {
    let move = { x1: -1, y1: -1, x2: -1, y2: -1 }

    // moveFrom
    // 若原来是1，现是0，则为起点
    for (let i = 0; i < lastBoard.length; i++) {
        for (let j = 0; j < lastBoard[i].length; j++) {
            if (lastBoard[i][j] === 1 && currentBoard[i][j] === 0) {
                move.x1 = i
                move.y1 = j
            }
        }
    }
    // moveTo
    // 获取网页上的移动过的棋子的位置
    const elements = await driver.findElements(By.css(".pieces-container > div"))
    let moved = { x: -1, index: -1 } // index: 这一行的第几个棋子
    let pieces = []
    for (let el of elements) {
        const elChild = await el.findElement(By.css(".pieces-container > div > div > div"))
        const rowNum = 11 - (await el.getAttribute("r"))
        pieces[rowNum] = pieces[rowNum] || []
        pieces[rowNum].push(el)
        if ((await elChild.getAttribute("class")).match("moved-piece")) {
            moved.x = rowNum
            moved.index = pieces[rowNum].length - 1
        }
    }
    // 用moved.x和moved.index获取棋子在棋盘上的位置
    let count = 0
    move.x2 = moved.x - 1
    for (let k in currentBoard[moved.x - 1]) {
        if (currentBoard[moved.x - 1][k] == 1) {
            count++
            if (count == moved.index + 1) {
                move.y2 = Number(k)
                break
            }
        }
    }
    return move
}

// 发送走法到Chess98UI Server
async function updateXiangqiaiChangeToChess98UI(driver, move) {
    const x1 = String(9 - move.x1)
    const y1 = String(move.y1)
    const x2 = String(9 - move.x2)
    const y2 = String(move.y2)
    const moveString = y1 + x1 + y2 + x2
    if (moveString.match(/-/g)) {
        console.error("步进失败，坐标不合法", moveString)
        return await getChess98LastMove(driver)
    }
    console.log("着法被发送至服务器", moveString)
    http.request({
        hostname: "127.0.0.1",
        path: "/move?playermove=" + moveString,
        port: 9494,
        method: "GET"
    }).end()
    await driver.sleep(300)
}

// 初始化webdriver
async function init() {
    const options = new edge.Options()

    options.addArguments(
        `--user-data-dir=C:\\Users\\Yeshui\\AppData\\Local\\Microsoft\\Edge\\User Data`,
        `--profile-directory=Default`,
        `--log-level=3`
    )
    const driver = await new Builder()
        .forBrowser("MicrosoftEdge")
        .setEdgeOptions(options)
        .build()

    return driver
}

// 打印棋盘
function printBoard(board) {
    for (let v of board) { console.log(v.toString()) }
    console.log("====================================")
}

async function run() {
    exec(`taskkill /F /IM msedge.exe`, async () => {
        console.log("开始执行")
        const driver = await init()

        try {
            await driver.get("https://play.xiangqi.com/")
            const playComputer = await driver.findElement(By.css("div.btn-list > div:nth-child(2)"))
            await playComputer.click()

            const bot = await driver.findElement(By.css(`.all-bots :nth-child(${OPPONENT_LEVEL})`))
            await bot.click()

            const playButton = await driver.findElement(By.css(".button-wrapper button:nth-child(1)"))
            await playButton.click()
        } catch (error) {
            console.error("无法访问网站")
            driver.quit()
            return
        }
        const wait = async () => {
            try {
                const element = await driver.findElement(By.css(".body"))
                if (element) {
                    await driver.sleep(200)
                    await wait()
                }
            } catch (error) {
                return
            }
        }
        await wait()

        await driver.sleep(500)

        while (true) {
            await getChess98LastMove(driver)
            const currentState = state
            const wait = async () => {
                if (state == currentState) {
                    await driver.sleep(200)
                    await wait()
                }
                else {
                    return
                }
            }
            await wait()
        }
    })
}

run()
