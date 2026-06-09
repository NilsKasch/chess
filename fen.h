#ifndef FEN_H
#define FEN_H

#include "data_rep.h"

int parse_fen(const char *fen, Piece *pieces, int *grid, Board *board, short *white);

#endif
