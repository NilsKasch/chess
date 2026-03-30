#include <stdio.h>
#include "data_rep.h"
#include "print.h"

char *get_symbole(Piece *piece, int i){
    if (i<16){
        if (piece->txt == 'p'){
            return "♟";
        }
        else if (piece->txt == 'K'){
            return "♚";
        }
        else if (piece->txt == 'Q'){
            return "♛";
        }
        else if (piece->txt == 'R'){
            return "♜";
        }
        else if (piece->txt == 'B'){
            return "♝";
        }
        else if (piece->txt == 'N'){
            return "♞";
        }
    }
    else{
        if (piece->txt == 'p'){
            return "♙";
        }
        else if (piece->txt == 'K'){
            return "♔";
        }
        else if (piece->txt == 'Q'){
            return "♕";
        }
        else if (piece->txt == 'R'){
            return "♖";
        }
        else if (piece->txt == 'B'){
            return "♗";
        }
        else if (piece->txt == 'N'){
            return "♘";
        }
    }
}

void plot_grid_old(Piece *pieces, int grid[]){
    int i;
    for (int y = 7; y >= 0; y--){
        for (int x = 0; x < 8; x++){
            i = x + 8*y;
            if (grid[i]==32){
                printf(". ");
            }
            else{
                printf("%c ", pieces[grid[i]].txt);
            }
            if ((i+1)%8==0){
                printf("\n");
            }
        }
    }
}

void plot_grid(Piece *pieces, int grid[]){
    int i;
    for (int y = 7; y >= 0; y--){
        for (int x = 0; x < 8; x++){
            i = x + 8*y;
            if (grid[i]==32){
                printf(". ");
            }
            else{
                printf("%s ", get_symbole(&pieces[grid[i]],grid[i]));
            }
            if ((i+1)%8==0){
                printf("\n");
            }
        }
    }
}
