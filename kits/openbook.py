"""开局库：用皮卡鱼蒸馏局面，存入 SQLite，支持断点续训与多进程。"""

from __future__ import annotations

import argparse
import os
import sqlite3
import sys
import time
from concurrent.futures import ProcessPoolExecutor, as_completed
from dataclasses import dataclass
from pathlib import Path
from typing import Iterable

from kits.pikafish import Pikafish, PikafishEngine

# ---------------------------------------------------------------------------
# 常量
# ---------------------------------------------------------------------------

_CACHE_DIR = Path(__file__).resolve().parent / "__pycache__"
OPENBOOK_PATH = _CACHE_DIR / "openbook.db"
MAX_DEPTH = 13
INIT_FEN = "rnbakabnr/9/1c5c1/p1p1p1p1p/9/9/P1P1P1P1P/1C5C1/9/RNBAKABNR w"
PRUNING_BEG_PLY = 2
BEST_MOVES_NUM = 4

_FILES = "abcdefghi"
_PALACE_FILES = {3, 4, 5}
_RED_PALACE_RANKS = {0, 1, 2}
_BLACK_PALACE_RANKS = {7, 8, 9}

# ---------------------------------------------------------------------------
# 终端输出
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


class _ProgressBar:
    def __init__(self, label: str, total: int | None = None) -> None:
        self.label = label
        self.total = total if total and total > 0 else None
        self.current = 0
        self._tty = sys.stdout.isatty()
        self._last = 0.0
        self._start = time.monotonic()
        self._done = False
        if not self._tty:
            if self.total is not None:
                _log(f"{label}（共 {self.total}）…")
            else:
                _log(f"{label}…")

    def update(self, current: int, suffix: str = "") -> None:
        if self._done:
            return
        self.current = max(0, current)
        if not self._tty:
            return
        now = time.monotonic()
        if now - self._last < 0.08 and (
            self.total is None or self.current < (self.total or 0)
        ):
            return
        self._last = now
        self._render(suffix)

    def finish(self, suffix: str = "") -> None:
        if self._done:
            return
        self._done = True
        if self.total is not None:
            self.current = self.total
        if self._tty:
            self._render(suffix, final=True)
            print(flush=True)
        else:
            elapsed = time.monotonic() - self._start
            extra = f" {suffix}" if suffix else ""
            _log(f"{self.label}完成（{self.current}，{elapsed:.1f}s）{extra}")

    def _render(self, suffix: str = "", final: bool = False) -> None:
        width = 28
        filled_ch, empty_ch = ("█", "░") if "utf" in (
            getattr(sys.stdout, "encoding", "") or ""
        ).lower() else ("#", "-")
        if self.total:
            ratio = min(1.0, self.current / self.total)
            filled = int(width * ratio)
            bar = filled_ch * filled + empty_ch * (width - filled)
            line = (
                f"\r{self.label} [{bar}] {ratio * 100:5.1f}% "
                f"{self.current}/{self.total}"
            )
        else:
            pulse = int((time.monotonic() - self._start) * 4) % width
            cells = [empty_ch] * width
            for i in range(5):
                cells[(pulse + i) % width] = filled_ch
            line = f"\r{self.label} [{''.join(cells)}] {self.current}"
        if suffix:
            line += f" | {suffix}"
        try:
            print(line, end="" if not final else "\n", flush=True)
        except UnicodeEncodeError:
            print(
                line.encode("ascii", errors="replace").decode("ascii"),
                end="" if not final else "\n",
                flush=True,
            )


# ---------------------------------------------------------------------------
# 中国象棋：局面 / 着法 / 镜像
# ---------------------------------------------------------------------------


