"""皮卡鱼引擎封装：按需下载、选型，并经 UCI 通信。

仅作为模块导入使用；直接运行时立即退出。
"""

from __future__ import annotations

import os
import queue
import re
import shutil
import subprocess
import sys
import threading
import time
import urllib.error
import urllib.request
from pathlib import Path

# ---------------------------------------------------------------------------
# 常量
# ---------------------------------------------------------------------------

PIKAFISH_URL = (
    "https://github.com/official-pikafish/Pikafish/releases/download/"
    "Pikafish-2026-01-02/Pikafish.2026-01-02.7z"
)

_CACHE_DIR = Path(__file__).resolve().parent / "__pycache__"
ZIP_PATH = _CACHE_DIR / "pikafish.7z"
EXTRACT_PATH = _CACHE_DIR / "pikafish"
EXE_PATH = _CACHE_DIR / "pikafish.exe"
NNUE_PATH = _CACHE_DIR / "pikafish.nnue"

# 快 → 慢（文件名匹配用）
_ENGINE_PRIORITY = (
    "avx512icl",
    "vnni512",
    "avx512",
    "avxvnni",
    "bmi2",
    "avx2",
    "sse41-popcnt",
    "ssse3",
    "sse3-popcnt",
    "sse3",
    "x86-64",
    "general-64",
)

_DEFAULT_MAX_DEPTH = 20
_DOWNLOAD_ATTEMPTS = 3
_USER_AGENT = "Chess98-pikafish/1.0"

_SCORE_CP_RE = re.compile(r"\bscore\s+cp\s+(-?\d+)\b")
_SCORE_MATE_RE = re.compile(r"\bscore\s+mate\s+(-?\d+)\b")
_BESTMOVE_RE = re.compile(
    r"^bestmove\s+(\S+)(?:\s+ponder\s+(\S+))?", re.IGNORECASE
)
_MULTIPV_RE = re.compile(r"\bmultipv\s+(\d+)\b")
_PV_MOVE_RE = re.compile(r"\bpv\s+(\S+)")


def _parse_score(line: str) -> str | None:
    cm = _SCORE_CP_RE.search(line)
    if cm:
        return cm.group(1)
    mm = _SCORE_MATE_RE.search(line)
    if mm:
        return f"mate {mm.group(1)}"
    return None


def _parse_search_lines(lines: list[str]) -> tuple[str, str]:
    move = ""
    vl = ""
    for line in lines:
        score = _parse_score(line)
        if score is not None:
            vl = score
        bm = _BESTMOVE_RE.match(line.strip())
        if bm:
            move = bm.group(1)
    if not move or move.lower() == "(none)":
        raise RuntimeError(
            "皮卡鱼未返回 bestmove：\n" + "\n".join(lines[-30:])
        )
    return move, vl


def _parse_multipv_lines(lines: list[str], n: int) -> list[tuple[str, str]]:
    """从 UCI info 行解析 MultiPV 结果，按 multipv 编号排序。"""
    slots: dict[int, tuple[str, str]] = {}
    for line in lines:
        if not line.startswith("info "):
            continue
        mp = _MULTIPV_RE.search(line)
        pv = _PV_MOVE_RE.search(line)
        if not pv:
            continue
        idx = int(mp.group(1)) if mp else 1
        score = _parse_score(line) or ""
        slots[idx] = (pv.group(1), score)
    results = [slots[i] for i in sorted(slots) if i <= n]
    if results:
        return results
    # 无 multipv 标记时退回 bestmove
    move, vl = _parse_search_lines(lines)
    return [(move, vl)]

# ---------------------------------------------------------------------------
# 命令行提示 / 进度条
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
        encoding = getattr(sys.stdout, "encoding", None) or "utf-8"
        sys.stdout.buffer.write(
            (message + "\n").encode(encoding, errors="replace")
        )
        sys.stdout.buffer.flush()


def _die(message: str) -> None:
    try:
        print(message, file=sys.stderr, flush=True)
    except UnicodeEncodeError:
        sys.stderr.buffer.write(
            (message + "\n").encode("utf-8", errors="replace")
        )
        sys.stderr.buffer.flush()
    raise SystemExit(1)


def _format_bytes(n: float) -> str:
    units = ("B", "KB", "MB", "GB")
    value = float(max(0, n))
    for unit in units:
        if value < 1024.0 or unit == units[-1]:
            if unit == "B":
                return f"{int(value)}{unit}"
            return f"{value:.1f}{unit}"
        value /= 1024.0
    return f"{n}B"


