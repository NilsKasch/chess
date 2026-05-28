#include <stdio.h>
#include <stdlib.h>
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

void write_grid(FILE *fichier, Piece *pieces, int grid[]){
    int i;
    for (int y = 7; y >= 0; y--){
        for (int x = 0; x < 8; x++){
            i = x + 8*y;
            if (grid[i]==32){
                fprintf(fichier, ". ");
            }
            else{
                fprintf(fichier, "%s ", get_symbole(&pieces[grid[i]],grid[i]));
            }
            if ((i+1)%8==0){
                fprintf(fichier, "\n");
            }
        }
    }
}

void export_data(FILE *fichier, Piece *pieces, int grid[], int depth, float eval){

    fprintf(fichier, "depth: %d eval: %f\n", depth, eval);
    write_grid(fichier, pieces, grid);

}


void print_bits(unsigned int n) {
    for (int i = sizeof(n) * 8 - 1; i >= 0; i--) {
        printf("%d", (n >> i) & 1);
    }
    printf("\n");
}

        // //file open
        // snprintf(filename, sizeof(filename),"data/data_%d.chess", i);
        // FILE *file = fopen(filename, "w");
        // if (file == NULL) {
        //     printf("Erreur ouverture fichier\n");
        //     exit(1);
        // }
        // fprintf(file, "white: %d\n",white);
        // export_data(file,pieces,grid,-1,-1);
        // fclose(file);