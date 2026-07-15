"""自动对弈测试：Chess98（或皮卡鱼替身）vs 皮卡鱼。"""

from __future__ import annotations

import argparse
import os
import queue
import re
import shutil
import subprocess
import sys
import threading
import time
from concurrent.futures import ProcessPoolExecutor, as_completed
from dataclasses import dataclass, field
from datetime import datetime
from enum import Enum
from pathlib import Path

from kits.openbook import (
    INIT_FEN,
    Position,
    format_move,
    generate_legal_moves,
    make_move,
)
from kits.pikafish import EXE_PATH as PIKAFISH_EXE
from kits.pikafish import ensure_engine

# ---------------------------------------------------------------------------
# 常量 / 路径
# ---------------------------------------------------------------------------

_PROJECT_ROOT = Path(__file__).resolve().parent.parent
_CACHE_DIR = Path(__file__).resolve().parent / "__pycache__"
CHESS98_EXE = _CACHE_DIR / "chess98.exe"
MAIN_CPP = _PROJECT_ROOT / "src" / "main.cpp"
README_PATH = _PROJECT_ROOT / "README.md"
LOG_PATH = _CACHE_DIR / "autotest.log"
GAMES_DIR = _CACHE_DIR / "autotest_games"

MAX_PLY = 400
# 连续无吃子半手数达到此值判和（计算机象棋常用“自然限着”）
NO_CAPTURE_DRAW_PLY = 120
REFACTOR_MARKER = "This project is being refactored now..."
DEFAULT_GAMES = 100
DEFAULT_DEPTH = 10

_SCORE_CP_RE = re.compile(r"\bscore\s+cp\s+(-?\d+)\b")
_SCORE_MATE_RE = re.compile(r"\bscore\s+mate\s+(-?\d+)\b")


class GameResult(str, Enum):
    WIN = "win"  # 己方胜
    LOSS = "loss"  # 己方负
    DRAW = "draw"
    ILLEGAL = "illegal"  # 己方违规着法
    TOO_LONG = "too_long"  # 超过长度限制
    CRASH = "crash"  # 己方崩溃


# ---------------------------------------------------------------------------
# 终端
# ---------------------------------------------------------------------------


def _ensure_utf8_stdio() -> None:
    for name in ("stdout", "stderr"):
        stream = getattr(sys, name, None)
        if stream is None:
            continue
        try:
            stream.reconfigure(encoding="utf-8", errors="replace")
        except Exception:
            pass


_ensure_utf8_stdio()


def _log(message: str) -> None:
    try:
        print(message, flush=True)
    except UnicodeEncodeError:
        sys.stdout.buffer.write((message + "\n").encode("utf-8", errors="replace"))
        sys.stdout.buffer.flush()


