#ifndef ENGINE_H
#define ENGINE_H

extern int g_uci_mode;

void update_grid(Piece *pieces, int grid[]);
int not_defended(int piece, Piece pieces[], int grid[], Move *move, short *white);
void apply_move(Piece *pieces, int grid[], Board *board, Move *move, Piece *undo_piece);
void undo_move(Piece *pieces, int grid[], Board *board, Move *move, Piece *undo_piece);
void possible_moves(short white, Piece *pieces, int grid[], Board *board, Move possible[], int *fill);
Move rnd_best_move(short white, Piece *pieces, int grid[], Board *board,  int depth);
float minimax_pv(Absolute_Move *pv, short white, Piece *pieces, int grid[], Board *board, float alpha, float beta, int depth, int max_depth, int *count);

#endif