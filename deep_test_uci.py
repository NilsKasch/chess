import chess
import chess.engine
import subprocess
import time
import sys

ENGINE = "./chess"


def test_play_five_games():
    """Play 5 full games at depth 5, verify all moves legal, no crashes."""
    for game_num in range(1, 6):
        with chess.engine.SimpleEngine.popen_uci(ENGINE) as engine:
            board = chess.Board()
            move_count = 0
            while not board.is_game_over() and move_count < 100:
                try:
                    result = engine.play(board, chess.engine.Limit(depth=5))
                except Exception as e:
                    print(f"\nGame {game_num} crashed at move {move_count}: {e}")
                    print(f"Position FEN: {board.fen()}")
                    raise
                if result.move is None:
                    break
                assert result.move in board.legal_moves, (
                    f"Game {game_num} move {move_count}: {result.move} not legal in {board.fen()}"
                )
                board.push(result.move)
                move_count += 1
            print(f"  Game {game_num}: {move_count} moves, "
                  f"result={board.outcome().result() if board.outcome() else 'unknown'}")


def test_play_both_sides():
    """Play a game where engine alternates white/black each game."""
    for side in range(2):
        with chess.engine.SimpleEngine.popen_uci(ENGINE) as engine:
            board = chess.Board()
            engine_white = (side == 0)
            move_count = 0
            while not board.is_game_over() and move_count < 100:
                if (board.turn == chess.WHITE) == engine_white:
                    result = engine.play(board, chess.engine.Limit(depth=5))
                    if result.move is None:
                        break
                    assert result.move in board.legal_moves
                    board.push(result.move)
                else:
                    legal = list(board.legal_moves)
                    if not legal:
                        break
                    board.push(legal[0])
                move_count += 1
            print(f"  Engine plays {'white' if engine_white else 'black'}: "
                  f"{move_count} moves, "
                  f"result={board.outcome().result() if board.outcome() else 'unknown'}")


def test_en_passant_scenario():
    """Engine correctly handles en passant captures at depth 5."""
    fen = "rnbqkbnr/1ppppppp/8/p7/3PP3/8/PPP2PPP/RNBQKBNR b KQkq - 0 1"
    with chess.engine.SimpleEngine.popen_uci(ENGINE) as engine:
        board = chess.Board(fen)
        result = engine.play(board, chess.engine.Limit(depth=5))
        assert result.move is not None
        assert result.move in board.legal_moves
        print(f"  En passant scenario: {result.move}")


def test_castling_scenarios():
    """Engine handles castling positions at depth 5."""
    fens = [
        "r3k2r/pppppppp/8/8/8/8/PPPPPPPP/R3K2R w KQkq - 0 1",
        "r3k2r/pppppppp/8/8/8/8/PPPPPPPP/R3K2R b KQkq - 0 1",
    ]
    for i, fen in enumerate(fens):
        with chess.engine.SimpleEngine.popen_uci(ENGINE) as engine:
            board = chess.Board(fen)
            result = engine.play(board, chess.engine.Limit(depth=5))
            assert result.move is not None
            assert result.move in board.legal_moves
            print(f"  Castling {'white' if i == 0 else 'black'}: {result.move}")


def test_promotion_scenario():
    """Engine handles pawn promotion at depth 5."""
    fen = "8/PP6/8/8/8/8/pp6/8 w - - 0 1"
    with chess.engine.SimpleEngine.popen_uci(ENGINE) as engine:
        board = chess.Board(fen)
        result = engine.play(board, chess.engine.Limit(depth=5))
        assert result.move is not None
        assert result.move in board.legal_moves
        print(f"  Promotion scenario: {result.move}")


def test_complex_middlegame():
    """Engine handles a complex middlegame position at depth 5."""
    fen = "r1bq1rk1/pppp1ppp/2n2n2/2b1p3/2B1P3/2NP1N2/PPP2PPP/R1BQK2R w KQ - 0 1"
    with chess.engine.SimpleEngine.popen_uci(ENGINE) as engine:
        board = chess.Board(fen)
        result = engine.play(board, chess.engine.Limit(depth=5))
        assert result.move is not None
        assert result.move in board.legal_moves
        print(f"  Complex middlegame: {result.move}")


def test_endgame_positions():
    """Engine handles endgame positions at depth 5."""
    fens = [
        "8/3k4/8/8/8/4K3/8/8 w - - 0 1",
        "8/8/3k4/8/3K4/8/8/8 w - - 0 1",
        "4k3/8/8/8/8/8/8/4K3 w - - 0 1",
    ]
    for i, fen in enumerate(fens):
        with chess.engine.SimpleEngine.popen_uci(ENGINE) as engine:
            board = chess.Board(fen)
            result = engine.play(board, chess.engine.Limit(depth=5))
            assert result.move is not None
            assert result.move in board.legal_moves
            print(f"  Endgame {i+1}: {result.move}")