def _worker_count() -> int:
    logical = os.cpu_count() or 2
    physical = max(1, logical // 2)
    return max(1, physical - 2)


# ---------------------------------------------------------------------------
# 编译 Chess98
# ---------------------------------------------------------------------------


def is_refactoring() -> bool:
    if not README_PATH.is_file():
        return False
    text = README_PATH.read_text(encoding="utf-8", errors="replace")
    return REFACTOR_MARKER in text


@dataclass
class Compiler:
    name: str
    path: str


def detect_compiler() -> Compiler | None:
    for name in ("cl", "g++", "clang++"):
        path = shutil.which(name)
        if path:
            return Compiler(name=name, path=path)
    # VS Developer Prompt 外尝试 vswhere → cl
    vswhere = Path(os.environ.get("ProgramFiles(x86)", r"C:\Program Files (x86)")) / (
        "Microsoft Visual Studio/Installer/vswhere.exe"
    )
    if vswhere.is_file():
        try:
            out = subprocess.check_output(
                [
                    str(vswhere),
                    "-latest",
                    "-products",
                    "*",
                    "-requires",
                    "Microsoft.VisualStudio.Component.VC.Tools.x86.x64",
                    "-find",
                    "**/Hostx64/x64/cl.exe",
                ],
                text=True,
                timeout=30,
            ).strip()
            if out:
                cl = out.splitlines()[0].strip()
                if Path(cl).is_file():
                    return Compiler(name="cl", path=cl)
        except Exception:
            pass
    return None


def compile_chess98(out_exe: Path = CHESS98_EXE) -> Path:
    """以最高常见优化等级编译 src/main.cpp → out_exe。"""
    if not MAIN_CPP.is_file():
        raise FileNotFoundError(f"找不到源文件：{MAIN_CPP}")

    compiler = detect_compiler()
    if compiler is None:
        raise RuntimeError(
            "未检测到 C++ 编译器（需要 cl / g++ / clang++）。"
            "请安装 Visual Studio、MinGW 或 LLVM 并加入 PATH。"
        )

    out_exe.parent.mkdir(parents=True, exist_ok=True)
    src_dir = MAIN_CPP.parent
    _log(f"使用编译器：{compiler.name} ({compiler.path})")
    _log(f"编译 {MAIN_CPP.name} → {out_exe} …")

    if compiler.name == "cl":
        cmd = [
            compiler.path,
            "/nologo",
            "/O2",
            "/EHsc",
            "/std:c++17",
            f"/I{src_dir}",
            f"/Fe:{out_exe}",
            str(MAIN_CPP),
        ]
        # MSVC 会生成 .obj 在 cwd
        cwd = str(_CACHE_DIR)
        _CACHE_DIR.mkdir(parents=True, exist_ok=True)
    else:
        cmd = [
            compiler.path,
            "-O3",
            "-DNDEBUG",
            "-std=c++17",
            f"-I{src_dir}",
            "-o",
            str(out_exe),
            str(MAIN_CPP),
        ]
        cwd = str(_PROJECT_ROOT)

    proc = subprocess.run(
        cmd,
        cwd=cwd,
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
    )
    if proc.returncode != 0 or not out_exe.is_file():
        detail = (proc.stdout or "") + "\n" + (proc.stderr or "")
        raise RuntimeError(f"编译失败（exit={proc.returncode}）：\n{detail}")

    _log(f"编译成功：{out_exe}")
    return out_exe


# ---------------------------------------------------------------------------
# 通用 UCI 引擎进程
# ---------------------------------------------------------------------------


def _creationflags() -> int:
    if sys.platform == "win32":
        return subprocess.CREATE_NO_WINDOW  # type: ignore[attr-defined]
    return 0


_BESTMOVE_RE = re.compile(
    r"^bestmove\s+(\S+)(?:\s+ponder\s+(\S+))?", re.IGNORECASE
)


class UciEngine:
    """任意 UCI 可执行文件的轻量封装。"""

    def __init__(self, exe: Path, depth: int = DEFAULT_DEPTH) -> None:
        self.exe = Path(exe)
        self.depth = depth
        self._proc: subprocess.Popen[str] | None = None
        self._queue: queue.Queue[str | None] = queue.Queue()
        self._lock = threading.Lock()
        self._alive = True

    def start(self) -> None:
        if self._proc is not None and self._proc.poll() is None:
            return
        if not self.exe.is_file():
            raise FileNotFoundError(f"引擎不存在：{self.exe}")

        self._proc = subprocess.Popen(
            [str(self.exe)],
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.DEVNULL,
            text=True,
            bufsize=1,
            cwd=str(self.exe.parent),
            creationflags=_creationflags(),
        )
        self._queue = queue.Queue()
        self._alive = True
        threading.Thread(target=self._reader, daemon=True).start()

        self.send("uci")
        self._read_until(lambda line: line.strip() == "uciok", timeout=15.0)
        # 多进程下必须单线程，否则互相抢核、局面漫长且不稳定
        self.send("setoption name Threads value 1")
        self.send("setoption name Hash value 64")
        self.send("setoption name MultiPV value 1")
        self.send("ucinewgame")
        self.send("isready")
        self._read_until(lambda line: line.strip() == "readyok", timeout=60.0)

    def _reader(self) -> None:
        proc = self._proc
        if proc is None or proc.stdout is None:
            self._queue.put(None)
            return
        try:
            while True:
                line = proc.stdout.readline()
                if line == "":
                    self._queue.put(None)
                    break
                self._queue.put(line.rstrip("\r\n"))
        except Exception:
            self._queue.put(None)

    def send(self, command: str) -> None:
        self.start()
        assert self._proc is not None and self._proc.stdin is not None
        if self._proc.poll() is not None:
            self._alive = False
            raise RuntimeError("引擎进程已退出")
        with self._lock:
            self._proc.stdin.write(command.rstrip("\n") + "\n")
            self._proc.stdin.flush()

    def _read_line(self, timeout: float | None) -> str | None:
        try:
            if timeout is None:
                return self._queue.get()
            return self._queue.get(timeout=timeout)
        except queue.Empty:
            return None

    def _read_until(self, predicate, timeout: float | None = None) -> list[str]:
        collected: list[str] = []
        deadline = None if timeout is None else time.monotonic() + timeout
        while True:
            remaining = None
            if deadline is not None:
                remaining = deadline - time.monotonic()
                if remaining <= 0:
                    break
            line = self._read_line(timeout=remaining)
            if line is None:
                break
            collected.append(line)
            if predicate(line):
                break
        return collected

    def is_alive(self) -> bool:
        if self._proc is None:
            return False
        return self._proc.poll() is None and self._alive

    def search(self, fen: str) -> tuple[str, int | None, bool]:
        """返回 (bestmove, score_cp, is_mate)。mate 时 score_cp 为 mate 步数。"""
        if not self.is_alive():
            raise RuntimeError("引擎已崩溃")
        self.send(f"position fen {fen}")
        self.send(f"go depth {self.depth}")
        timeout = max(60.0, float(self.depth) * 20.0)
        lines = self._read_until(
            lambda line: line.startswith("bestmove"),
            timeout=timeout,
        )
        if not self.is_alive():
            raise RuntimeError("引擎在搜索中崩溃")
        if not lines or not lines[-1].startswith("bestmove"):
            try:
                self.send("stop")
            except Exception:
                pass
            extra = self._read_until(
                lambda line: line.startswith("bestmove"),
                timeout=5.0,
            )
            lines.extend(extra)
        if not lines or not lines[-1].startswith("bestmove"):
            if not self.is_alive():
                raise RuntimeError("引擎在搜索中崩溃")
            raise RuntimeError("未返回 bestmove：" + "\n".join(lines[-10:]))

        score_cp: int | None = None
        is_mate = False
        for line in lines:
            mm = _SCORE_MATE_RE.search(line)
            if mm:
                score_cp = int(mm.group(1))
                is_mate = True
            cm = _SCORE_CP_RE.search(line)
            if cm and not is_mate:
                score_cp = int(cm.group(1))

        m = _BESTMOVE_RE.match(lines[-1].strip())
        if not m:
            raise RuntimeError(f"无法解析 bestmove：{lines[-1]}")
        move = m.group(1)
        return move, score_cp, is_mate

    def quit(self) -> None:
        proc = self._proc
        if proc is None:
            return
        try:
            if proc.poll() is None:
                try:
                    self.send("quit")
                except Exception:
                    pass
                try:
                    proc.wait(timeout=2.0)
                except subprocess.TimeoutExpired:
                    proc.kill()
        finally:
            self._proc = None
            self._alive = False


# ---------------------------------------------------------------------------
# 单局对弈
# ---------------------------------------------------------------------------


@dataclass
class GameReport:
    game_id: int
    result: GameResult
    ply: int
    ours_color: str  # "w" / "b"
    detail: str = ""
    moves: list[str] = field(default_factory=list)


def _result_for_side_to_move_lost(side: str, ours_color: str) -> GameResult:
    """当前走棋方无子可走（将被杀/困毙），返回相对己方的胜负。"""
    if side == ours_color:
        return GameResult.LOSS
    return GameResult.WIN


def _piece_count(pos: Position) -> int:
    return sum(1 for rank in pos.board for p in rank if p is not None)


def play_one_game(
    game_id: int,
    ours_exe: Path,
    foe_exe: Path,
    ours_depth: int,
    foe_depth: int,
    ours_plays_red: bool,
) -> GameReport:
    ours_color = "w" if ours_plays_red else "b"
    ours: UciEngine | None = None
    foe: UciEngine | None = None
    moves: list[str] = []

    try:
        ours = UciEngine(ours_exe, depth=ours_depth)
        foe = UciEngine(foe_exe, depth=foe_depth)
        ours.start()
        foe.start()

        fen = INIT_FEN
        pos = Position.from_fen(fen)
        no_capture = 0
        last_score_from_ours: int | None = None
        last_mate_from_ours = False

        for ply in range(MAX_PLY):
            legal = generate_legal_moves(pos)
            if not legal:
                result = _result_for_side_to_move_lost(pos.side, ours_color)
                return GameReport(
                    game_id, result, ply, ours_color, "无合法着法", moves
                )

            is_ours = pos.side == ours_color
            engine = ours if is_ours else foe
            assert engine is not None

            try:
                move, score_cp, is_mate = engine.search(fen)
            except Exception as exc:
                if is_ours:
                    return GameReport(
                        game_id,
                        GameResult.CRASH,
                        ply,
                        ours_color,
                        str(exc),
                        moves,
                    )
                return GameReport(
                    game_id,
                    GameResult.DRAW,
                    ply,
                    ours_color,
                    f"对手异常：{exc}",
                    moves,
                )

            if is_ours:
                last_score_from_ours = score_cp
                last_mate_from_ours = is_mate

            # 引擎主动给出无着 / 评估恰为 0 → 和棋（重复等由皮卡鱼自行体现）
            if move.lower() == "(none)" or (
                not is_mate and score_cp is not None and score_cp == 0
            ):
                return GameReport(
                    game_id,
                    GameResult.DRAW,
                    ply,
                    ours_color,
                    "引擎判和（bestmove none 或 score cp 0）",
                    moves,
                )

            if move not in legal:
                if is_ours:
                    return GameReport(
                        game_id,
                        GameResult.ILLEGAL,
                        ply,
                        ours_color,
                        f"非法着法 {move}，合法数={len(legal)}",
                        moves,
                    )
                return GameReport(
                    game_id,
                    GameResult.DRAW,
                    ply,
                    ours_color,
                    f"对手非法着法 {move}",
                    moves,
                )

            pieces_before = _piece_count(pos)
            moves.append(move)
            try:
                pos = make_move(pos, move)
            except Exception as exc:
                if is_ours:
                    return GameReport(
                        game_id,
                        GameResult.ILLEGAL,
                        ply,
                        ours_color,
                        f"走子失败 {move}: {exc}",
                        moves,
                    )
                return GameReport(
                    game_id,
                    GameResult.DRAW,
                    ply,
                    ours_color,
                    f"对手走子失败 {move}: {exc}",
                    moves,
                )

            if _piece_count(pos) < pieces_before:
                no_capture = 0
            else:
                no_capture += 1
                if no_capture >= NO_CAPTURE_DRAW_PLY:
                    return GameReport(
                        game_id,
                        GameResult.DRAW,
                        ply + 1,
                        ours_color,
                        f"无吃子限着和（{NO_CAPTURE_DRAW_PLY} ply）",
                        moves,
                    )

            fen = pos.key()

            if not generate_legal_moves(pos):
                if pos.side == ours_color:
                    return GameReport(
                        game_id,
                        GameResult.LOSS,
                        ply + 1,
                        ours_color,
                        "己方无合法着法",
                        moves,
                    )
                return GameReport(
                    game_id,
                    GameResult.WIN,
                    ply + 1,
                    ours_color,
                    "对方无合法着法",
                    moves,
                )

        # 到达局长：显著优势却杀不死 → too_long；否则按和棋
        if last_mate_from_ours or (
            last_score_from_ours is not None and abs(last_score_from_ours) >= 300
        ):
            return GameReport(
                game_id,
                GameResult.TOO_LONG,
                MAX_PLY,
                ours_color,
                f"达到 {MAX_PLY} ply 且仍有明显优势未终局 "
                f"(score={last_score_from_ours}"
                f"{' mate' if last_mate_from_ours else ''})",
                moves,
            )
        return GameReport(
            game_id,
            GameResult.DRAW,
            MAX_PLY,
            ours_color,
            f"达到 {MAX_PLY} ply，判和",
            moves,
        )
    finally:
        if ours is not None:
            ours.quit()
        if foe is not None:
            foe.quit()


# ---------------------------------------------------------------------------
# 多进程工人
# ---------------------------------------------------------------------------


@dataclass(frozen=True)
class _Job:
    game_id: int
    ours_exe: str
    foe_exe: str
    ours_depth: int
    foe_depth: int
    ours_plays_red: bool


def _run_job(job: _Job) -> GameReport:
    return play_one_game(
        job.game_id,
        Path(job.ours_exe),
        Path(job.foe_exe),
        job.ours_depth,
        job.foe_depth,
        job.ours_plays_red,
    )


# ---------------------------------------------------------------------------
# 日志 / 逐局过程 / 汇总
# ---------------------------------------------------------------------------


def _format_game_record(report: GameReport) -> str:
    """生成单局复盘文本（含中文着法）。"""
    lines: list[str] = [
        f"game_id: {report.game_id}",
        f"result: {report.result.value}",
        f"ply: {report.ply}",
        f"ours_color: {report.ours_color}",
        f"detail: {report.detail}",
        f"moves: {len(report.moves)}",
        "",
        "--- moves ---",
    ]
    fen = INIT_FEN
    for i, move in enumerate(report.moves, start=1):
        try:
            shown = format_move(fen, move)
        except Exception:
            shown = move
        side = "红" if Position.from_fen(fen).side == "w" else "黑"
        lines.append(f"{i:3d}. [{side}] {shown}")
        try:
            fen = make_move(Position.from_fen(fen), move).key()
        except Exception:
            lines.append(f"      !! 局面无法继续应用着法 {move}")
            break
    lines.append("")
    lines.append(f"final_fen: {fen}")
    return "\n".join(lines) + "\n"


class ResultLogger:
    def __init__(
        self,
        path: Path = LOG_PATH,
        games_dir: Path = GAMES_DIR,
    ) -> None:
        self.path = path
        self.games_dir = games_dir
        path.parent.mkdir(parents=True, exist_ok=True)
        if games_dir.exists():
            shutil.rmtree(games_dir)
        games_dir.mkdir(parents=True, exist_ok=True)
        self._lock = threading.Lock()
        self.counts: dict[GameResult, int] = {r: 0 for r in GameResult}

    def write(self, report: GameReport) -> None:
        self.counts[report.result] += 1
        mark = ""
        if report.result in (
            GameResult.ILLEGAL,
            GameResult.CRASH,
            GameResult.TOO_LONG,
        ):
            mark = " !!!"
        line = (
            f"[{datetime.now().strftime('%Y-%m-%d %H:%M:%S')}] "
            f"GAME {report.game_id} {report.result.value.upper()}{mark} "
            f"ply={report.ply} ours={report.ours_color} "
            f"{report.detail}"
        )
        record_path = self.games_dir / f"game_{report.game_id:04d}.txt"
        record_body = _format_game_record(report)
        with self._lock:
            with open(self.path, "a", encoding="utf-8") as f:
                f.write(line + "\n")
                f.flush()
            with open(record_path, "w", encoding="utf-8") as f:
                f.write(record_body)
        _log(line)
        _log(f"  过程已写入：{record_path.name}")

    def summary(self) -> str:
        total = sum(self.counts.values())
        parts = [f"{r.value}={self.counts[r]}" for r in GameResult]
        return f"合计 {total} 局：" + "，".join(parts)


# ---------------------------------------------------------------------------
# 主流程
# ---------------------------------------------------------------------------


def prepare_engines() -> tuple[Path, Path, bool]:
    """返回 (己方 exe, 皮卡鱼 exe, 是否替身模式)。"""
    ensure_engine()
    foe = PIKAFISH_EXE
    if is_refactoring():
        _log(
            f"检测到 README 含「{REFACTOR_MARKER}」，"
            "使用皮卡鱼代替 Chess98 进行协议自测。"
        )
        return foe, foe, True
    _log("README 未处于重构标记，编译并使用 Chess98。")
    ours = compile_chess98(CHESS98_EXE)
    return ours, foe, False


def run_autotest(
    count: int = DEFAULT_GAMES,
    depth: int = DEFAULT_DEPTH,
    workers: int | None = None,
) -> int:
    if count < 1:
        raise ValueError("对局数必须 >= 1")
    if depth < 1:
        raise ValueError("搜索深度必须 >= 1")

    workers = workers if workers is not None else _worker_count()
    _CACHE_DIR.mkdir(parents=True, exist_ok=True)

    _log("=" * 60)
    _log("Chess98 自动对弈测试")
    _log("=" * 60)

    ours_exe, foe_exe, standin = prepare_engines()
    ours_depth = depth
    # 替身模式双方同强容易拖满局长/刻意规避重复；对手降 3 层以便分出胜负验证流程
    foe_depth = max(1, depth - 3) if standin else depth

    _log(f"己方引擎：{ours_exe}" + ("（皮卡鱼替身）" if standin else ""))
    _log(f"对手引擎：{foe_exe}")
    _log(
        f"搜索深度：己方={ours_depth} 对手={foe_depth}，"
        f"对局数：{count}，进程数：{workers}"
    )
    _log(f"局长度上限：{MAX_PLY} ply；无吃子限着：{NO_CAPTURE_DRAW_PLY} ply")
    _log(f"汇总日志：{LOG_PATH}")
    _log(f"对局过程：{GAMES_DIR}/game_XXXX.txt")

    logger = ResultLogger(LOG_PATH, GAMES_DIR)
    with open(LOG_PATH, "a", encoding="utf-8") as f:
        f.write(
            f"\n==== autotest start {datetime.now().isoformat()} "
            f"count={count} depth={depth} workers={workers} "
            f"standin={standin} ====\n"
        )

    jobs = [
        _Job(
            game_id=i + 1,
            ours_exe=str(ours_exe),
            foe_exe=str(foe_exe),
            ours_depth=ours_depth,
            foe_depth=foe_depth,
            ours_plays_red=(i % 2 == 0),
        )
        for i in range(count)
    ]

    t0 = time.monotonic()
    try:
        with ProcessPoolExecutor(max_workers=workers) as pool:
            futures = {pool.submit(_run_job, job): job for job in jobs}
            for fut in as_completed(futures):
                job = futures[fut]
                try:
                    report = fut.result()
                except Exception as exc:
                    report = GameReport(
                        job.game_id,
                        GameResult.CRASH,
                        0,
                        "w" if job.ours_plays_red else "b",
                        f"工人异常：{exc}",
                    )
                logger.write(report)
    finally:
        elapsed = time.monotonic() - t0
        summary = logger.summary()
        _log("-" * 60)
        _log(summary)
        _log(f"耗时 {elapsed:.1f}s")
        with open(LOG_PATH, "a", encoding="utf-8") as f:
            f.write(summary + f" elapsed={elapsed:.1f}s\n")

    # 有违规/崩溃时返回非零，方便 CI
    bad = logger.counts[GameResult.ILLEGAL] + logger.counts[GameResult.CRASH]
    return 1 if bad else 0


def build_parser() -> argparse.ArgumentParser:
    p = argparse.ArgumentParser(
        prog="python -m kits.autotest",
        description="Chess98 vs 皮卡鱼自动对弈测试",
    )
    p.add_argument(
        "--count",
        type=int,
        default=DEFAULT_GAMES,
        help=f"测试对局次数（默认 {DEFAULT_GAMES}）",
    )
    p.add_argument(
        "--depth",
        type=int,
        default=DEFAULT_DEPTH,
        help=f"皮卡鱼/引擎搜索深度（默认 {DEFAULT_DEPTH}）",
    )
    p.add_argument(
        "--workers",
        type=int,
        default=None,
        help="并行进程数（默认 约物理核-2）",
    )
    return p


def main(argv: list[str] | None = None) -> int:
    args = build_parser().parse_args(argv)
    try:
        return run_autotest(
            count=args.count,
            depth=args.depth,
            workers=args.workers,
        )
    except Exception as exc:
        _log(f"自动测试失败：{exc}")
        return 2


if __name__ == "__main__":
    raise SystemExit(main())
