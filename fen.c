#include <string.h>
#include <ctype.h>
#include "fen.h"

static int char_to_value(char c) {
    switch (tolower(c)) {
        case 'p': return 1;
        case 'n': case 'b': return 3;
        case 'r': return 5;
        case 'q': return 9;
        case 'k': return 1000;
        default: return 0;
    }
}

static char fen_to_internal(char c) {
    char lower = tolower(c);
    if (lower == 'p') return 'p';
    return toupper(c);
}

int parse_fen(const char *fen, Piece *pieces, int *grid, Board *board, short *white) {
    for (int i = 0; i < 32; i++) {
        pieces[i].value = 0;
        pieces[i].txt = 0;
    }
    for (int i = 0; i < 64; i++) {
        grid[i] = 32;
    }

    char fen_copy[256];
    strncpy(fen_copy, fen, sizeof(fen_copy) - 1);
    fen_copy[sizeof(fen_copy) - 1] = '\0';

    char *fields[6];
    int field_count = 0;
    char *token = strtok(fen_copy, " ");
    while (token && field_count < 6) {
        fields[field_count++] = token;
        token = strtok(NULL, " ");
    }
    if (field_count < 4) return -1;

    int x = 0, y = 7;
    int white_idx = 0, black_idx = 16;
    const char *board_str = fields[0];

    for (int i = 0; board_str[i] && y >= 0; i++) {
        char c = board_str[i];
        if (c == '/') {
            y--;
            x = 0;
        } else if (c >= '1' && c <= '8') {
            x += c - '0';
        } else {
            int is_white = isupper(c) ? 1 : 0;
            int idx;
            if (is_white) {
                if (white_idx >= 16) return -2;
                idx = white_idx++;
            } else {
                if (black_idx >= 32) return -3;
                idx = black_idx++;
            }
            pieces[idx].txt = fen_to_internal(c);
            pieces[idx].value = char_to_value(c);
            pieces[idx].x = x;
            pieces[idx].y = y;
            grid[x + y * 8] = idx;
            x++;
        }
    }

    if (fields[1][0] == 'w') *white = 1;
    else *white = -1;

    board->castle_rights = 0;
    if (strcmp(fields[2], "-") != 0) {
        for (int i = 0; fields[2][i]; i++) {
            char c = fields[2][i];
            if (c == 'K') board->castle_rights |= WHITE_CASLTE_RIGHT;
            else if (c == 'Q') board->castle_rights |= WHITE_CASLTE_LEFT;
            else if (c == 'k') board->castle_rights |= BLACK_CASLTE_RIGHT;
            else if (c == 'q') board->castle_rights |= BLACK_CASLTE_LEFT;
        }
    }

    board->en_passant = 0;
    if (strcmp(fields[3], "-") != 0) {
        int fx = fields[3][0] - 'a';
        int fy = fields[3][1] - '1';
        int py;
        if (*white == 1) {
            py = fy - 1;
        } else {
            py = fy + 1;
        }
        if (py >= 0 && py < 8) {
            int pawn_idx = grid[fx + py * 8];
            if (pawn_idx >= 0 && pawn_idx < 32 && pieces[pawn_idx].value > 0 && pieces[pawn_idx].txt == 'p') {
                board->en_passant = pawn_idx;
            }
        }
    }

    return 0;
}