def _bar_charset() -> tuple[str, str]:
    """返回 (filled, empty)；在窄编码终端回退到 ASCII。"""
    filled, empty = "#", "-"
    encoding = (getattr(sys.stdout, "encoding", None) or "").lower()
    if "utf" in encoding:
        return "█", "░"
    return filled, empty


class _ProgressBar:
    """单行进度条（stdout）；非 TTY 时退化为阶段性文字。"""

    def __init__(
        self,
        label: str,
        total: int | None = None,
        *,
        unit: str = "bytes",
    ) -> None:
        self.label = label
        self.total = total if total and total > 0 else None
        self.unit = unit  # "bytes" | "count"
        self.current = 0
        self._tty = sys.stdout.isatty()
        self._last_render = 0.0
        self._start = time.monotonic()
        self._finished = False
        self._filled, self._empty = _bar_charset()
        if not self._tty:
            if self.total is not None:
                _log(f"{label}（共 {self._fmt(self.total)}）…")
            else:
                _log(f"{label}…")

    def _fmt(self, n: float) -> str:
        if self.unit == "count":
            return str(int(n))
        return _format_bytes(n)

    def update(self, current: int) -> None:
        if self._finished:
            return
        self.current = max(0, current)
        if not self._tty:
            return
        now = time.monotonic()
        if now - self._last_render < 0.08 and (
            self.total is None or self.current < self.total
        ):
            return
        self._last_render = now
        self._render()

    def advance(self, delta: int) -> None:
        self.update(self.current + delta)

    def finish(self) -> None:
        if self._finished:
            return
        self._finished = True
        if self.total is not None:
            self.current = self.total
        if self._tty:
            self._render(final=True)
            print(flush=True)
        else:
            elapsed = time.monotonic() - self._start
            if self.unit == "count" and self.total is not None:
                _log(f"{self.label}完成（{self.total}/{self.total}，{elapsed:.1f}s）")
            elif self.total is None and self.unit != "bytes":
                _log(f"{self.label}完成（{elapsed:.1f}s）")
            else:
                _log(
                    f"{self.label}完成（{self._fmt(self.current)}，{elapsed:.1f}s）"
                )

    def _render(self, final: bool = False) -> None:
        width = 28
        elapsed = max(0.001, time.monotonic() - self._start)
        if self.total is not None:
            ratio = min(1.0, self.current / self.total)
            filled = int(width * ratio)
            bar = self._filled * filled + self._empty * (width - filled)
            if self.unit == "count":
                line = (
                    f"\r{self.label} [{bar}] {ratio * 100:5.1f}% "
                    f"{int(self.current)}/{int(self.total)}"
                )
            else:
                speed = self.current / elapsed
                line = (
                    f"\r{self.label} [{bar}] {ratio * 100:5.1f}% "
                    f"{_format_bytes(self.current)}/{_format_bytes(self.total)} "
                    f"{_format_bytes(speed)}/s"
                )
        else:
            pulse = int(elapsed * 4) % width
            cells = [self._empty] * width
            for i in range(5):
                cells[(pulse + i) % width] = self._filled
            if self.unit == "bytes":
                speed = self.current / elapsed
                extra = f" {_format_bytes(self.current)} {_format_bytes(speed)}/s"
            else:
                extra = f" {elapsed:.1f}s"
            line = f"\r{self.label} [{''.join(cells)}]{extra}"
        try:
            print(line, end="" if not final else "\n", flush=True)
        except UnicodeEncodeError:
            ascii_line = line.encode("ascii", errors="replace").decode("ascii")
            print(ascii_line, end="" if not final else "\n", flush=True)


# ---------------------------------------------------------------------------
# 下载 / 解压 / 选型
# ---------------------------------------------------------------------------


def _safe_unlink(path: Path) -> None:
    try:
        path.unlink(missing_ok=True)
    except OSError:
        pass


def _replace_file(src: Path, dest: Path, attempts: int = 8) -> None:
    """Windows 上杀毒软件可能短暂占用刚写完的文件，因此带重试。"""
    last_error: Exception | None = None
    for i in range(attempts):
        try:
            os.replace(src, dest)
            return
        except OSError as exc:
            last_error = exc
            time.sleep(0.25 * (i + 1))
    raise RuntimeError(f"无法将 {src} 移动到 {dest}: {last_error}") from last_error


