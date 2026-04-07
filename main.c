#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "data_rep.h"
#include "print.h"

float eval(Piece *pieces){
    float sum=0;
    for (int i = 0; i < 16; i++) {
        sum = sum + pieces[i].value;
        //printf("%f\n",pieces[i].value);
    }
    for (int i = 16; i < 32; i++) {
        sum = sum - pieces[i].value;
        //printf("%d\n",i);
    }
    return sum;
}

void update_grid(Piece *pieces, int grid[]){
    // inpossible de supprimer cette boucle pour l'instant
    for (int i = 0; i < 64; i++) {
        grid[i]=32;
    }
    for (int i = 0; i < 32; i++) {
        if (pieces[i].value > 0){
            grid[pieces[i].x+pieces[i].y*8]=i;
        }
    }
}

void apply_move(Piece *pieces, int grid[], Move *move, Piece *undo_piece){
    //Don't work with x=0 and y=0 move
    int target=grid[(pieces[move->piece].x+(move->x))+(pieces[move->piece].y+(move->y))*8];
    if (target!=32){
        undo_piece->txt = pieces[target].txt;
        undo_piece->value = pieces[target].value;
        undo_piece->x = target;
        undo_piece->y = target;
        pieces[target].value=-1;
    }
    grid[pieces[move->piece].x+pieces[move->piece].y*8]=32;
    pieces[move->piece].x += move->x;
    pieces[move->piece].y += move->y;
    if (move->transform){
        pieces[move->piece].txt = move->transform;
        if(move->transform == 'Q'){
            pieces[move->piece].value = 9;
        }
        else if(move->transform == 'R'){
            pieces[move->piece].value = 5;
        }
        else{
            pieces[move->piece].value = 3;
        }
    }
    grid[pieces[move->piece].x+pieces[move->piece].y*8]=move->piece;
}

void undo_move(Piece *pieces, int grid[], Move *move, Piece *undo_piece){
    //Don't work with x=0 and y=0 move
    if (undo_piece->value > 0){
        pieces[undo_piece->x].txt = undo_piece->txt;
        pieces[undo_piece->x].value = undo_piece->value;
        pieces[undo_piece->x].x = pieces[move->piece].x;
        pieces[undo_piece->x].y = pieces[move->piece].y;
        grid[pieces[move->piece].x+pieces[move->piece].y*8]=undo_piece->x;
    }
    else{
        grid[pieces[move->piece].x+pieces[move->piece].y*8]=32;
    }
    pieces[move->piece].x -= move->x;
    pieces[move->piece].y -= move->y;
    if (move->transform){
        pieces[move->piece].txt = 'p';
        pieces[move->piece].value = 1;
    }
    grid[pieces[move->piece].x+pieces[move->piece].y*8]=move->piece;
}

int is_on_the_board(Piece *piece, Move *move){
    if (0 <= piece->x+move->x && piece->x+move->x < 8){
        if (0 <= piece->y+move->y && piece->y+move->y < 8){
            return 1;
        }
    }
    return 0;
}

int piece_there(Piece *piece, int grid[], Move *move){
    return grid[piece->x+move->x+(piece->y+move->y)*8];
}

int opponent_piece_there(Piece *piece, int grid[], Move *move, short *white){
    //white = your color
    int target=grid[piece->x+move->x+(piece->y+move->y)*8];
    if ( 8 + *white*8 <= target && target < 24 + *white*8 )
    {
        return 1;
    }
    return 0;
}

int opponent_or_free_there(Piece *piece, int grid[], Move *move, short *white){
    //white = your color
    int target=grid[piece->x+move->x+(piece->y+move->y)*8];
    if ((8 + *white*8 <= target && target < 24 + *white*8) || target==32)
    {
        return 1;
    }
    return 0;
}