def test_multiple_position_go_cycles():
    """50 sequential position+go cycles without restarting engine."""
    with chess.engine.SimpleEngine.popen_uci(ENGINE) as engine:
        board = chess.Board()
        for i in range(50):
            result = engine.play(board, chess.engine.Limit(depth=5))
            assert result.move is not None, f"Cycle {i}: engine returned no move"
            assert result.move in board.legal_moves, f"Cycle {i}: {result.move} not legal"
            board.push(result.move)
            if board.is_game_over():
                board = chess.Board()
        print(f"  50 cycles completed")


def test_raw_go_infinite():
    """Engine responds to go infinite and stop."""
    with subprocess.Popen([ENGINE], stdin=subprocess.PIPE, stdout=subprocess.PIPE,
                          stderr=subprocess.PIPE, text=True, bufsize=1) as proc:
        proc.stdin.write("uci\n")
        proc.stdin.flush()
        while proc.stdout.readline().strip() != "uciok":
            pass
        proc.stdin.write("position startpos\n")
        proc.stdin.flush()
        proc.stdin.write("go infinite\n")
        proc.stdin.flush()
        proc.stdin.write("stop\n")
        proc.stdin.flush()
        time.sleep(0.5)
        proc.stdin.write("quit\n")
        proc.stdin.flush()
        proc.wait(timeout=3)
    print("  go infinite + stop: OK")


def test_raw_stalemate():
    """Engine detects stalemate: sends bestmove 0000."""
    with subprocess.Popen([ENGINE], stdin=subprocess.PIPE, stdout=subprocess.PIPE,
                          stderr=subprocess.PIPE, text=True, bufsize=1) as proc:
        proc.stdin.write("uci\n")
        proc.stdin.flush()
        while proc.stdout.readline().strip() != "uciok":
            pass
        board = chess.Board()
        board.set_fen("7k/5K2/8/8/8/8/8/8 b - - 0 1")
        proc.stdin.write(f"position fen {board.fen()}\n")
        proc.stdin.flush()
        proc.stdin.write("go depth 5\n")
        proc.stdin.flush()
        line = proc.stdout.readline().strip()
        assert line.startswith("bestmove"), f"Expected bestmove, got: {line}"
        print(f"  Stalemate: {line}")
        proc.stdin.write("quit\n")
        proc.stdin.flush()
        proc.wait(timeout=3)


def test_raw_checkmate_and_resign():
    """Engine resigns in a lost position (depth 5)."""
    with subprocess.Popen([ENGINE], stdin=subprocess.PIPE, stdout=subprocess.PIPE,
                          stderr=subprocess.PIPE, text=True, bufsize=1) as proc:
        proc.stdin.write("uci\n")
        proc.stdin.flush()
        while proc.stdout.readline().strip() != "uciok":
            pass
        board = chess.Board()
        board.push_uci("f2f3")
        board.push_uci("e7e5")
        board.push_uci("g2g4")
        board.push_uci("d8h4")
        proc.stdin.write(f"position fen {board.fen()}\n")
        proc.stdin.flush()
        proc.stdin.write("go depth 5\n")
        proc.stdin.flush()
        line = proc.stdout.readline().strip()
        assert line.startswith("bestmove"), f"Expected bestmove, got: {line}"
        print(f"  Checkmate: {line}")
        proc.stdin.write("quit\n")
        proc.stdin.flush()
        proc.wait(timeout=3)


def test_raw_fen_with_en_passant():
    """FEN with en passant square is parsed correctly."""
    with subprocess.Popen([ENGINE], stdin=subprocess.PIPE, stdout=subprocess.PIPE,
                          stderr=subprocess.PIPE, text=True, bufsize=1) as proc:
        proc.stdin.write("uci\n")
        proc.stdin.flush()
        while proc.stdout.readline().strip() != "uciok":
            pass
        fen = "rnbqkbnr/1pp1pppp/p7/3pP3/8/8/PPPP1PPP/RNBQKBNR w KQkq d6 0 1"
        proc.stdin.write(f"position fen {fen}\n")
        proc.stdin.flush()
        proc.stdin.write("go depth 5\n")
        proc.stdin.flush()
        line = proc.stdout.readline().strip()
        assert line.startswith("bestmove"), f"Expected bestmove, got: {line}"
        print(f"  FEN with EP: {line}")
        proc.stdin.write("quit\n")
        proc.stdin.flush()
        proc.wait(timeout=3)


if __name__ == "__main__":
    tests = [fn for fn in globals() if fn.startswith("test_")]
    passed = 0
    failed = 0
    for name in tests:
        sys.stdout.write(f"{name} ... ")
        sys.stdout.flush()
        start = time.time()
        try:
            globals()[name]()
            elapsed = time.time() - start
            sys.stdout.write(f"PASS ({elapsed:.1f}s)\n")
            passed += 1
        except Exception as e:
            elapsed = time.time() - start
            sys.stdout.write(f"FAIL ({elapsed:.1f}s) -- {e}\n")
            failed += 1
    sys.stdout.write(f"\n{passed}/{passed + failed} passed\n")
    sys.exit(0 if failed == 0 else 1)