@dataclass(frozen=True)
class Position:
    """board[rank][file]，rank 0 为红底；side 为 'w'/'b'（红/黑）。"""

    board: tuple[tuple[str | None, ...], ...]
    side: str

    @staticmethod
    def from_fen(fen: str) -> "Position":
        parts = fen.strip().split()
        if len(parts) < 2:
            raise ValueError(f"非法 FEN：{fen}")
        rows = parts[0].split("/")
        if len(rows) != 10:
            raise ValueError(f"非法 FEN 行数：{fen}")
        grid: list[list[str | None]] = [[None] * 9 for _ in range(10)]
        for fen_row, rank in zip(rows, range(9, -1, -1)):
            file = 0
            for ch in fen_row:
                if ch.isdigit():
                    file += int(ch)
                else:
                    if file >= 9:
                        raise ValueError(f"非法 FEN：{fen}")
                    grid[rank][file] = ch
                    file += 1
            if file != 9:
                raise ValueError(f"非法 FEN：{fen}")
        side = parts[1][0].lower()
        if side not in ("w", "b"):
            raise ValueError(f"非法走棋方：{fen}")
        return Position(tuple(tuple(r) for r in grid), side)

    def to_fen(self) -> str:
        rows: list[str] = []
        for rank in range(9, -1, -1):
            empty = 0
            parts: list[str] = []
            for file in range(9):
                piece = self.board[rank][file]
                if piece is None:
                    empty += 1
                else:
                    if empty:
                        parts.append(str(empty))
                        empty = 0
                    parts.append(piece)
            if empty:
                parts.append(str(empty))
            rows.append("".join(parts))
        return "/".join(rows) + f" {self.side}"

    def key(self) -> str:
        return self.to_fen()

    def mirror(self) -> "Position":
        mirrored = tuple(
            tuple(row[::-1]) for row in self.board
        )
        return Position(mirrored, self.side)

    def piece_at(self, file: int, rank: int) -> str | None:
        return self.board[rank][file]


def _sq_uci(file: int, rank: int) -> str:
    return f"{_FILES[file]}{rank}"


def _parse_uci(move: str) -> tuple[int, int, int, int]:
    move = move.strip()
    if len(move) < 4:
        raise ValueError(f"非法着法：{move}")
    ff = _FILES.index(move[0])
    fr = int(move[1])
    tf = _FILES.index(move[2])
    tr = int(move[3])
    return ff, fr, tf, tr


def mirror_move(move: str) -> str:
    ff, fr, tf, tr = _parse_uci(move)
    return _sq_uci(8 - ff, fr) + _sq_uci(8 - tf, tr)


# 中文着法：红方路数从己方右侧数 1–9（对应 UCI i→a）；黑方同理（对应 UCI a→i）
_ZH_NUM = "一二三四五六七八九"
_ZH_PIECE_RED = {
    "K": "帅",
    "A": "仕",
    "B": "相",
    "N": "马",
    "R": "车",
    "C": "炮",
    "P": "兵",
}
_ZH_PIECE_BLACK = {
    "k": "将",
    "a": "士",
    "b": "象",
    "n": "马",
    "r": "车",
    "c": "炮",
    "p": "卒",
}


def _file_to_zh_num(file: int, red: bool) -> int:
    """UCI file 索引 0..8 → 该走棋方视角下的路数 1..9。"""
    return (9 - file) if red else (file + 1)


def _zh_digit(n: int) -> str:
    if 1 <= n <= 9:
        return _ZH_NUM[n - 1]
    return str(n)


def _piece_zh_name(piece: str) -> str:
    if piece in _ZH_PIECE_RED:
        return _ZH_PIECE_RED[piece]
    if piece in _ZH_PIECE_BLACK:
        return _ZH_PIECE_BLACK[piece]
    return piece


def _same_file_siblings(
    pos: Position, piece: str, file: int
) -> list[tuple[int, int]]:
    """同兵种且同路的棋子坐标，按「前→后」排序（相对走棋方）。"""
    red = _is_red(piece)
    sqs = [
        (f, r)
        for r in range(10)
        for f in range(9)
        if pos.board[r][f] == piece and f == file
    ]
    # 红方 rank 大者为前；黑方 rank 小者为前
    sqs.sort(key=lambda fr: fr[1], reverse=red)
    return sqs