int not_defended(int piece, Piece pieces[], int grid[], Move *move, short *white){
    //white = your color
    int test_piece;
    grid[pieces[piece].x + pieces[piece].y*8] = 32;
    Move tmp={};
    Piece start_piece = pieces[piece];
    for (int x=1; x<8; x++){
        //BISHOP
        tmp.x=move->x + x;
        tmp.y=move->y + x*(*white);
        if (!is_on_the_board(&start_piece,&tmp)){
            break;
        }
        test_piece = piece_there(&start_piece, grid, &tmp);
        if ( 8 + *white*8 <= test_piece && test_piece < 24 + *white*8 ){
            if (x==1){
                if (pieces[test_piece].txt == 'K' || pieces[test_piece].txt == 'p'){
                    grid[pieces[piece].x + pieces[piece].y*8] = piece;
                    return 0;
                }
            }
            if (pieces[test_piece].txt == 'B' || pieces[test_piece].txt == 'Q'){
                grid[pieces[piece].x + pieces[piece].y*8] = piece;
                return 0;
            }
        }
        if(test_piece!=32){
            break;
        }
    }
    for (int x=1; x<8; x++){
        tmp.x=move->x - x;
        tmp.y=move->y + x*(*white);
        if (!is_on_the_board(&start_piece,&tmp)){
            break;
        }
        test_piece = piece_there(&start_piece, grid, &tmp);
        if ( 8 + *white*8 <= test_piece && test_piece < 24 + *white*8 ){
            if (x==1){
                if (pieces[test_piece].txt == 'K' || pieces[test_piece].txt == 'p'){
                    grid[pieces[piece].x + pieces[piece].y*8] = piece;
                    return 0;
                }
            }
            if (pieces[test_piece].txt == 'B' || pieces[test_piece].txt == 'Q'){
                grid[pieces[piece].x + pieces[piece].y*8] = piece;
                return 0;
            }
        }
        if(test_piece!=32){
            break;
        }
    }
    for (int x=1; x<8; x++){
        tmp.x=move->x + x;
        tmp.y=move->y - x*(*white);
        if (!is_on_the_board(&start_piece,&tmp)){
            break;
        }
        test_piece = piece_there(&start_piece, grid, &tmp);
        if ( 8 + *white*8 <= test_piece && test_piece < 24 + *white*8 ){
            if (x==1){
                if (pieces[test_piece].txt == 'K'){
                    grid[pieces[piece].x + pieces[piece].y*8] = piece;
                    return 0;
                }
            }
            if (pieces[test_piece].txt == 'B' || pieces[test_piece].txt == 'Q'){
                grid[pieces[piece].x + pieces[piece].y*8] = piece;
                return 0;
            }
        }
        if(test_piece!=32){
            break;
        }
    }
    for (int x=1; x<8; x++){
        tmp.x=move->x - x;
        tmp.y=move->y - x*(*white);
        if (!is_on_the_board(&start_piece,&tmp)){
            break;
        }
        test_piece = piece_there(&start_piece, grid, &tmp);
        if ( 8 + *white*8 <= test_piece && test_piece < 24 + *white*8 ){
            if (x==1){
                if (pieces[test_piece].txt == 'K'){
                    grid[pieces[piece].x + pieces[piece].y*8] = piece;
                    return 0;
                }
            }
            if (pieces[test_piece].txt == 'B' || pieces[test_piece].txt == 'Q'){
                grid[pieces[piece].x + pieces[piece].y*8] = piece;
                return 0;
            }
        }
        if(test_piece!=32){
            break;
        }
    }
    //ROOK
    for (int x=1; x<8; x++){
        tmp.x=move->x + x;
        tmp.y=move->y;
        if (!is_on_the_board(&start_piece,&tmp)){
            break;
        }
        test_piece = piece_there(&start_piece, grid, &tmp);
        if ( 8 + *white*8 <= test_piece && test_piece < 24 + *white*8 ){
            if (x==1){
                if (pieces[test_piece].txt == 'K'){
                    grid[pieces[piece].x + pieces[piece].y*8] = piece;
                    return 0;
                }
            }
            if (pieces[test_piece].txt == 'R' || pieces[test_piece].txt == 'Q'){
                grid[pieces[piece].x + pieces[piece].y*8] = piece;
                return 0;
            }
        }
        if(test_piece!=32){
            break;
        }
    }
    for (int x=1; x<8; x++){
        tmp.x=move->x - x;
        tmp.y=move->y;
        if (!is_on_the_board(&start_piece,&tmp)){
            break;
        }
        test_piece = piece_there(&start_piece, grid, &tmp);
        if ( 8 + *white*8 <= test_piece && test_piece < 24 + *white*8 ){
            if (x==1){
                if (pieces[test_piece].txt == 'K'){
                    grid[pieces[piece].x + pieces[piece].y*8] = piece;
                    return 0;
                }
            }
            if (pieces[test_piece].txt == 'R' || pieces[test_piece].txt == 'Q'){
                grid[pieces[piece].x + pieces[piece].y*8] = piece;
                return 0;
            }
        }
        if(test_piece!=32){
            break;
        }
    }
    for (int x=1; x<8; x++){
        tmp.x=move->x;
        tmp.y=move->y + x;
        if (!is_on_the_board(&start_piece,&tmp)){
            break;
        }
        test_piece = piece_there(&start_piece, grid, &tmp);
        if ( 8 + *white*8 <= test_piece && test_piece < 24 + *white*8 ){
            if (x==1){
                if (pieces[test_piece].txt == 'K'){
                    grid[pieces[piece].x + pieces[piece].y*8] = piece;
                    return 0;
                }
            }
            if (pieces[test_piece].txt == 'R' || pieces[test_piece].txt == 'Q'){
                grid[pieces[piece].x + pieces[piece].y*8] = piece;
                return 0;
            }
        }
        if(test_piece!=32){
            break;
        }
    }
    for (int x=1; x<8; x++){
        tmp.x=move->x;
        tmp.y=move->y - x;
        if (!is_on_the_board(&start_piece,&tmp)){
            break;
        }
        test_piece = piece_there(&start_piece, grid, &tmp);
        if ( 8 + *white*8 <= test_piece && test_piece < 24 + *white*8 ){
            if (x==1){
                if (pieces[test_piece].txt == 'K'){
                    grid[pieces[piece].x + pieces[piece].y*8] = piece;
                    return 0;
                }
            }
            if (pieces[test_piece].txt == 'R' || pieces[test_piece].txt == 'Q'){
                grid[pieces[piece].x + pieces[piece].y*8] = piece;
                return 0;
            }
        }
        if(test_piece!=32){
            break;
        }
    }
    //Knight
    tmp.x=move->x + 1;
    tmp.y=move->y + 2;
    if (is_on_the_board(&start_piece,&tmp)){
        test_piece = piece_there(&start_piece, grid, &tmp);
        if (( 8 + *white*8 <= test_piece && test_piece < 24 + *white*8 ) && pieces[test_piece].txt == 'N'){
            grid[pieces[piece].x + pieces[piece].y*8] = piece;
            return 0;
        }
    }
    tmp.x=move->x - 1;
    tmp.y=move->y + 2;
    if (is_on_the_board(&start_piece,&tmp)){
        test_piece = piece_there(&start_piece, grid, &tmp);
        if (( 8 + *white*8 <= test_piece && test_piece < 24 + *white*8 ) && pieces[test_piece].txt == 'N'){
            grid[pieces[piece].x + pieces[piece].y*8] = piece;
            return 0;
        }
    }
    tmp.x=move->x + 1;
    tmp.y=move->y - 2;
    if (is_on_the_board(&start_piece,&tmp)){
        test_piece = piece_there(&start_piece, grid, &tmp);
        if (( 8 + *white*8 <= test_piece && test_piece < 24 + *white*8 ) && pieces[test_piece].txt == 'N'){
            grid[pieces[piece].x + pieces[piece].y*8] = piece;
            return 0;
        }
    }
    tmp.x=move->x - 1;
    tmp.y=move->y - 2;
    if (is_on_the_board(&start_piece,&tmp)){
        test_piece = piece_there(&start_piece, grid, &tmp);
        if (( 8 + *white*8 <= test_piece && test_piece < 24 + *white*8 ) && pieces[test_piece].txt == 'N'){
            grid[pieces[piece].x + pieces[piece].y*8] = piece;
            return 0;
        }
    }
    tmp.x=move->x + 2;
    tmp.y=move->y + 1;
    if (is_on_the_board(&start_piece,&tmp)){
        test_piece = piece_there(&start_piece, grid, &tmp);
        if (( 8 + *white*8 <= test_piece && test_piece < 24 + *white*8 ) && pieces[test_piece].txt == 'N'){
            grid[pieces[piece].x + pieces[piece].y*8] = piece;
            return 0;
        }
    }
    tmp.x=move->x - 2;
    tmp.y=move->y + 1;
    if (is_on_the_board(&start_piece,&tmp)){
        test_piece = piece_there(&start_piece, grid, &tmp);
        if (( 8 + *white*8 <= test_piece && test_piece < 24 + *white*8 ) && pieces[test_piece].txt == 'N'){
            grid[pieces[piece].x + pieces[piece].y*8] = piece;
            return 0;
        }
    }
    tmp.x=move->x + 2;
    tmp.y=move->y - 1;
    if (is_on_the_board(&start_piece,&tmp)){
        test_piece = piece_there(&start_piece, grid, &tmp);
        if (( 8 + *white*8 <= test_piece && test_piece < 24 + *white*8 ) && pieces[test_piece].txt == 'N'){
            grid[pieces[piece].x + pieces[piece].y*8] = piece;
            return 0;
        }
    }
    tmp.x=move->x - 2;
    tmp.y=move->y - 1;
    if (is_on_the_board(&start_piece,&tmp)){
        test_piece = piece_there(&start_piece, grid, &tmp);
        if (( 8 + *white*8 <= test_piece && test_piece < 24 + *white*8 ) && pieces[test_piece].txt == 'N'){
            grid[pieces[piece].x + pieces[piece].y*8] = piece;
            return 0;
        }
    }
    grid[pieces[piece].x + pieces[piece].y*8] = piece;
    return 1;
}