def _download(url: str, dest: Path) -> None:
    dest.parent.mkdir(parents=True, exist_ok=True)
    tmp = dest.with_name(f"{dest.name}.{os.getpid()}.{time.time_ns()}.part")
    req = urllib.request.Request(url, headers={"User-Agent": _USER_AGENT})
    incomplete = True
    bar: _ProgressBar | None = None
    try:
        with urllib.request.urlopen(req, timeout=300) as resp, open(tmp, "wb") as out:
            total_header = resp.headers.get("Content-Length")
            total = int(total_header) if total_header and total_header.isdigit() else None
            bar = _ProgressBar("下载皮卡鱼", total)
            while True:
                chunk = resp.read(256 * 1024)
                if not chunk:
                    break
                out.write(chunk)
                bar.advance(len(chunk))
            out.flush()
            os.fsync(out.fileno())
        incomplete = False
        if bar is not None:
            bar.finish()
        _log(f"下载完成：{dest}")
        _replace_file(tmp, dest)
    except Exception:
        if bar is not None and not bar._finished:
            print(flush=True)
        if incomplete:
            _safe_unlink(tmp)
        elif tmp.exists():
            # 下载完整但改名失败：尽量挪到目标，否则留给下次清理
            try:
                _replace_file(tmp, dest)
            except Exception:
                _safe_unlink(tmp)
                raise
        else:
            raise


def _extract_7z(archive: Path, dest_dir: Path) -> None:
    if dest_dir.exists():
        shutil.rmtree(dest_dir)
    dest_dir.mkdir(parents=True, exist_ok=True)

    try:
        import py7zr  # type: ignore

        with py7zr.SevenZipFile(archive, mode="r") as zf:
            names = zf.getnames()
            _log(f"正在解压皮卡鱼（{len(names)} 个条目）…")
            bar = _ProgressBar("解压皮卡鱼", None, unit="count")
            stop = threading.Event()

            def _pulse() -> None:
                n = 0
                while not stop.is_set():
                    n += 1
                    bar.update(n)
                    stop.wait(0.12)

            worker = threading.Thread(target=_pulse, daemon=True)
            worker.start()
            try:
                zf.extractall(path=dest_dir)
            finally:
                stop.set()
                worker.join(timeout=1.0)
                bar.finish()
        _log(f"解压完成：{dest_dir}")
        return
    except ImportError:
        pass

    for cmd in ("7z", "7za", "7zr"):
        exe = shutil.which(cmd)
        if exe:
            _log(f"使用 {cmd} 解压皮卡鱼…")
            subprocess.run(
                [exe, "x", str(archive), f"-o{dest_dir}", "-y"],
                check=True,
                capture_output=True,
            )
            _log(f"解压完成：{dest_dir}")
            return

    _die(
        "无法解压 .7z：请安装 py7zr（pip install py7zr）或将 7-Zip 加入 PATH。"
    )


def _is_engine_binary(path: Path) -> bool:
    if not path.is_file():
        return False
    name = path.name.lower()
    if "nnue" in name:
        return False
    if sys.platform == "win32":
        return path.suffix.lower() == ".exe" and "pikafish" in name
    return "pikafish" in name and os.access(path, os.X_OK)


def _iter_binaries(root: Path) -> list[Path]:
    if not root.is_dir():
        return []
    return [p for p in root.rglob("*") if _is_engine_binary(p)]


def _priority_index(path: Path) -> int:
    name = path.stem.lower()
    for i, tag in enumerate(_ENGINE_PRIORITY):
        if tag in name:
            return i
    return len(_ENGINE_PRIORITY)


def _creationflags() -> int:
    if sys.platform == "win32":
        return subprocess.CREATE_NO_WINDOW  # type: ignore[attr-defined]
    return 0