def move_to_chinese(fen: str, move: str) -> str:
    """将 UCI/ICCS 着法转为中文中国象棋记谱（如 炮二平五）。"""
    pos = Position.from_fen(fen)
    ff, fr, tf, tr = _parse_uci(move)
    piece = pos.board[fr][ff]
    if piece is None:
        raise ValueError(f"着法起点无子：{move}")

    red = pos.side == "w"
    if _is_red(piece) != red:
        raise ValueError(f"非走棋方棋子：{move}")

    name = _piece_zh_name(piece)
    kind = piece.upper()
    from_line = _file_to_zh_num(ff, red)
    to_line = _file_to_zh_num(tf, red)

    # 进 / 退 / 平
    if fr == tr:
        verb = "平"
        tail = _zh_digit(to_line)
    else:
        advancing = (tr > fr) if red else (tr < fr)
        verb = "进" if advancing else "退"
        # 马、相/象、仕/士：末数字为到达路数；车炮兵将帅：为进退格数
        if kind in ("N", "B", "A"):
            tail = _zh_digit(to_line)
        else:
            tail = _zh_digit(abs(tr - fr))

    # 同路多子：前/后/中 + 棋子名 + 进退平 + 数字（省略路数）
    siblings = _same_file_siblings(pos, piece, ff)
    if len(siblings) >= 2:
        labels = {2: ("前", "后"), 3: ("前", "中", "后")}
        # 多于 3 个时按位置夹逼：前/中…/后，中间统一叫中再编号过少见，简化为前中后+序
        if len(siblings) in labels:
            tag = labels[len(siblings)][siblings.index((ff, fr))]
        else:
            idx = siblings.index((ff, fr))
            if idx == 0:
                tag = "前"
            elif idx == len(siblings) - 1:
                tag = "后"
            else:
                tag = "中"
        return f"{tag}{name}{verb}{tail}"

    # 兵/卒 不同路但需消歧时用前/后兵（同兵种多枚且仅靠路数仍可能混淆的少见情况从略）
    # 标准格式：棋子 + 路数 + 进/退/平 + 数字
    return f"{name}{_zh_digit(from_line)}{verb}{tail}"


def format_move(fen: str, move: str) -> str:
    """输出用：中文着法，失败时回退 UCI。"""
    try:
        zh = move_to_chinese(fen, move)
        return f"{zh}（{move}）"
    except Exception:
        return move


def canonicalize(fen: str) -> tuple[str, bool]:
    """返回 (规范 FEN, 是否相对原局面做了镜像)。"""
    pos = Position.from_fen(fen)
    a = pos.key()
    b = pos.mirror().key()
    if a <= b:
        return a, False
    return b, True


def _in_board(file: int, rank: int) -> bool:
    return 0 <= file < 9 and 0 <= rank < 10


def _is_red(piece: str) -> bool:
    return piece.isupper()


def _same_side(a: str, b: str) -> bool:
    return _is_red(a) == _is_red(b)


def _find_king(board: tuple[tuple[str | None, ...], ...], red: bool) -> tuple[int, int]:
    target = "K" if red else "k"
    for rank in range(10):
        for file in range(9):
            if board[rank][file] == target:
                return file, rank
    raise ValueError("局面缺少将/帅")


def _piece_attacks(
    board: tuple[tuple[str | None, ...], ...],
    file: int,
    rank: int,
    piece: str,
) -> list[tuple[int, int]]:
    red = _is_red(piece)
    kind = piece.upper()
    hits: list[tuple[int, int]] = []

    def add(f: int, r: int) -> None:
        if _in_board(f, r):
            hits.append((f, r))

    if kind == "K":
        for df, dr in ((1, 0), (-1, 0), (0, 1), (0, -1)):
            f, r = file + df, rank + dr
            if f in _PALACE_FILES and r in (
                _RED_PALACE_RANKS if red else _BLACK_PALACE_RANKS
            ):
                add(f, r)
    elif kind == "A":
        for df, dr in ((1, 1), (1, -1), (-1, 1), (-1, -1)):
            f, r = file + df, rank + dr
            if f in _PALACE_FILES and r in (
                _RED_PALACE_RANKS if red else _BLACK_PALACE_RANKS
            ):
                add(f, r)
    elif kind == "B":
        for df, dr in ((2, 2), (2, -2), (-2, 2), (-2, -2)):
            eye_f, eye_r = file + df // 2, rank + dr // 2
            f, r = file + df, rank + dr
            if not _in_board(f, r):
                continue
            if board[eye_r][eye_f] is not None:
                continue
            if red and r > 4:
                continue
            if not red and r < 5:
                continue
            add(f, r)
    elif kind == "N":
        for block, dests in (
            ((0, 1), ((-1, 2), (1, 2))),
            ((0, -1), ((-1, -2), (1, -2))),
            ((1, 0), ((2, -1), (2, 1))),
            ((-1, 0), ((-2, -1), (-2, 1))),
        ):
            bf, br = file + block[0], rank + block[1]
            if not _in_board(bf, br) or board[br][bf] is not None:
                continue
            for df, dr in dests:
                add(file + df, rank + dr)
    elif kind == "R":
        for df, dr in ((1, 0), (-1, 0), (0, 1), (0, -1)):
            f, r = file + df, rank + dr
            while _in_board(f, r):
                hits.append((f, r))
                if board[r][f] is not None:
                    break
                f += df
                r += dr
    elif kind == "C":
        for df, dr in ((1, 0), (-1, 0), (0, 1), (0, -1)):
            f, r = file + df, rank + dr
            while _in_board(f, r) and board[r][f] is None:
                hits.append((f, r))
                f += df
                r += dr
            if not _in_board(f, r):
                continue
            # 炮架
            f += df
            r += dr
            while _in_board(f, r):
                if board[r][f] is not None:
                    hits.append((f, r))
                    break
                f += df
                r += dr
    elif kind == "P":
        forward = 1 if red else -1
        add(file, rank + forward)
        crossed = (red and rank >= 5) or ((not red) and rank <= 4)
        if crossed:
            add(file - 1, rank)
            add(file + 1, rank)
    return [(f, r) for f, r in hits if _in_board(f, r)]


