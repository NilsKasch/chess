# Agent Instructions - Chess Engine

## Commands
* **Build**: `make`
* **Clean**: `make clean`
* **Run**: `./chess <steps> <depth>` (e.g., `./chess 10 3`). Both parameters are required integers.

## Architecture & Data Representation
* **Piece Array Layout**:
  * White pieces are indices `0-15` (White King is `15`).
  * Black pieces are indices `16-31` (Black King is `31`).
  * Left and Right Rooks: White left (`8`), White right (`9`), Black left (`24`), Black right (`25`).
* **Grid**:
  * Flat 1D array of 64 elements (`x + y * 8`).
  * Empty cells are represented by `32` (not `0` or negative).
* **Side-to-Move**:
  * Turn is represented by `white` as `1` (White) or `-1` (Black).
  * The King's index for the active side is calculated using `23 - 8 * white`.

## Engine Quirks & Limitations
* **Undo State Hack**:
  * When executing `apply_move` and `undo_move`, the state of `castle_rights` is stored inside `undo_piece->y` and the target/captured piece index is stored inside `undo_piece->x`.
* **Zero Moves**:
  * `apply_move` and `undo_move` do not support null/stationary moves (`x = 0, y = 0`).
* **Unicode Symbology**:
  * White pieces (`i < 16`) print as filled (black) symbols (e.g., `♟`, `♚`).
  * Black pieces (`i >= 16`) print as hollow (white) symbols (e.g., `♙`, `♔`).
  * This paradox is intentional to display correctly on dark-themed terminals.