def _probe_uci(path: Path, timeout: float = 5.0) -> bool:
    """启动二进制并确认能完成 UCI 握手。"""
    proc: subprocess.Popen[bytes] | None = None
    try:
        proc = subprocess.Popen(
            [str(path)],
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.DEVNULL,
            # 二进制模式：不兼容指令集崩溃时避免 TextIOWrapper 收尾异常
            bufsize=0,
            cwd=str(path.parent),
            creationflags=_creationflags(),
        )
    except OSError:
        return False

    assert proc.stdin is not None and proc.stdout is not None
    q: queue.Queue[bytes | None] = queue.Queue()

    def reader() -> None:
        try:
            while True:
                line = proc.stdout.readline()
                if not line:
                    q.put(None)
                    break
                q.put(line)
        except Exception:
            q.put(None)

    threading.Thread(target=reader, daemon=True).start()
    try:
        proc.stdin.write(b"uci\n")
        proc.stdin.flush()
        ok = False
        end = time.monotonic() + timeout
        while time.monotonic() < end:
            try:
                line = q.get(timeout=0.1)
            except queue.Empty:
                if proc.poll() is not None:
                    break
                continue
            if line is None:
                break
            if line.strip() == b"uciok":
                ok = True
                break
        return ok
    except Exception:
        return False
    finally:
        if proc is not None:
            try:
                if proc.poll() is None:
                    try:
                        proc.stdin.write(b"quit\n")
                        proc.stdin.flush()
                    except Exception:
                        pass
                    try:
                        proc.wait(timeout=0.5)
                    except Exception:
                        pass
                if proc.poll() is None:
                    proc.kill()
                    try:
                        proc.wait(timeout=1.0)
                    except Exception:
                        pass
            except Exception:
                pass
            for stream in (proc.stdin, proc.stdout, proc.stderr):
                try:
                    if stream is not None:
                        stream.close()
                except Exception:
                    pass


def _find_nnue(root: Path) -> Path | None:
    direct = root / "pikafish.nnue"
    if direct.is_file():
        return direct
    matches = list(root.rglob("*.nnue"))
    return matches[0] if matches else None


def _select_best_binary(extract_dir: Path) -> Path:
    binaries = sorted(_iter_binaries(extract_dir), key=_priority_index)
    if not binaries:
        _die(f"解压目录中未找到皮卡鱼可执行文件：{extract_dir}")

    _log(f"开始探测适合本机的皮卡鱼二进制（共 {len(binaries)} 个）…")
    bar = _ProgressBar("选型探测", len(binaries), unit="count")
    chosen: Path | None = None
    for i, path in enumerate(binaries, start=1):
        bar.update(i - 1)
        if _probe_uci(path):
            chosen = path
            bar.update(i)
            break
        bar.update(i)

    if chosen is None:
        bar.finish()
        _die(f"当前 CPU 无法运行任何皮卡鱼二进制：{extract_dir}")

    bar.finish()
    _log(f"已选择：{chosen.name}")
    return chosen


def _salvage_part_files() -> None:
    """把可能残留的完整 .part 抢救为 ZIP_PATH。"""
    if ZIP_PATH.is_file():
        return
    parts = sorted(
        _CACHE_DIR.glob(f"{ZIP_PATH.name}*.part"),
        key=lambda p: p.stat().st_mtime,
        reverse=True,
    )
    for part in parts:
        # 发布包约 54MB；过小的视为未下完
        if part.stat().st_size < 50 * 1024 * 1024:
            continue
        try:
            _replace_file(part, ZIP_PATH)
            _log(f"已恢复未完成的下载文件：{ZIP_PATH.name}")
            return
        except Exception:
            continue


def _install_engine() -> None:
    """下载（必要时重试）→ 解压 → 选型并安装到 EXE_PATH。"""
    last_error: Exception | None = None
    _CACHE_DIR.mkdir(parents=True, exist_ok=True)
    _salvage_part_files()

    for attempt in range(1, _DOWNLOAD_ATTEMPTS + 1):
        try:
            _log(
                f"准备安装皮卡鱼"
                f"（第 {attempt}/{_DOWNLOAD_ATTEMPTS} 次）…"
            )
            if not ZIP_PATH.is_file():
                _log(f"从 GitHub 下载：{PIKAFISH_URL}")
                _download(PIKAFISH_URL, ZIP_PATH)
            else:
                _log(f"使用已有压缩包：{ZIP_PATH}")

            _extract_7z(ZIP_PATH, EXTRACT_PATH)
            best = _select_best_binary(EXTRACT_PATH)

            _log("正在安装可执行文件与 NNUE…")
            shutil.copy2(best, EXE_PATH)
            nnue = _find_nnue(EXTRACT_PATH)
            if nnue is not None:
                shutil.copy2(nnue, NNUE_PATH)
                _log(f"NNUE：{NNUE_PATH.name}")
            else:
                _log("警告：未找到 NNUE 文件，引擎可能无法正常评估。")

            if not _probe_uci(EXE_PATH):
                raise RuntimeError("安装后的 pikafish.exe 无法通过 UCI 探测")
            _log(f"皮卡鱼已就绪：{EXE_PATH}")
            return
        except SystemExit:
            raise
        except Exception as exc:
            last_error = exc
            _log(f"安装失败：{exc}")
            _safe_unlink(EXE_PATH)
            _safe_unlink(NNUE_PATH)
            if attempt < _DOWNLOAD_ATTEMPTS:
                _log("将清理缓存并重试…")
                _safe_unlink(ZIP_PATH)
                if EXTRACT_PATH.exists():
                    shutil.rmtree(EXTRACT_PATH, ignore_errors=True)
                time.sleep(1.0 * attempt)

    _die(
        f"下载或安装皮卡鱼失败（已尝试 {_DOWNLOAD_ATTEMPTS} 次）：{last_error}"
    )