def _is_face_to_face(board: tuple[tuple[str | None, ...], ...]) -> bool:
    rf, rr = _find_king(board, True)
    bf, br = _find_king(board, False)
    if rf != bf:
        return False
    lo, hi = sorted((rr, br))
    for r in range(lo + 1, hi):
        if board[r][rf] is not None:
            return False
    return True


def _in_check(board: tuple[tuple[str | None, ...], ...], red_to_move_side_checked: bool) -> bool:
    """red_to_move_side_checked=True 表示检查红方是否被将。"""
    if _is_face_to_face(board):
        return True
    kf, kr = _find_king(board, red_to_move_side_checked)
    for rank in range(10):
        for file in range(9):
            piece = board[rank][file]
            if piece is None:
                continue
            if _is_red(piece) == red_to_move_side_checked:
                continue
            for af, ar in _piece_attacks(board, file, rank, piece):
                if af == kf and ar == kr:
                    # 炮攻击列表含走子与吃子目标；对将需要确实是可吃到
                    if piece.upper() == "C":
                        # 攻击生成已保证隔一子
                        return True
                    if piece.upper() == "R":
                        return True
                    if piece.upper() in ("K", "A", "B", "N", "P"):
                        return True
                    return True
    return False


def _apply_move_board(
    board: tuple[tuple[str | None, ...], ...],
    move: str,
) -> tuple[tuple[str | None, ...], ...]:
    ff, fr, tf, tr = _parse_uci(move)
    grid = [list(row) for row in board]
    piece = grid[fr][ff]
    if piece is None:
        raise ValueError(f"着法起点无子：{move}")
    grid[fr][ff] = None
    grid[tr][tf] = piece
    return tuple(tuple(r) for r in grid)


def make_move(pos: Position, move: str) -> Position:
    board = _apply_move_board(pos.board, move)
    side = "b" if pos.side == "w" else "w"
    return Position(board, side)


def generate_legal_moves(pos: Position) -> list[str]:
    red = pos.side == "w"
    moves: list[str] = []
    for rank in range(10):
        for file in range(9):
            piece = pos.board[rank][file]
            if piece is None or _is_red(piece) != red:
                continue
            for tf, tr in _piece_attacks(pos.board, file, rank, piece):
                target = pos.board[tr][tf]
                if target is not None and _same_side(piece, target):
                    continue
                # 炮的非吃子在 attacks 里已是空位；吃子目标为对方子
                if piece.upper() == "C" and target is not None and _same_side(piece, target):
                    continue
                move = _sq_uci(file, rank) + _sq_uci(tf, tr)
                try:
                    nxt = _apply_move_board(pos.board, move)
                except ValueError:
                    continue
                if _in_check(nxt, red):
                    continue
                moves.append(move)
    return moves


