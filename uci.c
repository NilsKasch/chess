#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "data_rep.h"
#include "engine.h"
#include "fen.h"
#include "uci.h"

int g_uci_mode = 0;

void move_to_uci(Move *move, Piece *pieces, char *buf) {
    int fx = pieces[move->piece].x;
    int fy = pieces[move->piece].y;
    int tx = fx + move->x;
    int ty = fy + move->y;
    buf[0] = 'a' + fx;
    buf[1] = '1' + fy;
    buf[2] = 'a' + tx;
    buf[3] = '1' + ty;
    buf[4] = '\0';
    if (move->transform) {
        buf[4] = move->transform + 32;
        buf[5] = '\0';
    }
}

static void handle_position(const char *line, Piece *pieces, int *grid, Board *board, short *white) {
    const char *p = line + 9;
    while (*p == ' ') p++;

    const char *moves_pos = strstr(p, " moves");
    int has_moves = (moves_pos != NULL);

    if (strncmp(p, "startpos", 8) == 0) {
        parse_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
                  pieces, grid, board, white);
    } else if (strncmp(p, "fen", 3) == 0) {
        const char *fen_start = p + 3;
        while (*fen_start == ' ') fen_start++;
        int fen_len;
        if (has_moves) {
            fen_len = moves_pos - fen_start;
        } else {
            fen_len = strlen(fen_start);
        }
        while (fen_len > 0 && fen_start[fen_len - 1] == ' ') fen_len--;
        char fen_buf[256];
        int copy_len = fen_len < 255 ? fen_len : 255;
        strncpy(fen_buf, fen_start, copy_len);
        fen_buf[copy_len] = '\0';
        parse_fen(fen_buf, pieces, grid, board, white);
    }

    if (has_moves) {
        const char *m = moves_pos + 6;
        while (*m == ' ') m++;

        char moves_copy[1024];
        strncpy(moves_copy, m, sizeof(moves_copy) - 1);
        moves_copy[sizeof(moves_copy) - 1] = '\0';

        char *token = strtok(moves_copy, " ");
        while (token) {
            Move possible[138];
            int fill = 0;
            possible_moves(*white, pieces, grid, board, possible, &fill);

            int found = 0;
            Move matched = {};
            for (int i = 0; i < fill; i++) {
                char uci_buf[6];
                move_to_uci(&possible[i], pieces, uci_buf);
                if (strcmp(uci_buf, token) == 0) {
                    matched = possible[i];
                    found = 1;
                    break;
                }
            }

            if (found) {
                Piece undo = {};
                apply_move(pieces, grid, board, &matched, &undo);
                *white = -*white;
            }

            token = strtok(NULL, " ");
        }
    }
}

static void handle_go(const char *line, short white, Piece *pieces, int *grid, Board *board) {
    int depth = 5;
    if (sscanf(line, "go depth %d", &depth) == 1) {
    } else if (strstr(line, "go movetime") != NULL || strstr(line, "go wtime") != NULL) {
        depth = 5;
    } else if (strstr(line, "go infinite") != NULL) {
        depth = 5;
    } else if (strstr(line, "go perft") != NULL) {
        printf("bestmove 0000\n");
        return;
    }

    Move best = rnd_best_move(white, pieces, grid, board, depth);

    if (best.x == 0 && best.y == 0) {
        printf("bestmove 0000\n");
    } else {
        char uci_move[6];
        move_to_uci(&best, pieces, uci_move);
        printf("bestmove %s\n", uci_move);
    }
}

void uci_loop(void) {
    g_uci_mode = 1;
    setbuf(stdin, NULL);
    setbuf(stdout, NULL);

    Piece pieces[32];
    Board board;
    int grid[64];
    short white = 1;

    parse_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
              pieces, grid, &board, &white);
    white = 1;

    char line[4096];
    while (fgets(line, sizeof(line), stdin)) {
        line[strcspn(line, "\n")] = 0;

        if (strcmp(line, "uci") == 0) {
            printf("id name ChessEngine\n");
            printf("id author Nils\n");
            printf("uciok\n");
            fflush(stdout);
        } else if (strcmp(line, "isready") == 0) {
            printf("readyok\n");
            fflush(stdout);
        } else if (strcmp(line, "ucinewgame") == 0) {
            parse_fen("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
                      pieces, grid, &board, &white);
            white = 1;
        } else if (strncmp(line, "position", 8) == 0) {
            handle_position(line, pieces, grid, &board, &white);
        } else if (strncmp(line, "go", 2) == 0) {
            handle_go(line, white, pieces, grid, &board);
            fflush(stdout);
        } else if (strcmp(line, "stop") == 0) {
        } else if (strcmp(line, "quit") == 0) {
            break;
        }
    }
}