int move_defend_king(Piece pieces[], int grid[], Move *move, short *white){
    Piece undo_piece = {};
    Move tmp = {};
    apply_move(pieces,grid,move, &undo_piece);
    int not_attacked = not_defended(23-8*(*white), pieces, grid, &tmp, white);
    undo_move(pieces,grid,move, &undo_piece);
    return not_attacked;
}

Move next(short white, Piece *pieces, int grid[], Move move, int depth){
    /////// SEQ /////////
    //prend en arg une grille et retourne la valeur du meilleur mouve trouvé et le meilleur move
    //parcour chaques moves possibles:
        // do move
        // La valeur du move est la valeur retournée par next
        // si la valeur est plus grande que celle des moves calculés on la stoque, et on supprime les autres.
        // si la valeur est égale, on ajoute au tableau en vu de faire un move aléatoire
        // undo move
    Move best = {};
    Move tmp = {};
    Move possible[138] = {};
    Move possible_best[138] = {};
    Piece undo_piece = {};
    int fill = 0;
    int equal = 0;
    int test_piece;

    undo_piece.value=-1;
    best.value=-1000*white;
    if (move.x != 0 || move.y != 0){
        apply_move(pieces,grid,&move, &undo_piece);
    }
    if (depth == 0){
        best.piece=move.piece;
        best.x=move.x;
        best.y=move.y;
        best.value=eval(pieces);
        undo_move(pieces,grid,&move, &undo_piece);
        return best;
    }
    for (int i = 8 - white*8 ; i < 24 - white*8 ; i++) {
        //printf("%d\n",i);
        if (pieces[i].value < 0){
            continue;
        }
        tmp.piece=i;
        if (pieces[i].txt == 'p'){
            // move forward
            tmp.x=0;
            tmp.y=1*white;
            if (piece_there(&pieces[i], grid, &tmp)==32 && move_defend_king(pieces, grid, &tmp, &white))
            {
                // transform into another piece
                if ((white == 1 && pieces[i].y == 6) || (white == -1 && pieces[i].y == 1)){
                    tmp.transform = 'Q';
                    tmp.value = next(-white,pieces, grid, tmp, depth - 1).value;
                    possible[fill]=tmp;
                    fill += 1;
                    tmp.transform = 'R';
                    tmp.value = next(-white,pieces, grid, tmp, depth - 1).value;
                    possible[fill]=tmp;
                    fill += 1;
                    tmp.transform = 'B';
                    tmp.value = next(-white,pieces, grid, tmp, depth - 1).value;
                    possible[fill]=tmp;
                    fill += 1;
                    tmp.transform = 'N';
                    tmp.value = next(-white,pieces, grid, tmp, depth - 1).value;
                    possible[fill]=tmp;
                    fill += 1;
                    tmp.transform = 0;
                }
                else{
                    tmp.value = next(-white,pieces, grid, tmp, depth - 1).value;
                    possible[fill]=tmp;
                    fill += 1;
                    // move forward +2
                    if ((white==1 && pieces[i].y==1) || (white==-1 && pieces[i].y==6)){
                        tmp.x=0;
                        tmp.y=2*white;
                        if (move_defend_king(pieces, grid, &tmp, &white))
                        {
                            tmp.value = next(-white,pieces, grid, tmp, depth - 1).value;
                            possible[fill]=tmp;
                            fill += 1;
                        }
                    }
                }
            }
            // eat on the left (for white, and right for black)
            tmp.x=-1;
            tmp.y=1*white;
            if (is_on_the_board(&pieces[i],&tmp) && opponent_piece_there(&pieces[i], grid, &tmp, &white) && move_defend_king(pieces, grid, &tmp, &white))
            {
                // transform into another piece
                if ((white == 1 && pieces[i].y == 6) || (white == -1 && pieces[i].y == 1)){
                    tmp.transform = 'Q';
                    tmp.value = next(-white,pieces, grid, tmp, depth - 1).value;
                    possible[fill]=tmp;
                    fill += 1;
                    tmp.transform = 'R';
                    tmp.value = next(-white,pieces, grid, tmp, depth - 1).value;
                    possible[fill]=tmp;
                    fill += 1;
                    tmp.transform = 'B';
                    tmp.value = next(-white,pieces, grid, tmp, depth - 1).value;
                    possible[fill]=tmp;
                    fill += 1;
                    tmp.transform = 'N';
                    tmp.value = next(-white,pieces, grid, tmp, depth - 1).value;
                    possible[fill]=tmp;
                    fill += 1;
                    tmp.transform = 0;
                }
                tmp.value = next(-white,pieces, grid, tmp, depth - 1).value;
                possible[fill]=tmp;
                fill += 1;
            }
            // eat on the right (for white, and left for black)
            tmp.x=1;
            tmp.y=1*white;
            if (is_on_the_board(&pieces[i],&tmp) && opponent_piece_there(&pieces[i], grid, &tmp, &white) && move_defend_king(pieces, grid, &tmp, &white))
            {
                // transform into another piece
                if ((white == 1 && pieces[i].y == 6) || (white == -1 && pieces[i].y == 1)){
                    tmp.transform = 'Q';
                    tmp.value = next(-white,pieces, grid, tmp, depth - 1).value;
                    possible[fill]=tmp;
                    fill += 1;
                    tmp.transform = 'R';
                    tmp.value = next(-white,pieces, grid, tmp, depth - 1).value;
                    possible[fill]=tmp;
                    fill += 1;
                    tmp.transform = 'B';
                    tmp.value = next(-white,pieces, grid, tmp, depth - 1).value;
                    possible[fill]=tmp;
                    fill += 1;
                    tmp.transform = 'N';
                    tmp.value = next(-white,pieces, grid, tmp, depth - 1).value;
                    possible[fill]=tmp;
                    fill += 1;
                    tmp.transform = 0;
                }
                tmp.value = next(-white,pieces, grid, tmp, depth - 1).value;
                possible[fill]=tmp;
                fill += 1;
            }
        }
        else if (pieces[i].txt == 'N'){
            tmp.x=1;
            tmp.y=2;
            if (is_on_the_board(&pieces[i],&tmp) && opponent_or_free_there(&pieces[i], grid, &tmp, &white) && move_defend_king(pieces, grid, &tmp, &white)){
                tmp.value = next(-white,pieces, grid, tmp, depth - 1).value;
                possible[fill]=tmp;
                fill += 1;
            }
            tmp.x=-1;
            tmp.y=2;
            if (is_on_the_board(&pieces[i],&tmp) && opponent_or_free_there(&pieces[i], grid, &tmp, &white) && move_defend_king(pieces, grid, &tmp, &white)){
                tmp.value = next(-white,pieces, grid, tmp, depth - 1).value;
                possible[fill]=tmp;
                fill += 1;
            }
            tmp.x=1;
            tmp.y=-2;
            if (is_on_the_board(&pieces[i],&tmp) && opponent_or_free_there(&pieces[i], grid, &tmp, &white) && move_defend_king(pieces, grid, &tmp, &white)){
                tmp.value = next(-white,pieces, grid, tmp, depth - 1).value;
                possible[fill]=tmp;
                fill += 1;
            }
            tmp.x=-1;
            tmp.y=-2;
            if (is_on_the_board(&pieces[i],&tmp) && opponent_or_free_there(&pieces[i], grid, &tmp, &white) && move_defend_king(pieces, grid, &tmp, &white)){
                tmp.value = next(-white,pieces, grid, tmp, depth - 1).value;
                possible[fill]=tmp;
                fill += 1;
            }
            tmp.x=2;
            tmp.y=1;
            if (is_on_the_board(&pieces[i],&tmp) && opponent_or_free_there(&pieces[i], grid, &tmp, &white) && move_defend_king(pieces, grid, &tmp, &white)){
                tmp.value = next(-white,pieces, grid, tmp, depth - 1).value;
                possible[fill]=tmp;
                fill += 1;
            }
            tmp.x=2;
            tmp.y=-1;
            if (is_on_the_board(&pieces[i],&tmp) && opponent_or_free_there(&pieces[i], grid, &tmp, &white) && move_defend_king(pieces, grid, &tmp, &white)){
                tmp.value = next(-white,pieces, grid, tmp, depth - 1).value;
                possible[fill]=tmp;
                fill += 1;
            }
            tmp.x=-2;
            tmp.y=1;
            if (is_on_the_board(&pieces[i],&tmp) && opponent_or_free_there(&pieces[i], grid, &tmp, &white) && move_defend_king(pieces, grid, &tmp, &white)){
                tmp.value = next(-white,pieces, grid, tmp, depth - 1).value;
                possible[fill]=tmp;
                fill += 1;
            }
            tmp.x=-2;
            tmp.y=-1;
            if (is_on_the_board(&pieces[i],&tmp) && opponent_or_free_there(&pieces[i], grid, &tmp, &white) && move_defend_king(pieces, grid, &tmp, &white)){
                tmp.value = next(-white,pieces, grid, tmp, depth - 1).value;
                possible[fill]=tmp;
                fill += 1;
            }
        }
        else if (pieces[i].txt == 'B'){
            for (int x=1; x<8; x++){
                tmp.x=x;
                tmp.y=x;
                if (!is_on_the_board(&pieces[i],&tmp)){
                    break;
                }
                test_piece = piece_there(&pieces[i], grid, &tmp);
                if (((8 + white*8 <= test_piece && test_piece < 24 + white*8) || test_piece==32) && move_defend_king(pieces, grid, &tmp, &white)){
                    tmp.value = next(-white,pieces, grid, tmp, depth - 1).value;
                    possible[fill]=tmp;
                    fill += 1;
                }
                if (test_piece!=32){
                    break;
                }
            }
            for (int x=1; x<8; x++){
                tmp.x=-x;
                tmp.y=x;
                if (!is_on_the_board(&pieces[i],&tmp)){
                    break;
                }
                test_piece = piece_there(&pieces[i], grid, &tmp);
                if (((8 + white*8 <= test_piece && test_piece < 24 + white*8) || test_piece==32) && move_defend_king(pieces, grid, &tmp, &white)){
                    tmp.value = next(-white,pieces, grid, tmp, depth - 1).value;
                    possible[fill]=tmp;
                    fill += 1;
                }
                if (test_piece!=32){
                    break;
                }
            }
            for (int x=1; x<8; x++){
                tmp.x=x;
                tmp.y=-x;
                if (!is_on_the_board(&pieces[i],&tmp)){
                    break;
                }
                test_piece = piece_there(&pieces[i], grid, &tmp);
                if (((8 + white*8 <= test_piece && test_piece < 24 + white*8) || test_piece==32) && move_defend_king(pieces, grid, &tmp, &white)){
                    tmp.value = next(-white,pieces, grid, tmp, depth - 1).value;
                    possible[fill]=tmp;
                    fill += 1;
                }
                if (test_piece!=32){
                    break;
                }
            }
            for (int x=1; x<8; x++){
                tmp.x=-x;
                tmp.y=-x;
                if (!is_on_the_board(&pieces[i],&tmp)){
                    break;
                }
                test_piece = piece_there(&pieces[i], grid, &tmp);
                if (((8 + white*8 <= test_piece && test_piece < 24 + white*8) || test_piece==32) && move_defend_king(pieces, grid, &tmp, &white)){
                    tmp.value = next(-white,pieces, grid, tmp, depth - 1).value;
                    possible[fill]=tmp;
                    fill += 1;
                }
                if (test_piece!=32){
                    break;
                }
            }
        }
        else if (pieces[i].txt == 'R'){
            for (int x=1; x<8; x++){
                tmp.x=x;
                tmp.y=0;
                if (!is_on_the_board(&pieces[i],&tmp)){
                    break;
                }
                test_piece = piece_there(&pieces[i], grid, &tmp);
                if (((8 + white*8 <= test_piece && test_piece < 24 + white*8) || test_piece==32) && move_defend_king(pieces, grid, &tmp, &white)){
                    tmp.value = next(-white,pieces, grid, tmp, depth - 1).value;
                    possible[fill]=tmp;
                    fill += 1;
                }
                if (test_piece!=32){
                    break;
                }
            }
            for (int x=1; x<8; x++){
                tmp.x=-x;
                tmp.y=0;
                if (!is_on_the_board(&pieces[i],&tmp)){
                    break;
                }
                test_piece = piece_there(&pieces[i], grid, &tmp);
                if (((8 + white*8 <= test_piece && test_piece < 24 + white*8) || test_piece==32) && move_defend_king(pieces, grid, &tmp, &white)){
                    tmp.value = next(-white,pieces, grid, tmp, depth - 1).value;
                    possible[fill]=tmp;
                    fill += 1;
                }
                if (test_piece!=32){
                    break;
                }
            }
            for (int x=1; x<8; x++){
                tmp.x=0;
                tmp.y=x;
                if (!is_on_the_board(&pieces[i],&tmp)){
                    break;
                }
                test_piece = piece_there(&pieces[i], grid, &tmp);
                if (((8 + white*8 <= test_piece && test_piece < 24 + white*8) || test_piece==32) && move_defend_king(pieces, grid, &tmp, &white)){
                    tmp.value = next(-white,pieces, grid, tmp, depth - 1).value;
                    possible[fill]=tmp;
                    fill += 1;
                }
                if (test_piece!=32){
                    break;
                }
            }
            for (int x=1; x<8; x++){
                tmp.x=0;
                tmp.y=-x;
                if (!is_on_the_board(&pieces[i],&tmp)){
                    break;
                }
                test_piece = piece_there(&pieces[i], grid, &tmp);
                if (((8 + white*8 <= test_piece && test_piece < 24 + white*8) || test_piece==32) && move_defend_king(pieces, grid, &tmp, &white)){
                    tmp.value = next(-white,pieces, grid, tmp, depth - 1).value;
                    possible[fill]=tmp;
                    fill += 1;
                }
                if (test_piece!=32){
                    break;
                }
            }
        }
        else if (pieces[i].txt == 'Q'){
            //BISHOP
            for (int x=1; x<8; x++){
                tmp.x=x;
                tmp.y=x;
                if (!is_on_the_board(&pieces[i],&tmp)){
                    break;
                }
                test_piece = piece_there(&pieces[i], grid, &tmp);
                if (((8 + white*8 <= test_piece && test_piece < 24 + white*8) || test_piece==32) && move_defend_king(pieces, grid, &tmp, &white)){
                    tmp.value = next(-white,pieces, grid, tmp, depth - 1).value;
                    possible[fill]=tmp;
                    fill += 1;
                }
                if (test_piece!=32){
                    break;
                }
            }
            for (int x=1; x<8; x++){
                tmp.x=-x;
                tmp.y=x;
                if (!is_on_the_board(&pieces[i],&tmp)){
                    break;
                }
                test_piece = piece_there(&pieces[i], grid, &tmp);
                if (((8 + white*8 <= test_piece && test_piece < 24 + white*8) || test_piece==32) && move_defend_king(pieces, grid, &tmp, &white)){
                    tmp.value = next(-white,pieces, grid, tmp, depth - 1).value;
                    possible[fill]=tmp;
                    fill += 1;
                }
                if (test_piece!=32){
                    break;
                }
            }
            for (int x=1; x<8; x++){
                tmp.x=x;
                tmp.y=-x;
                if (!is_on_the_board(&pieces[i],&tmp)){
                    break;
                }
                test_piece = piece_there(&pieces[i], grid, &tmp);
                if (((8 + white*8 <= test_piece && test_piece < 24 + white*8) || test_piece==32) && move_defend_king(pieces, grid, &tmp, &white)){
                    tmp.value = next(-white,pieces, grid, tmp, depth - 1).value;
                    possible[fill]=tmp;
                    fill += 1;
                }
                if (test_piece!=32){
                    break;
                }
            }
            for (int x=1; x<8; x++){
                tmp.x=-x;
                tmp.y=-x;
                if (!is_on_the_board(&pieces[i],&tmp)){
                    break;
                }
                test_piece = piece_there(&pieces[i], grid, &tmp);
                if (((8 + white*8 <= test_piece && test_piece < 24 + white*8) || test_piece==32) && move_defend_king(pieces, grid, &tmp, &white)){
                    tmp.value = next(-white,pieces, grid, tmp, depth - 1).value;
                    possible[fill]=tmp;
                    fill += 1;
                }
                if (test_piece!=32){
                    break;
                }
            }
            //ROOK
            for (int x=1; x<8; x++){
                tmp.x=x;
                tmp.y=0;
                if (!is_on_the_board(&pieces[i],&tmp)){
                    break;
                }
                test_piece = piece_there(&pieces[i], grid, &tmp);
                if (((8 + white*8 <= test_piece && test_piece < 24 + white*8) || test_piece==32) && move_defend_king(pieces, grid, &tmp, &white)){
                    tmp.value = next(-white,pieces, grid, tmp, depth - 1).value;
                    possible[fill]=tmp;
                    fill += 1;
                }
                if (test_piece!=32){
                    break;
                }
            }
            for (int x=1; x<8; x++){
                tmp.x=-x;
                tmp.y=0;
                if (!is_on_the_board(&pieces[i],&tmp)){
                    break;
                }
                test_piece = piece_there(&pieces[i], grid, &tmp);
                if (((8 + white*8 <= test_piece && test_piece < 24 + white*8) || test_piece==32) && move_defend_king(pieces, grid, &tmp, &white)){
                    tmp.value = next(-white,pieces, grid, tmp, depth - 1).value;
                    possible[fill]=tmp;
                    fill += 1;
                }
                if (test_piece!=32){
                    break;
                }
            }
            for (int x=1; x<8; x++){
                tmp.x=0;
                tmp.y=x;
                if (!is_on_the_board(&pieces[i],&tmp)){
                    break;
                }
                test_piece = piece_there(&pieces[i], grid, &tmp);
                if (((8 + white*8 <= test_piece && test_piece < 24 + white*8) || test_piece==32) && move_defend_king(pieces, grid, &tmp, &white)){
                    tmp.value = next(-white,pieces, grid, tmp, depth - 1).value;
                    possible[fill]=tmp;
                    fill += 1;
                }
                if (test_piece!=32){
                    break;
                }
            }
            for (int x=1; x<8; x++){
                tmp.x=0;
                tmp.y=-x;
                if (!is_on_the_board(&pieces[i],&tmp)){
                    break;
                }
                test_piece = piece_there(&pieces[i], grid, &tmp);
                if (((8 + white*8 <= test_piece && test_piece < 24 + white*8) || test_piece==32) && move_defend_king(pieces, grid, &tmp, &white)){
                    tmp.value = next(-white,pieces, grid, tmp, depth - 1).value;
                    possible[fill]=tmp;
                    fill += 1;
                }
                if (test_piece!=32){
                    break;
                }
            }
        }
        else if (pieces[i].txt == 'K'){
            tmp.x=1;
            tmp.y=0;
            if (is_on_the_board(&pieces[i],&tmp) && opponent_or_free_there(&pieces[i], grid, &tmp, &white) && not_defended(i, pieces, grid, &tmp, &white)){
                tmp.value = next(-white,pieces, grid, tmp, depth - 1).value;
                possible[fill]=tmp;
                fill += 1;
            }
            tmp.x=1;
            tmp.y=1;
            if (is_on_the_board(&pieces[i],&tmp) && opponent_or_free_there(&pieces[i], grid, &tmp, &white) && not_defended(i, pieces, grid, &tmp, &white)){
                tmp.value = next(-white,pieces, grid, tmp, depth - 1).value;
                possible[fill]=tmp;
                fill += 1;
            }
            tmp.x=0;
            tmp.y=1;
            if (is_on_the_board(&pieces[i],&tmp) && opponent_or_free_there(&pieces[i], grid, &tmp, &white) && not_defended(i, pieces, grid, &tmp, &white)){
                tmp.value = next(-white,pieces, grid, tmp, depth - 1).value;
                possible[fill]=tmp;
                fill += 1;
            }
            tmp.x=-1;
            tmp.y=1;
            if (is_on_the_board(&pieces[i],&tmp) && opponent_or_free_there(&pieces[i], grid, &tmp, &white) && not_defended(i, pieces, grid, &tmp, &white)){
                tmp.value = next(-white,pieces, grid, tmp, depth - 1).value;
                possible[fill]=tmp;
                fill += 1;
            }
            tmp.x=-1;
            tmp.y=0;
            if (is_on_the_board(&pieces[i],&tmp) && opponent_or_free_there(&pieces[i], grid, &tmp, &white) && not_defended(i, pieces, grid, &tmp, &white)){
                tmp.value = next(-white,pieces, grid, tmp, depth - 1).value;
                possible[fill]=tmp;
                fill += 1;
            }
            tmp.x=-1;
            tmp.y=-1;
            if (is_on_the_board(&pieces[i],&tmp) && opponent_or_free_there(&pieces[i], grid, &tmp, &white) && not_defended(i, pieces, grid, &tmp, &white)){
                tmp.value = next(-white,pieces, grid, tmp, depth - 1).value;
                possible[fill]=tmp;
                fill += 1;
            }
            tmp.x=0;
            tmp.y=-1;
            if (is_on_the_board(&pieces[i],&tmp) && opponent_or_free_there(&pieces[i], grid, &tmp, &white) && not_defended(i, pieces, grid, &tmp, &white)){
                tmp.value = next(-white,pieces, grid, tmp, depth - 1).value;
                possible[fill]=tmp;
                fill += 1;
            }
            tmp.x=1;
            tmp.y=-1;
            if (is_on_the_board(&pieces[i],&tmp) && opponent_or_free_there(&pieces[i], grid, &tmp, &white) && not_defended(i, pieces, grid, &tmp, &white)){
                tmp.value = next(-white,pieces, grid, tmp, depth - 1).value;
                possible[fill]=tmp;
                fill += 1;
            }
        }
    }
    // select
    if (fill==0){
        if (not_defended(23-8*white, pieces, grid, &best, &white)){
            //stalemate
            best.value=0;
        }
        undo_move(pieces,grid,&move, &undo_piece);
        return best;
    }
    undo_move(pieces,grid,&move, &undo_piece);
    best = possible[0];
    for (int i=0; i < fill; i++){
        if (possible[i].value*white > best.value*white){
            best = possible[i];
        }
    }
    for (int i=0; i < fill; i++){
        if (possible[i].value == best.value){
            possible_best[equal]=possible[i];
            equal += 1;
        }
    }
    if (equal > 1){
        best=possible_best[rand() % equal];
    }
    return best;
}

int main (int argc, char *argv[]){
    int n,d;
    if (argc == 3)
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

    Move move = {0,0,0,0};
    Piece undo_piece = {};
    char lettre;
    short white = 1;
    //srand(19); //10 14 15
    srand(time(NULL));  // Seed
    //main loop
    for (int i = 1; i <= n; i++) {
        move = next(white,pieces,grid,move,d);
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
        printf("value: %f\n", move.value);
        apply_move(pieces,grid,&move, &undo_piece);
        move.transform = 0;
        plot_grid(pieces,grid);
        printf("\n");
        white=-white;
        move.x=0;
        move.y=0;
    }
    return 0;
}