# ---------------------------------------------------------------------------
# SQLite
# ---------------------------------------------------------------------------


class OpenBookDB:
    def __init__(self, path: Path = OPENBOOK_PATH) -> None:
        self.path = path
        path.parent.mkdir(parents=True, exist_ok=True)
        self.conn = sqlite3.connect(str(path))
        self.conn.execute("PRAGMA journal_mode=WAL")
        self.conn.execute("PRAGMA synchronous=NORMAL")
        self._init_schema()

    def _init_schema(self) -> None:
        self.conn.executescript(
            """
            CREATE TABLE IF NOT EXISTS meta (
                key   TEXT PRIMARY KEY,
                value TEXT NOT NULL
            );
            CREATE TABLE IF NOT EXISTS positions (
                fen       TEXT PRIMARY KEY,
                ply       INTEGER NOT NULL,
                bestmove  TEXT,
                score     TEXT,
                expanded  INTEGER NOT NULL DEFAULT 0
            );
            CREATE TABLE IF NOT EXISTS edges (
                parent_fen TEXT NOT NULL,
                move       TEXT NOT NULL,
                child_fen  TEXT NOT NULL,
                PRIMARY KEY (parent_fen, move)
            );
            CREATE INDEX IF NOT EXISTS idx_positions_ply ON positions(ply);
            CREATE INDEX IF NOT EXISTS idx_edges_parent ON edges(parent_fen);
            """
        )
        self.conn.commit()

    def get_meta(self, key: str, default: str | None = None) -> str | None:
        row = self.conn.execute(
            "SELECT value FROM meta WHERE key=?", (key,)
        ).fetchone()
        return row[0] if row else default

    def set_meta(self, key: str, value: str) -> None:
        self.conn.execute(
            "INSERT INTO meta(key, value) VALUES(?, ?) "
            "ON CONFLICT(key) DO UPDATE SET value=excluded.value",
            (key, value),
        )
        self.conn.commit()

    def get_position(self, fen: str) -> sqlite3.Row | None:
        self.conn.row_factory = sqlite3.Row
        row = self.conn.execute(
            "SELECT * FROM positions WHERE fen=?", (fen,)
        ).fetchone()
        self.conn.row_factory = None
        return row

    def upsert_position(
        self,
        fen: str,
        ply: int,
        bestmove: str,
        score: str,
        expanded: int = 0,
    ) -> None:
        self.conn.execute(
            """
            INSERT INTO positions(fen, ply, bestmove, score, expanded)
            VALUES(?, ?, ?, ?, ?)
            ON CONFLICT(fen) DO UPDATE SET
                ply=excluded.ply,
                bestmove=excluded.bestmove,
                score=excluded.score,
                expanded=excluded.expanded
            """,
            (fen, ply, bestmove, score, expanded),
        )
        self.conn.commit()

    def mark_expanded(self, fen: str) -> None:
        self.conn.execute(
            "UPDATE positions SET expanded=1 WHERE fen=?", (fen,)
        )
        self.conn.commit()

    def add_edge(self, parent: str, move: str, child: str) -> None:
        self.conn.execute(
            """
            INSERT OR IGNORE INTO edges(parent_fen, move, child_fen)
            VALUES(?, ?, ?)
            """,
            (parent, move, child),
        )
        self.conn.commit()

    def get_edges(self, parent: str) -> list[tuple[str, str]]:
        rows = self.conn.execute(
            "SELECT move, child_fen FROM edges WHERE parent_fen=?",
            (parent,),
        ).fetchall()
        return [(r[0], r[1]) for r in rows]

    def count_positions(self) -> int:
        return int(
            self.conn.execute("SELECT COUNT(*) FROM positions").fetchone()[0]
        )

    def depth_histogram(self) -> dict[int, int]:
        rows = self.conn.execute(
            "SELECT ply, COUNT(*) FROM positions GROUP BY ply ORDER BY ply"
        ).fetchall()
        return {int(p): int(c) for p, c in rows}

    def unexpanded(self, max_ply: int) -> list[tuple[str, int]]:
        rows = self.conn.execute(
            """
            SELECT fen, ply FROM positions
            WHERE expanded=0 AND ply<=?
            ORDER BY ply ASC
            """,
            (max_ply,),
        ).fetchall()
        return [(r[0], int(r[1])) for r in rows]

    def prepare_resume(self, target_depth: int) -> None:
        """加深训练时，把仍需扩展的节点重新标为未扩展。"""
        self.conn.execute(
            "UPDATE positions SET expanded=0 WHERE ply < ?",
            (target_depth,),
        )
        self.conn.commit()

    def close(self) -> None:
        self.conn.close()