def ensure_engine() -> Path:
    """保证 EXE_PATH 可用；缺失则下载安装。"""
    if EXE_PATH.is_file() and _probe_uci(EXE_PATH):
        return EXE_PATH
    _log("本地皮卡鱼不可用，开始自动安装…")
    _install_engine()
    return EXE_PATH


# ---------------------------------------------------------------------------
# UCI 进程
# ---------------------------------------------------------------------------


class _EngineProcess:
    def __init__(self, exe: Path) -> None:
        self._exe = exe
        self._proc: subprocess.Popen[str] | None = None
        self._queue: queue.Queue[str | None] = queue.Queue()
        self._lock = threading.Lock()

    def start(self) -> None:
        if self._proc is not None and self._proc.poll() is None:
            return

        self._proc = subprocess.Popen(
            [str(self._exe)],
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.DEVNULL,
            text=True,
            bufsize=1,
            cwd=str(self._exe.parent),
            creationflags=_creationflags(),
        )
        self._queue = queue.Queue()
        threading.Thread(target=self._reader, daemon=True).start()

        self.send("uci")
        self.read_until(lambda line: line.strip() == "uciok", timeout=10.0)

        if NNUE_PATH.is_file():
            self.send(f"setoption name EvalFile value {NNUE_PATH.resolve()}")

        self.send("isready")
        self.read_until(lambda line: line.strip() == "readyok", timeout=60.0)

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
        with self._lock:
            self._proc.stdin.write(command.rstrip("\n") + "\n")
            self._proc.stdin.flush()

    def read_line(self, timeout: float | None = None) -> str | None:
        try:
            if timeout is None:
                return self._queue.get()
            return self._queue.get(timeout=timeout)
        except queue.Empty:
            return None

    def read_until(
        self,
        predicate,
        timeout: float | None = None,
    ) -> list[str]:
        collected: list[str] = []
        deadline = None if timeout is None else time.monotonic() + timeout
        while True:
            remaining = None
            if deadline is not None:
                remaining = deadline - time.monotonic()
                if remaining <= 0:
                    break
            line = self.read_line(timeout=remaining)
            if line is None:
                break
            collected.append(line)
            if predicate(line):
                break
        return collected

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
                    proc.wait(timeout=3.0)
                except subprocess.TimeoutExpired:
                    proc.kill()
        finally:
            self._proc = None


# ---------------------------------------------------------------------------
# 对外 API
# ---------------------------------------------------------------------------


