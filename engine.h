#ifndef ENGINE_H
#define ENGINE_H

void update_grid(Piece *pieces, int grid[]);
int not_defended(int piece, Piece pieces[], int grid[], Move *move, short *white);
void apply_move(Piece *pieces, int grid[], Board *board, Move *move, Piece *undo_piece);
Move rnd_best_move(short white, Piece *pieces, int grid[], Board *board,  int depth);

#endif