# ---------------------------------------------------------------------------
# 训练（多进程）
# ---------------------------------------------------------------------------

_WORKER_ENGINE: PikafishEngine | None = None
_WORKER_DEPTH = MAX_DEPTH


def _init_worker(engine_depth: int) -> None:
    global _WORKER_ENGINE, _WORKER_DEPTH
    _WORKER_DEPTH = engine_depth
    _WORKER_ENGINE = PikafishEngine(max_depth=engine_depth)


def _shutdown_worker() -> None:
    global _WORKER_ENGINE
    if _WORKER_ENGINE is not None:
        _WORKER_ENGINE.close()
        _WORKER_ENGINE = None


def _worker_search(task: tuple[str, int, bool, int]) -> dict:
    """task = (fen, ply, use_multipv, multipv_n)"""
    fen, ply, use_multipv, multipv_n = task
    assert _WORKER_ENGINE is not None
    if use_multipv:
        lines = _WORKER_ENGINE.search_multipv(fen, multipv_n)
        bestmove, score = lines[0]
        branch = [m for m, _ in lines]
    else:
        bestmove, score = _WORKER_ENGINE.search(fen)
        branch = generate_legal_moves(Position.from_fen(fen))
    return {
        "fen": fen,
        "ply": ply,
        "bestmove": bestmove,
        "score": score,
        "branch_moves": branch,
    }