class PikafishEngine:
    """独立皮卡鱼进程，可供多进程/多实例并行使用。"""

    def __init__(self, max_depth: int = _DEFAULT_MAX_DEPTH) -> None:
        ensure_engine()
        self.max_depth = max_depth
        self._engine = _EngineProcess(EXE_PATH)
        self._engine.start()

    def set_max_depth(self, max_depth: int) -> bool:
        if not isinstance(max_depth, int) or max_depth < 1:
            return False
        self.max_depth = max_depth
        return True

    def set_option(self, name: str, value: str | int) -> None:
        self._engine.send(f"setoption name {name} value {value}")

    def search(self, fen: str) -> tuple[str, str]:
        fen = (fen or "").strip()
        if not fen:
            raise ValueError("fen 不能为空")
        self.set_option("MultiPV", 1)
        self._engine.send(f"position fen {fen}")
        self._engine.send(f"go depth {self.max_depth}")
        lines = self._wait_bestmove()
        return _parse_search_lines(lines)

    def search_multipv(self, fen: str, n: int) -> list[tuple[str, str]]:
        fen = (fen or "").strip()
        if not fen:
            raise ValueError("fen 不能为空")
        n = max(1, int(n))
        self.set_option("MultiPV", n)
        self._engine.send(f"position fen {fen}")
        self._engine.send(f"go depth {self.max_depth}")
        lines = self._wait_bestmove()
        # 恢复默认，避免影响后续单 PV 搜索
        self.set_option("MultiPV", 1)
        return _parse_multipv_lines(lines, n)

    def _wait_bestmove(self) -> list[str]:
        timeout = max(60.0, float(self.max_depth) * 15.0)
        lines = self._engine.read_until(
            lambda line: line.startswith("bestmove"),
            timeout=timeout,
        )
        if not lines or not lines[-1].startswith("bestmove"):
            self._engine.send("stop")
            extra = self._engine.read_until(
                lambda line: line.startswith("bestmove"),
                timeout=10.0,
            )
            lines.extend(extra)
        return lines

    def close(self) -> None:
        self._engine.quit()

    def __enter__(self) -> "PikafishEngine":
        return self

    def __exit__(self, *exc: object) -> None:
        self.close()


class Pikafish:
    """皮卡鱼门面：类方法按需拉起引擎。"""

    _max_depth: int = _DEFAULT_MAX_DEPTH
    _fen: str | None = None
    _engine: _EngineProcess | None = None
    _ready: bool = False

    @classmethod
    def _ensure(cls) -> _EngineProcess:
        ensure_engine()
        if cls._engine is None:
            cls._engine = _EngineProcess(EXE_PATH)
        cls._engine.start()
        cls._ready = True
        return cls._engine

    @classmethod
    def set_max_depth(cls, max_depth: int) -> bool:
        cls._ensure()
        if not isinstance(max_depth, int) or max_depth < 1:
            return False
        cls._max_depth = max_depth
        return True

    @classmethod
    def set_fen(cls, fen: str) -> bool:
        engine = cls._ensure()
        fen = (fen or "").strip()
        if not fen:
            return False
        try:
            engine.send(f"position fen {fen}")
            engine.send("isready")
            lines = engine.read_until(
                lambda line: line.strip() == "readyok", timeout=30.0
            )
            if not lines or lines[-1].strip() != "readyok":
                return False
            cls._fen = fen
            return True
        except Exception:
            return False

    @classmethod
    def search(cls, fen: str) -> tuple[str, str]:
        engine = cls._ensure()
        fen = (fen or "").strip()
        if not fen:
            raise ValueError("fen 不能为空")

        engine.send("setoption name MultiPV value 1")
        engine.send(f"position fen {fen}")
        engine.send(f"go depth {cls._max_depth}")

        timeout = max(60.0, float(cls._max_depth) * 15.0)
        lines = engine.read_until(
            lambda line: line.startswith("bestmove"),
            timeout=timeout,
        )
        if not lines or not lines[-1].startswith("bestmove"):
            engine.send("stop")
            extra = engine.read_until(
                lambda line: line.startswith("bestmove"),
                timeout=10.0,
            )
            lines.extend(extra)

        move, vl = _parse_search_lines(lines)
        cls._fen = fen
        return move, vl

    @classmethod
    def search_multipv(cls, fen: str, n: int) -> list[tuple[str, str]]:
        """搜索并返回前 n 个着法（MultiPV）。"""
        engine = cls._ensure()
        fen = (fen or "").strip()
        if not fen:
            raise ValueError("fen 不能为空")
        n = max(1, int(n))
        engine.send(f"setoption name MultiPV value {n}")
        engine.send(f"position fen {fen}")
        engine.send(f"go depth {cls._max_depth}")

        timeout = max(60.0, float(cls._max_depth) * 15.0)
        lines = engine.read_until(
            lambda line: line.startswith("bestmove"),
            timeout=timeout,
        )
        if not lines or not lines[-1].startswith("bestmove"):
            engine.send("stop")
            extra = engine.read_until(
                lambda line: line.startswith("bestmove"),
                timeout=10.0,
            )
            lines.extend(extra)

        engine.send("setoption name MultiPV value 1")
        cls._fen = fen
        return _parse_multipv_lines(lines, n)


if __name__ == "__main__":
    raise SystemExit(0)
