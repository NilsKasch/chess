import chess
import chess.engine
import subprocess
import time
import sys

ENGINE = "./chess"


def test_raw_uci_handshake():
    with subprocess.Popen([ENGINE], stdin=subprocess.PIPE, stdout=subprocess.PIPE,
                          stderr=subprocess.PIPE, text=True, bufsize=1) as proc:
        proc.stdin.write("uci\n")
        proc.stdin.flush()
        lines = []
        while True:
            line = proc.stdout.readline().strip()
            if not line:
                continue
            lines.append(line)
            if line == "uciok":
                break
        assert any("id name" in l for l in lines), "Missing id name"
        assert "uciok" in lines, "Missing uciok"
        proc.stdin.write("quit\n")
        proc.stdin.flush()
        proc.wait(timeout=2)


def test_raw_checkmate():
    board = chess.Board()
    board.push_uci("f2f3")
    board.push_uci("e7e5")
    board.push_uci("g2g4")
    board.push_uci("d8h4")

    with subprocess.Popen([ENGINE], stdin=subprocess.PIPE, stdout=subprocess.PIPE,
                          stderr=subprocess.PIPE, text=True, bufsize=1) as proc:
        proc.stdin.write("uci\n")
        proc.stdin.flush()
        while proc.stdout.readline().strip() != "uciok":
            pass
        proc.stdin.write(f"position fen {board.fen()}\n")
        proc.stdin.flush()
        proc.stdin.write("go depth 2\n")
        proc.stdin.flush()
        line = proc.stdout.readline().strip()
        assert line.startswith("bestmove"), f"Expected bestmove, got: {line}"
        proc.stdin.write("quit\n")
        proc.stdin.flush()
        proc.wait(timeout=2)


def test_highlevel_startpos():
    with chess.engine.SimpleEngine.popen_uci(ENGINE) as engine:
        board = chess.Board()
        result = engine.play(board, chess.engine.Limit(depth=2))
        assert result.move is not None
        assert result.move in board.legal_moves


def test_highlevel_after_moves():
    with chess.engine.SimpleEngine.popen_uci(ENGINE) as engine:
        board = chess.Board()
        board.push_uci("e2e4")
        result = engine.play(board, chess.engine.Limit(depth=2))
        assert result.move is not None
        assert result.move in board.legal_moves


def test_highlevel_fen_setup():
    with chess.engine.SimpleEngine.popen_uci(ENGINE) as engine:
        fen = "r1bqkb1r/pppp1ppp/2n2n2/4p3/2B1P3/5N2/PPPP1PPP/RNBQK2R w KQkq - 0 1"
        board = chess.Board(fen)
        result = engine.play(board, chess.engine.Limit(depth=2))
        assert result.move is not None
        assert result.move in board.legal_moves


def test_highlevel_multiple_searches():
    with chess.engine.SimpleEngine.popen_uci(ENGINE) as engine:
        board = chess.Board()
        for _ in range(3):
            result = engine.play(board, chess.engine.Limit(depth=1))
            assert result.move is not None
            assert result.move in board.legal_moves
            board.push(result.move)


def test_cli_mode():
    result = subprocess.run([ENGINE, "2", "1"], capture_output=True, text=True, timeout=5)
    assert "steps: 2" in result.stdout
    assert "depth: 1" in result.stdout
    assert "start" in result.stdout
    assert "value:" in result.stdout


def test_highlevel_position_moves_sequence():
    with chess.engine.SimpleEngine.popen_uci(ENGINE) as engine:
        board = chess.Board()
        board.push_uci("e2e4")
        board.push_uci("e7e5")
        board.push_uci("g1f3")
        result = engine.play(board, chess.engine.Limit(depth=2))
        assert result.move is not None
        assert result.move in board.legal_moves


if __name__ == "__main__":
    tests = [fn for fn in globals() if fn.startswith("test_")]
    passed = 0
    failed = 0
    for name in tests:
        sys.stdout.write(f"{name} ... ")
        sys.stdout.flush()
        try:
            globals()[name]()
            sys.stdout.write("PASS\n")
            passed += 1
        except Exception as e:
            sys.stdout.write(f"FAIL ({e})\n")
            failed += 1
    sys.stdout.write(f"\n{passed}/{passed+failed} passed\n")
    sys.exit(0 if failed == 0 else 1)