def _worker_count() -> int:
    """工作进程数：按大致物理核估算，并留 2 核给系统。

    ``os.cpu_count()`` 在 Windows 上通常是逻辑处理器（含超线程）。
    象棋引擎对超线程收益很差，进程过多只会加剧调度争用。
    """
    logical = os.cpu_count() or 2
    physical = max(1, logical // 2)
    return max(1, physical - 2)


def train(target_depth: int, engine_depth: int = MAX_DEPTH) -> None:
    if target_depth < 0:
        raise ValueError("训练深度必须 >= 0")

    _log(f"训练目标开局深度：{target_depth}  ply；皮卡鱼深度：{engine_depth}")
    _log(f"剪枝起点 ply={PRUNING_BEG_PLY}，剪枝保留前 {BEST_MOVES_NUM} 着")
    workers = _worker_count()
    _log(f"工作进程数：{workers}（约 物理核-2）")

    Pikafish.set_max_depth(engine_depth)
    db = OpenBookDB(OPENBOOK_PATH)
    db.set_meta("engine_depth", str(engine_depth))
    db.set_meta("target_depth", str(target_depth))
    db.prepare_resume(target_depth)

    root, _ = canonicalize(INIT_FEN)
    if db.get_position(root) is None and target_depth >= 0:
        # 先放入队列，由 worker 搜索根局面
        pass

    pending: dict[str, int] = {}
    for fen, ply in db.unexpanded(target_depth):
        pending[fen] = ply
    if root not in pending and db.get_position(root) is None:
        pending[root] = 0
    elif root not in pending:
        # 根已扩展完且无需加深
        row = db.get_position(root)
        if row is not None and int(row["expanded"]) == 0:
            pending[root] = int(row["ply"])

    # 若库非空但全部已扩展且目标未加深，可能无任务
    if not pending:
        # 仍检查是否有 ply < target 却缺边的叶子
        db.prepare_resume(target_depth)
        for fen, ply in db.unexpanded(target_depth):
            pending[fen] = ply

    if not pending:
        _log("没有待训练局面（可能已达到目标深度）。")
        db.close()
        return

    done_count = db.count_positions()
    bar = _ProgressBar("训练开局库", None)
    bar.update(done_count, suffix=f"队列 {len(pending)}")

    try:
        with ProcessPoolExecutor(
            max_workers=workers,
            initializer=_init_worker,
            initargs=(engine_depth,),
        ) as pool:
            while pending:
                # 本轮取一批（控制内存与进度刷新）
                batch_items = list(pending.items())
                batch_items.sort(key=lambda x: x[1])
                batch = batch_items[: max(workers * 2, 4)]
                for fen, _ply in batch:
                    del pending[fen]

                tasks: list[tuple[str, int, bool, int]] = []
                skip_expand: list[tuple[str, int]] = []

                for fen, ply in batch:
                    row = db.get_position(fen)
                    if row is not None and row["bestmove"]:
                        # 已有搜索结果：只需补边/展开
                        skip_expand.append((fen, ply))
                        continue
                    use_multipv = ply >= PRUNING_BEG_PLY
                    tasks.append(
                        (fen, ply, use_multipv, BEST_MOVES_NUM)
                    )

                futures = {
                    pool.submit(_worker_search, t): t for t in tasks
                }
                search_results: list[dict] = []
                for fut in as_completed(futures):
                    search_results.append(fut.result())

                # 处理新搜索
                for result in search_results:
                    fen = result["fen"]
                    ply = int(result["ply"])
                    db.upsert_position(
                        fen,
                        ply,
                        result["bestmove"],
                        result["score"],
                        expanded=0,
                    )
                    _expand_position(
                        db,
                        fen,
                        ply,
                        target_depth,
                        result["branch_moves"],
                        pending,
                    )

                # 处理仅展开
                for fen, ply in skip_expand:
                    edges = db.get_edges(fen)
                    if edges:
                        branch_moves = [m for m, _ in edges]
                    else:
                        if ply >= PRUNING_BEG_PLY:
                            # 需要重新 MultiPV 才能知道剪枝分支
                            result = _worker_search_local(
                                fen, ply, True, BEST_MOVES_NUM, engine_depth
                            )
                            db.upsert_position(
                                fen,
                                ply,
                                result["bestmove"],
                                result["score"],
                                expanded=0,
                            )
                            branch_moves = result["branch_moves"]
                        else:
                            branch_moves = generate_legal_moves(
                                Position.from_fen(fen)
                            )
                    _expand_position(
                        db, fen, ply, target_depth, branch_moves, pending
                    )

                done_count = db.count_positions()
                bar.update(
                    done_count,
                    suffix=f"队列 {len(pending)}",
                )
    finally:
        bar.finish(suffix=f"局面 {db.count_positions()}")
        db.set_meta("last_finished_target", str(target_depth))
        db.close()
        _log(f"训练结束，数据库：{OPENBOOK_PATH}")


def _worker_search_local(
    fen: str,
    ply: int,
    use_multipv: bool,
    multipv_n: int,
    engine_depth: int,
) -> dict:
    """主进程内临时搜索（补边用，较少调用）。"""
    with PikafishEngine(max_depth=engine_depth) as engine:
        if use_multipv:
            lines = engine.search_multipv(fen, multipv_n)
            bestmove, score = lines[0]
            branch = [m for m, _ in lines]
        else:
            bestmove, score = engine.search(fen)
            branch = generate_legal_moves(Position.from_fen(fen))
    return {
        "fen": fen,
        "ply": ply,
        "bestmove": bestmove,
        "score": score,
        "branch_moves": branch,
    }


def _expand_position(
    db: OpenBookDB,
    fen: str,
    ply: int,
    target_depth: int,
    branch_moves: Iterable[str],
    pending: dict[str, int],
) -> None:
    if ply >= target_depth:
        db.mark_expanded(fen)
        return

    pos = Position.from_fen(fen)
    existing = {m: c for m, c in db.get_edges(fen)}
    for move in branch_moves:
        if move in existing:
            child = existing[move]
        else:
            try:
                child_pos = make_move(pos, move)
            except Exception:
                continue
            child, _ = canonicalize(child_pos.key())
            # 着法存规范局面坐标系：若 child 相对 child_pos 镜像，边仍记录实际走出的 move
            # 子节点用规范 FEN；从当前规范 fen 出发的 move 本身已在规范坐标
            db.add_edge(fen, move, child)
        child_ply = ply + 1
        if child_ply <= target_depth:
            row = db.get_position(child)
            if row is None or int(row["expanded"]) == 0:
                # 尚未完全处理的子节点入队
                if child not in pending:
                    pending[child] = child_ply
            elif int(row["ply"]) > child_ply:
                pass
    db.mark_expanded(fen)


# ---------------------------------------------------------------------------
# CLI 子命令
# ---------------------------------------------------------------------------


def cmd_info() -> int:
    if not OPENBOOK_PATH.is_file():
        _log(f"开局库不存在：{OPENBOOK_PATH}")
        return 1
    db = OpenBookDB(OPENBOOK_PATH)
    try:
        total = db.count_positions()
        hist = db.depth_histogram()
        _log(f"路径：{OPENBOOK_PATH}")
        _log(f"局面数：{total}")
        _log(f"引擎深度：{db.get_meta('engine_depth', '未知')}")
        _log(f"上次目标 ply：{db.get_meta('last_finished_target', db.get_meta('target_depth', '未知'))}")
        _log("深度(ply) -> 局面数：")
        if not hist:
            _log("  （空）")
        else:
            for ply, count in hist.items():
                _log(f"  {ply}: {count}")
    finally:
        db.close()
    return 0


def cmd_fen(fen_str: str) -> int:
    if not OPENBOOK_PATH.is_file():
        _log(f"开局库不存在：{OPENBOOK_PATH}")
        return 1
    try:
        can, flipped = canonicalize(fen_str)
    except ValueError as exc:
        _log(str(exc))
        return 2

    db = OpenBookDB(OPENBOOK_PATH)
    try:
        row = db.get_position(can)
        if row is None:
            _log("未命中开局库。")
            _log(f"规范 FEN：{can}")
            return 1
        move = row["bestmove"] or ""
        score = row["score"] or ""
        if flipped and move:
            move = mirror_move(move)
        query_fen = fen_str.strip()
        _log("命中开局库。")
        _log(f"查询 FEN：{query_fen}")
        _log(f"规范 FEN：{can}")
        _log(f"ply：{row['ply']}")
        if move:
            _log(f"推荐着法：{format_move(query_fen, move)}")
        else:
            _log("推荐着法：（无）")
        _log(f"score：{score}")
        _log(f"expanded：{row['expanded']}")
        edges = db.get_edges(can)
        if edges:
            _log(f"分支数：{len(edges)}")
            parent_pos = Position.from_fen(can)
            query_pos = Position.from_fen(query_fen)
            for m, child in edges[:20]:
                out_m = mirror_move(m) if flipped else m
                try:
                    if flipped:
                        shown = make_move(query_pos, out_m).key()
                    else:
                        shown = make_move(parent_pos, m).key()
                except Exception:
                    shown = child
                _log(f"  {format_move(query_fen, out_m)} -> {shown}")
            if len(edges) > 20:
                _log(f"  … 另有 {len(edges) - 20} 条")
    finally:
        db.close()
    return 0


def cmd_train(depth: int) -> int:
    _log("=" * 60)
    _log("开始开局库训练 / 断点续训")
    _log("=" * 60)
    train(target_depth=depth, engine_depth=MAX_DEPTH)
    return 0


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        prog="python -m kits.openbook",
        description="皮卡鱼开局库蒸馏与查询",
    )
    parser.add_argument(
        "--train",
        type=int,
        metavar="DEPTH",
        help="训练或续训到指定开局 ply 深度",
    )
    parser.add_argument(
        "--fen",
        type=str,
        metavar="FEN",
        help="查询局面是否在开局库中",
    )
    parser.add_argument(
        "--info",
        action="store_true",
        help="显示开局库统计信息",
    )
    return parser


def main(argv: list[str] | None = None) -> int:
    parser = build_parser()
    args = parser.parse_args(argv)

    selected = sum(
        [
            args.train is not None,
            bool(args.fen),
            bool(args.info),
        ]
    )
    if selected == 0:
        parser.print_help()
        _log("")
        _log("可用命令：")
        _log("  python -m kits.openbook --train <depth>")
        _log("  python -m kits.openbook --fen <fen_str>")
        _log("  python -m kits.openbook --info")
        return 0
    if selected > 1:
        _log("请一次只使用一个命令选项。")
        return 2

    if args.info:
        return cmd_info()
    if args.fen is not None:
        return cmd_fen(args.fen)
    assert args.train is not None
    return cmd_train(args.train)


if __name__ == "__main__":
    raise SystemExit(main())
