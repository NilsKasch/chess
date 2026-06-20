#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "data_rep.h"
#include "engine.h"
#include "print.h"
#include "uci.h"

int main (int argc, char *argv[]){
    int n,d;
    srand(time(NULL));  // Seed
    if (argc == 1)
    {
        uci_loop();
        return 0;
    }
    else if (argc == 3)
    {
        char *end;
        n = strtol(argv[1], &end, 10);
        if (*end != '\0') {
            printf("Error: not a valid number\n");
            return 1;
        }
        d = strtol(argv[2], &end, 10);
        if (*end != '\0') {
            printf("Error: not a valid number\n");
            return 1;
        }
        //printf("steps: %s\n", argv[1]);
        //printf("depth: %s\n", argv[2]);
        printf("steps: %d\n", n);
        printf("depth: %d\n", d);
    }
    else
    {
        printf("Usage :\n");
        printf("%s <number of steps> <depth>\n", argv[0]);
        return 1;
    }

    Piece pieces[32] = {
        {'p',1,0,1},
        {'p',1,1,1},
        {'p',1,2,1},
        {'p',1,3,1},
        {'p',1,4,1},
        {'p',1,5,1},
        {'p',1,6,1},
        {'p',1,7,1},
        {'R',5,0,0},
        {'R',5,7,0},
        {'N',3,1,0},
        {'N',3,6,0},
        {'B',3,2,0},
        {'B',3,5,0},
        {'Q',9,3,0},
        {'K',1000,4,0},
        {'p',1,0,6},
        {'p',1,1,6},
        {'p',1,2,6},
        {'p',1,3,6},
        {'p',1,4,6},
        {'p',1,5,6},
        {'p',1,6,6},
        {'p',1,7,6},
        {'R',5,0,7},
        {'R',5,7,7},
        {'N',3,1,7},
        {'N',3,6,7},
        {'B',3,2,7},
        {'B',3,5,7},
        {'Q',9,3,7},
        {'K',1000,4,7}
    };

    Board board = {32,(WHITE_CASLTE_LEFT | WHITE_CASLTE_RIGHT | BLACK_CASLTE_LEFT | BLACK_CASLTE_RIGHT)};

    int grid[64]={};
    for (int i = 0; i < 64; i++) {
        grid[i]=32;
    }
    
    printf("start\n");
    //printf("eval: %f\n",eval(pieces));
    update_grid(pieces,grid);
    /*
    for (int i = 0; i < 64; i++) {
        printf("grid: %d\n",grid[i]);
    }
    */

    char filename[64];
    Move move = {};
    Piece undo_piece = {};
    char lettre;
    short white = 1;
    //srand(2); //10 14 15

    //main loop
    for (int i = 1; i <= n; i++) {
        move = rnd_best_move(white,pieces,grid,&board,d);
        if (move.x == 0 && move.y == 0){
            if (!not_defended(23-8*white, pieces, grid, &move, &white))
            {
                if (white==1){
                    printf("Black win\n");
                }
                else{
                    printf("White Win\n");
                }
            }
            else{
                printf("Stalemate\n");
            }
            break;
        }
        lettre = 'a' + pieces[move.piece].x+move.x;
        printf("%d.%c%c%d\n",i,pieces[move.piece].txt, lettre, pieces[move.piece].y+move.y+1);
        apply_move(pieces,grid,&board,&move, &undo_piece);
        plot_grid(pieces,grid);
        //print_bits(board.castle_rights);
        //printf("en passant: %d\n",board.en_passant);
        printf("\n");
        white=-white;
    }
    return 0;
}