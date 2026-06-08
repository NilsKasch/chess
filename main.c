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

void apply_move(Piece *pieces, int grid[], Board *board, Move *move, Piece *undo_piece){
    //Don't work with x=0 and y=0 move
    int target=grid[(pieces[move->piece].x+(move->x))+(pieces[move->piece].y+(move->y))*8];
    undo_piece->y = board->castle_rights; // used to stock castle rights
    if (target!=32){
        undo_piece->txt = pieces[target].txt;
        undo_piece->value = pieces[target].value;
        undo_piece->x = target;
        pieces[target].value=0;
        //remove castle rights if rook eaten
        if (pieces[target].txt == 'R'){
            if (pieces[target].y <= 3){
                if (pieces[target].x <= 4){
                    board->castle_rights &= ~WHITE_CASLTE_LEFT;
                }
                if (pieces[target].x >= 4){
                    board->castle_rights &= ~WHITE_CASLTE_RIGHT;
                }
            }
            else{
                if (pieces[target].x <= 4){
                        board->castle_rights &= ~BLACK_CASLTE_LEFT;
                }
                if (pieces[target].x >= 4){
                        board->castle_rights &= ~BLACK_CASLTE_RIGHT;
                }
            }
        }
    }
    // special rules
    if (pieces[move->piece].txt == 'p' && (move->y == 2 || move->y == -2)){
        //en passant
        board->en_passant = move->piece;
    }
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
    else if (board->castle_rights & (WHITE_CASLTE_LEFT | WHITE_CASLTE_RIGHT | BLACK_CASLTE_LEFT | BLACK_CASLTE_RIGHT)) //if supplémentaire a tester si plus rapide sans
    {
        if (pieces[move->piece].txt == 'K'){
            if (move->x == -2){
                if (pieces[move->piece].y <= 3){
                    //move the rook
                    grid[0]=32;
                    pieces[8].x = 3;
                    grid[3]=8;
                }
                else{
                    //move the rook
                    grid[56]=32;
                    pieces[24].x = 3;
                    grid[59]=24;
                }
            }
            if (move->x == 2){
                if (pieces[move->piece].y <= 3){
                    //move the rook
                    grid[7]=32;
                    pieces[9].x = 5;
                    grid[5]=9;
                }
                else{
                    //move the rook
                    grid[63]=32;
                    pieces[25].x = 5;
                    grid[61]=25;
                }
            }
        }
        if (pieces[move->piece].txt == 'R' || pieces[move->piece].txt == 'K'){
            if (pieces[move->piece].y <= 3){
                if (pieces[move->piece].x <= 4){
                    board->castle_rights &= ~WHITE_CASLTE_LEFT;
                }
                if (pieces[move->piece].x >= 4){
                    board->castle_rights &= ~WHITE_CASLTE_RIGHT;
                }
            }
            else{
                if (pieces[move->piece].x <= 4){
                        board->castle_rights &= ~BLACK_CASLTE_LEFT;
                }
                if (pieces[move->piece].x >= 4){
                        board->castle_rights &= ~BLACK_CASLTE_RIGHT;
                }
            }
        }
    }

    //update grid
    grid[pieces[move->piece].x+pieces[move->piece].y*8]=32;
    pieces[move->piece].x += move->x;
    pieces[move->piece].y += move->y;
    grid[pieces[move->piece].x+pieces[move->piece].y*8]=move->piece;
}

void undo_move(Piece *pieces, int grid[], Board *board, Move *move, Piece *undo_piece){
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

    //restore castle rights
    board->castle_rights = undo_piece->y;

    //update grid
    pieces[move->piece].x -= move->x;
    pieces[move->piece].y -= move->y;
    //special rules
    if (move->transform){
        pieces[move->piece].txt = 'p';
        pieces[move->piece].value = 1;
    }
    if (pieces[move->piece].txt == 'K'){
        if (move->x == -2){
            if (pieces[move->piece].y <= 3){
                //move the rook
                grid[0]=8;
                pieces[8].x = 0;
                grid[3]=32;
            }
            else{
                //move the rook
                grid[56]=24;
                pieces[24].x = 0;
                grid[59]=32;
            }
        }
        if (move->x == 2){
            if (pieces[move->piece].y <= 3){
                //move the rook
                grid[7]=9;
                pieces[9].x = 7;
                grid[5]=32;
            }
            else{
                //move the rook
                grid[63]=25;
                pieces[25].x = 7;
                grid[61]=32;
            }
        }
    }
    //finish update grid
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

int possible_to_castle_white_left(Piece pieces[], int grid[]){
    // call this only if there is free space for castling
    // grid[1]==32 && grid[2]==32 && grid[3]==32
    int test_piece;
    Move tmp={};
    for (int piece_x=1; piece_x<4; piece_x++){
        for (int x=1; x<8; x++){
            //BISHOP
            tmp.x=x;
            tmp.y=x;
            // if not on the board
            if ((piece_x + tmp.x < 0) || (7 < piece_x + tmp.x) || (tmp.y < 0) || (7 < tmp.y)){
                break;
            }
            test_piece = grid[piece_x+tmp.x+tmp.y*8];
            if ( 16 <= test_piece && test_piece < 32 ){
                if (x==1){
                    if (pieces[test_piece].txt == 'K' || pieces[test_piece].txt == 'p'){
                        return 0;
                    }
                }
                if (pieces[test_piece].txt == 'B' || pieces[test_piece].txt == 'Q'){
                    return 0;
                }
            }
            if(test_piece!=32){
                break;
            }
        }
        for (int x=1; x<8; x++){
            //BISHOP
            tmp.x=-x;
            tmp.y=x;
            // if not on the board
            if ((piece_x + tmp.x < 0) || (7 < piece_x + tmp.x) || (tmp.y < 0) || (7 < tmp.y)){
                break;
            }
            test_piece = grid[piece_x+tmp.x+tmp.y*8];
            if ( 16 <= test_piece && test_piece < 32 ){
                if (x==1){
                    if (pieces[test_piece].txt == 'K' || pieces[test_piece].txt == 'p'){
                        return 0;
                    }
                }
                if (pieces[test_piece].txt == 'B' || pieces[test_piece].txt == 'Q'){
                    return 0;
                }
            }
            if(test_piece!=32){
                break;
            }
        }
        for (int x=1; x<8; x++){
            //ROOK
            tmp.x=0;
            tmp.y=x;
            // if not on the board
            if ((piece_x + tmp.x < 0) || (7 < piece_x + tmp.x) || (tmp.y < 0) || (7 < tmp.y)){
                break;
            }
            test_piece = grid[piece_x+tmp.x+tmp.y*8];
            if ( 16 <= test_piece && test_piece < 32 ){
                if (x==1){
                    if (pieces[test_piece].txt == 'K'){
                        return 0;
                    }
                }
                if (pieces[test_piece].txt == 'R' || pieces[test_piece].txt == 'Q'){
                    return 0;
                }
            }
            if(test_piece!=32){
                break;
            }
        }
        //Knight
        tmp.x= 2;
        tmp.y= 1;
        // if not on the board
        if ((piece_x + tmp.x < 0) || (7 < piece_x + tmp.x) || (tmp.y < 0) || (7 < tmp.y)){
            test_piece = grid[piece_x+tmp.x+tmp.y*8];
            if (( 16 <= test_piece && test_piece < 32 ) && pieces[test_piece].txt == 'N'){
                return 0;
            }
        }
        tmp.x= 1;
        tmp.y= 2;
        // if not on the board
        if ((piece_x + tmp.x < 0) || (7 < piece_x + tmp.x) || (tmp.y < 0) || (7 < tmp.y)){
            test_piece = grid[piece_x+tmp.x+tmp.y*8];
            if (( 16 <= test_piece && test_piece < 32 ) && pieces[test_piece].txt == 'N'){
                return 0;
            }
        }
        tmp.x= -1;
        tmp.y= 2;
        // if not on the board
        if ((piece_x + tmp.x < 0) || (7 < piece_x + tmp.x) || (tmp.y < 0) || (7 < tmp.y)){
            test_piece = grid[piece_x+tmp.x+tmp.y*8];
            if (( 16 <= test_piece && test_piece < 32 ) && pieces[test_piece].txt == 'N'){
                return 0;
            }
        }
        tmp.x= -2;
        tmp.y= 1;
        // if not on the board
        if ((piece_x + tmp.x < 0) || (7 < piece_x + tmp.x) || (tmp.y < 0) || (7 < tmp.y)){
            test_piece = grid[piece_x+tmp.x+tmp.y*8];
            if (( 16 <= test_piece && test_piece < 32 ) && pieces[test_piece].txt == 'N'){
                return 0;
            }
        }
    }
    return 1;
}

int possible_to_castle_white_right(Piece pieces[], int grid[]){
    // call this only if there is free space for castling
    // grid[5]==32 && grid[6]==32
    int test_piece;
    Move tmp={};
    for (int piece_x=5; piece_x<7; piece_x++){
        for (int x=1; x<8; x++){
            //BISHOP
            tmp.x=x;
            tmp.y=x;
            // if not on the board
            if ((piece_x + tmp.x < 0) || (7 < piece_x + tmp.x) || (tmp.y < 0) || (7 < tmp.y)){
                break;
            }
            test_piece = grid[piece_x+tmp.x+tmp.y*8];
            if ( 16 <= test_piece && test_piece < 32 ){
                if (x==1){
                    if (pieces[test_piece].txt == 'K' || pieces[test_piece].txt == 'p'){
                        return 0;
                    }
                }
                if (pieces[test_piece].txt == 'B' || pieces[test_piece].txt == 'Q'){
                    return 0;
                }
            }
            if(test_piece!=32){
                break;
            }
        }
        for (int x=1; x<8; x++){
            //BISHOP
            tmp.x=-x;
            tmp.y=x;
            // if not on the board
            if ((piece_x + tmp.x < 0) || (7 < piece_x + tmp.x) || (tmp.y < 0) || (7 < tmp.y)){
                break;
            }
            test_piece = grid[piece_x+tmp.x+tmp.y*8];
            if ( 16 <= test_piece && test_piece < 32 ){
                if (x==1){
                    if (pieces[test_piece].txt == 'K' || pieces[test_piece].txt == 'p'){
                        return 0;
                    }
                }
                if (pieces[test_piece].txt == 'B' || pieces[test_piece].txt == 'Q'){
                    return 0;
                }
            }
            if(test_piece!=32){
                break;
            }
        }
        for (int x=1; x<8; x++){
            //ROOK
            tmp.x=0;
            tmp.y=x;
            // if not on the board
            if ((piece_x + tmp.x < 0) || (7 < piece_x + tmp.x) || (tmp.y < 0) || (7 < tmp.y)){
                break;
            }
            test_piece = grid[piece_x+tmp.x+tmp.y*8];
            if ( 16 <= test_piece && test_piece < 32 ){
                if (x==1){
                    if (pieces[test_piece].txt == 'K'){
                        return 0;
                    }
                }
                if (pieces[test_piece].txt == 'R' || pieces[test_piece].txt == 'Q'){
                    return 0;
                }
            }
            if(test_piece!=32){
                break;
            }
        }
        //Knight
        tmp.x= 2;
        tmp.y= 1;
        // if not on the board
        if ((piece_x + tmp.x < 0) || (7 < piece_x + tmp.x) || (tmp.y < 0) || (7 < tmp.y)){
            test_piece = grid[piece_x+tmp.x+tmp.y*8];
            if (( 16 <= test_piece && test_piece < 32 ) && pieces[test_piece].txt == 'N'){
                return 0;
            }
        }
        tmp.x= 1;
        tmp.y= 2;
        // if not on the board
        if ((piece_x + tmp.x < 0) || (7 < piece_x + tmp.x) || (tmp.y < 0) || (7 < tmp.y)){
            test_piece = grid[piece_x+tmp.x+tmp.y*8];
            if (( 16 <= test_piece && test_piece < 32 ) && pieces[test_piece].txt == 'N'){
                return 0;
            }
        }
        tmp.x= -1;
        tmp.y= 2;
        // if not on the board
        if ((piece_x + tmp.x < 0) || (7 < piece_x + tmp.x) || (tmp.y < 0) || (7 < tmp.y)){
            test_piece = grid[piece_x+tmp.x+tmp.y*8];
            if (( 16 <= test_piece && test_piece < 32 ) && pieces[test_piece].txt == 'N'){
                return 0;
            }
        }
        tmp.x= -2;
        tmp.y= 1;
        // if not on the board
        if ((piece_x + tmp.x < 0) || (7 < piece_x + tmp.x) || (tmp.y < 0) || (7 < tmp.y)){
            test_piece = grid[piece_x+tmp.x+tmp.y*8];
            if (( 16 <= test_piece && test_piece < 32 ) && pieces[test_piece].txt == 'N'){
                return 0;
            }
        }
    }
    return 1;
}

int possible_to_castle_black_left(Piece pieces[], int grid[]){
    // call this only if there is free space for castling
    // grid[57]==32 && grid[58]==32 && grid[59]==32
    int test_piece;
    Move tmp={};
    for (int piece_x=1; piece_x<4; piece_x++){
        for (int x=1; x<8; x++){
            //BISHOP
            tmp.x=x;
            tmp.y=-x;
            // if not on the board
            if ((piece_x + tmp.x < 0) || (7 < piece_x + tmp.x) || (7 + tmp.y < 0) || (7 < 7 + tmp.y)){
                break;
            }
            test_piece = grid[piece_x+tmp.x+(7+tmp.y)*8];
            if ( 0 <= test_piece && test_piece < 16 ){
                if (x==1){
                    if (pieces[test_piece].txt == 'K' || pieces[test_piece].txt == 'p'){
                        return 0;
                    }
                }
                if (pieces[test_piece].txt == 'B' || pieces[test_piece].txt == 'Q'){
                    return 0;
                }
            }
            if(test_piece!=32){
                break;
            }
        }
        for (int x=1; x<8; x++){
            //BISHOP
            tmp.x=-x;
            tmp.y=-x;
            // if not on the board
            if ((piece_x + tmp.x < 0) || (7 < piece_x + tmp.x) || (7 + tmp.y < 0) || (7 < 7 + tmp.y)){
                break;
            }
            test_piece = grid[piece_x+tmp.x+(7+tmp.y)*8];
            if ( 0 <= test_piece && test_piece < 16 ){
                if (x==1){
                    if (pieces[test_piece].txt == 'K' || pieces[test_piece].txt == 'p'){
                        return 0;
                    }
                }
                if (pieces[test_piece].txt == 'B' || pieces[test_piece].txt == 'Q'){
                    return 0;
                }
            }
            if(test_piece!=32){
                break;
            }
        }
        for (int x=1; x<8; x++){
            //ROOK
            tmp.x=0;
            tmp.y=-x;
            // if not on the board
            if ((piece_x + tmp.x < 0) || (7 < piece_x + tmp.x) || (7 + tmp.y < 0) || (7 < 7 + tmp.y)){
                break;
            }
            test_piece = grid[piece_x+tmp.x+(7+tmp.y)*8];
            if ( 0 <= test_piece && test_piece < 16 ){
                if (x==1){
                    if (pieces[test_piece].txt == 'K'){
                        return 0;
                    }
                }
                if (pieces[test_piece].txt == 'R' || pieces[test_piece].txt == 'Q'){
                    return 0;
                }
            }
            if(test_piece!=32){
                break;
            }
        }
        //Knight
        tmp.x= 2;
        tmp.y= -1;
        // if not on the board
        if ((piece_x + tmp.x < 0) || (7 < piece_x + tmp.x) || (7 + tmp.y < 0) || (7 < 7 + tmp.y)){
            test_piece = grid[piece_x+tmp.x+(7+tmp.y)*8];
            if (( 0 <= test_piece && test_piece < 16 ) && pieces[test_piece].txt == 'N'){
                return 0;
            }
        }
        tmp.x= 1;
        tmp.y= -2;
        // if not on the board
        if ((piece_x + tmp.x < 0) || (7 < piece_x + tmp.x) || (7 + tmp.y < 0) || (7 < 7 + tmp.y)){
            test_piece = grid[piece_x+tmp.x+(7+tmp.y)*8];
            if (( 0 <= test_piece && test_piece < 16 ) && pieces[test_piece].txt == 'N'){
                return 0;
            }
        }
        tmp.x= -1;
        tmp.y= -2;
        // if not on the board
        if ((piece_x + tmp.x < 0) || (7 < piece_x + tmp.x) || (7 + tmp.y < 0) || (7 < 7 + tmp.y)){
            test_piece = grid[piece_x+tmp.x+(7+tmp.y)*8];
            if (( 0 <= test_piece && test_piece < 16 ) && pieces[test_piece].txt == 'N'){
                return 0;
            }
        }
        tmp.x= -2;
        tmp.y= -1;
        // if not on the board
        if ((piece_x + tmp.x < 0) || (7 < piece_x + tmp.x) || (7 + tmp.y < 0) || (7 < 7 + tmp.y)){
            test_piece = grid[piece_x+tmp.x+(7+tmp.y)*8];
            if (( 0 <= test_piece && test_piece < 16 ) && pieces[test_piece].txt == 'N'){
                return 0;
            }
        }
    }
    return 1;
}

int possible_to_castle_black_right(Piece pieces[], int grid[]){
    // call this only if there is free space for castling
    // grid[61]==32 && grid[62]==32
    int test_piece;
    Move tmp={};
    for (int piece_x=5; piece_x<7; piece_x++){
        for (int x=1; x<8; x++){
            //BISHOP
            tmp.x=x;
            tmp.y=-x;
            // if not on the board
            if ((piece_x + tmp.x < 0) || (7 < piece_x + tmp.x) || (7 + tmp.y < 0) || (7 < 7 + tmp.y)){
                break;
            }
            test_piece = grid[piece_x+tmp.x+(7+tmp.y)*8];
            if ( 0 <= test_piece && test_piece < 16 ){
                if (x==1){
                    if (pieces[test_piece].txt == 'K' || pieces[test_piece].txt == 'p'){
                        return 0;
                    }
                }
                if (pieces[test_piece].txt == 'B' || pieces[test_piece].txt == 'Q'){
                    return 0;
                }
            }
            if(test_piece!=32){
                break;
            }
        }
        for (int x=1; x<8; x++){
            //BISHOP
            tmp.x=-x;
            tmp.y=-x;
            // if not on the board
            if ((piece_x + tmp.x < 0) || (7 < piece_x + tmp.x) || (7 + tmp.y < 0) || (7 < 7 + tmp.y)){
                break;
            }
            test_piece = grid[piece_x+tmp.x+(7+tmp.y)*8];
            if ( 0 <= test_piece && test_piece < 16 ){
                if (x==1){
                    if (pieces[test_piece].txt == 'K' || pieces[test_piece].txt == 'p'){
                        return 0;
                    }
                }
                if (pieces[test_piece].txt == 'B' || pieces[test_piece].txt == 'Q'){
                    return 0;
                }
            }
            if(test_piece!=32){
                break;
            }
        }
        for (int x=1; x<8; x++){
            //ROOK
            tmp.x=0;
            tmp.y=-x;
            // if not on the board
            if ((piece_x + tmp.x < 0) || (7 < piece_x + tmp.x) || (7 + tmp.y < 0) || (7 < 7 + tmp.y)){
                break;
            }
            test_piece = grid[piece_x+tmp.x+(7+tmp.y)*8];
            if ( 0 <= test_piece && test_piece < 16 ){
                if (x==1){
                    if (pieces[test_piece].txt == 'K'){
                        return 0;
                    }
                }
                if (pieces[test_piece].txt == 'R' || pieces[test_piece].txt == 'Q'){
                    return 0;
                }
            }
            if(test_piece!=32){
                break;
            }
        }
        //Knight
        tmp.x= 2;
        tmp.y= -1;
        // if not on the board
        if ((piece_x + tmp.x < 0) || (7 < piece_x + tmp.x) || (7 + tmp.y < 0) || (7 < 7 + tmp.y)){
            test_piece = grid[piece_x+tmp.x+(7+tmp.y)*8];
            if (( 0 <= test_piece && test_piece < 16 ) && pieces[test_piece].txt == 'N'){
                return 0;
            }
        }
        tmp.x= 1;
        tmp.y= -2;
        // if not on the board
        if ((piece_x + tmp.x < 0) || (7 < piece_x + tmp.x) || (7 + tmp.y < 0) || (7 < 7 + tmp.y)){
            test_piece = grid[piece_x+tmp.x+(7+tmp.y)*8];
            if (( 0 <= test_piece && test_piece < 16 ) && pieces[test_piece].txt == 'N'){
                return 0;
            }
        }
        tmp.x= -1;
        tmp.y= -2;
        // if not on the board
        if ((piece_x + tmp.x < 0) || (7 < piece_x + tmp.x) || (7 + tmp.y < 0) || (7 < 7 + tmp.y)){
            test_piece = grid[piece_x+tmp.x+(7+tmp.y)*8];
            if (( 0 <= test_piece && test_piece < 16 ) && pieces[test_piece].txt == 'N'){
                return 0;
            }
        }
        tmp.x= -2;
        tmp.y= -1;
        // if not on the board
        if ((piece_x + tmp.x < 0) || (7 < piece_x + tmp.x) || (7 + tmp.y < 0) || (7 < 7 + tmp.y)){
            test_piece = grid[piece_x+tmp.x+(7+tmp.y)*8];
            if (( 0 <= test_piece && test_piece < 16 ) && pieces[test_piece].txt == 'N'){
                return 0;
            }
        }
    }
    return 1;
}


int move_defend_king(Piece pieces[], int grid[], Board *board, Move *move, short *white){
    Piece undo_piece = {};
    Move tmp = {};
    apply_move(pieces,grid,board,move, &undo_piece);
    int not_attacked = not_defended(23-8*(*white), pieces, grid, &tmp, white);
    undo_move(pieces,grid,board,move, &undo_piece);
    board->en_passant=0;
    return not_attacked;
}

void possible_moves(short white, Piece *pieces, int grid[], Board *board, Move possible[], int *fill){
    Move tmp = {};
    int test_piece;

    for (int i = 8 - white*8 ; i < 24 - white*8 ; i++) {
        //printf("%d\n",i);
        if (pieces[i].value == 0){
            continue;
        }
        tmp.piece=i;
        if (pieces[i].txt == 'p'){
            // move forward
            tmp.x=0;
            tmp.y=1*white;
            if (is_on_the_board(&pieces[i],&tmp) && piece_there(&pieces[i], grid, &tmp)==32 && move_defend_king(pieces, grid, board, &tmp, &white))
            {
                // transform into another piece
                if ((white == 1 && pieces[i].y == 6) || (white == -1 && pieces[i].y == 1)){
                    tmp.transform = 'Q';
                    possible[*fill]=tmp;
                    *fill += 1;
                    tmp.transform = 'R';
                    possible[*fill]=tmp;
                    *fill += 1;
                    tmp.transform = 'B';
                    possible[*fill]=tmp;
                    *fill += 1;
                    tmp.transform = 'N';
                    possible[*fill]=tmp;
                    *fill += 1;
                    tmp.transform = 0;
                }
                else{
                    possible[*fill]=tmp;
                    *fill += 1;
                }
            }
            // move forward +2
            tmp.x=0;
            tmp.y=1*white;
            if ((white==1 && pieces[i].y==1) || (white==-1 && pieces[i].y==6)){
                if (piece_there(&pieces[i], grid, &tmp)==32){
                    tmp.y=2*white;
                    if (piece_there(&pieces[i], grid, &tmp)==32 && move_defend_king(pieces, grid, board, &tmp, &white))
                    {
                        possible[*fill]=tmp;
                        *fill += 1;
                    }
                }
            }
            // eat on the left (for white, and right for black)
            tmp.x=-1;
            tmp.y=1*white;
            if (is_on_the_board(&pieces[i],&tmp) && opponent_piece_there(&pieces[i], grid, &tmp, &white) && move_defend_king(pieces, grid, board, &tmp, &white))
            {
                // transform into another piece
                if ((white == 1 && pieces[i].y == 6) || (white == -1 && pieces[i].y == 1)){
                    tmp.transform = 'Q';
                    possible[*fill]=tmp;
                    *fill += 1;
                    tmp.transform = 'R';
                    possible[*fill]=tmp;
                    *fill += 1;
                    tmp.transform = 'B';
                    possible[*fill]=tmp;
                    *fill += 1;
                    tmp.transform = 'N';
                    possible[*fill]=tmp;
                    *fill += 1;
                    tmp.transform = 0;
                }
                possible[*fill]=tmp;
                *fill += 1;
            }
            // eat on the right (for white, and left for black)
            tmp.x=1;
            tmp.y=1*white;
            if (is_on_the_board(&pieces[i],&tmp) && opponent_piece_there(&pieces[i], grid, &tmp, &white) && move_defend_king(pieces, grid, board, &tmp, &white))
            {
                // transform into another piece
                if ((white == 1 && pieces[i].y == 6) || (white == -1 && pieces[i].y == 1)){
                    tmp.transform = 'Q';
                    possible[*fill]=tmp;
                    *fill += 1;
                    tmp.transform = 'R';
                    possible[*fill]=tmp;
                    *fill += 1;
                    tmp.transform = 'B';
                    possible[*fill]=tmp;
                    *fill += 1;
                    tmp.transform = 'N';
                    possible[*fill]=tmp;
                    *fill += 1;
                    tmp.transform = 0;
                }
                possible[*fill]=tmp;
                *fill += 1;
            }
            //en passant
            if (board->en_passant){
                // left
                tmp.x=-1;
                tmp.y=1*white;
                if ((pieces[i].x == pieces[board->en_passant].x + 1) && (pieces[i].y == pieces[board->en_passant].y)){
                    possible[*fill]=tmp;
                    *fill += 1;
                }
                // right
                tmp.x=+1;
                tmp.y=1*white;
                if ((pieces[i].x == pieces[board->en_passant].x - 1) && (pieces[i].y == pieces[board->en_passant].y)){
                    possible[*fill]=tmp;
                    *fill += 1;
                }
            }
        }
        else if (pieces[i].txt == 'N'){
            tmp.x=1;
            tmp.y=2;
            if (is_on_the_board(&pieces[i],&tmp) && opponent_or_free_there(&pieces[i], grid, &tmp, &white) && move_defend_king(pieces, grid, board, &tmp, &white)){
                possible[*fill]=tmp;
                *fill += 1;
            }
            tmp.x=-1;
            tmp.y=2;
            if (is_on_the_board(&pieces[i],&tmp) && opponent_or_free_there(&pieces[i], grid, &tmp, &white) && move_defend_king(pieces, grid, board, &tmp, &white)){
                possible[*fill]=tmp;
                *fill += 1;
            }
            tmp.x=1;
            tmp.y=-2;
            if (is_on_the_board(&pieces[i],&tmp) && opponent_or_free_there(&pieces[i], grid, &tmp, &white) && move_defend_king(pieces, grid, board, &tmp, &white)){
                possible[*fill]=tmp;
                *fill += 1;
            }
            tmp.x=-1;
            tmp.y=-2;
            if (is_on_the_board(&pieces[i],&tmp) && opponent_or_free_there(&pieces[i], grid, &tmp, &white) && move_defend_king(pieces, grid, board, &tmp, &white)){
                possible[*fill]=tmp;
                *fill += 1;
            }
            tmp.x=2;
            tmp.y=1;
            if (is_on_the_board(&pieces[i],&tmp) && opponent_or_free_there(&pieces[i], grid, &tmp, &white) && move_defend_king(pieces, grid, board, &tmp, &white)){
                possible[*fill]=tmp;
                *fill += 1;
            }
            tmp.x=2;
            tmp.y=-1;
            if (is_on_the_board(&pieces[i],&tmp) && opponent_or_free_there(&pieces[i], grid, &tmp, &white) && move_defend_king(pieces, grid, board, &tmp, &white)){
                possible[*fill]=tmp;
                *fill += 1;
            }
            tmp.x=-2;
            tmp.y=1;
            if (is_on_the_board(&pieces[i],&tmp) && opponent_or_free_there(&pieces[i], grid, &tmp, &white) && move_defend_king(pieces, grid, board, &tmp, &white)){
                possible[*fill]=tmp;
                *fill += 1;
            }
            tmp.x=-2;
            tmp.y=-1;
            if (is_on_the_board(&pieces[i],&tmp) && opponent_or_free_there(&pieces[i], grid, &tmp, &white) && move_defend_king(pieces, grid, board, &tmp, &white)){
                possible[*fill]=tmp;
                *fill += 1;
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
                if (((8 + white*8 <= test_piece && test_piece < 24 + white*8) || test_piece==32) && move_defend_king(pieces, grid, board, &tmp, &white)){
                    possible[*fill]=tmp;
                    *fill += 1;
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
                if (((8 + white*8 <= test_piece && test_piece < 24 + white*8) || test_piece==32) && move_defend_king(pieces, grid, board, &tmp, &white)){
                    possible[*fill]=tmp;
                    *fill += 1;
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
                if (((8 + white*8 <= test_piece && test_piece < 24 + white*8) || test_piece==32) && move_defend_king(pieces, grid, board, &tmp, &white)){
                    possible[*fill]=tmp;
                    *fill += 1;
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
                if (((8 + white*8 <= test_piece && test_piece < 24 + white*8) || test_piece==32) && move_defend_king(pieces, grid, board, &tmp, &white)){
                    possible[*fill]=tmp;
                    *fill += 1;
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
                if (((8 + white*8 <= test_piece && test_piece < 24 + white*8) || test_piece==32) && move_defend_king(pieces, grid, board, &tmp, &white)){
                    possible[*fill]=tmp;
                    *fill += 1;
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
                if (((8 + white*8 <= test_piece && test_piece < 24 + white*8) || test_piece==32) && move_defend_king(pieces, grid, board, &tmp, &white)){
                    possible[*fill]=tmp;
                    *fill += 1;
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
                if (((8 + white*8 <= test_piece && test_piece < 24 + white*8) || test_piece==32) && move_defend_king(pieces, grid, board, &tmp, &white)){
                    possible[*fill]=tmp;
                    *fill += 1;
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
                if (((8 + white*8 <= test_piece && test_piece < 24 + white*8) || test_piece==32) && move_defend_king(pieces, grid, board, &tmp, &white)){
                    possible[*fill]=tmp;
                    *fill += 1;
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
                if (((8 + white*8 <= test_piece && test_piece < 24 + white*8) || test_piece==32) && move_defend_king(pieces, grid, board, &tmp, &white)){
                    possible[*fill]=tmp;
                    *fill += 1;
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
                if (((8 + white*8 <= test_piece && test_piece < 24 + white*8) || test_piece==32) && move_defend_king(pieces, grid, board, &tmp, &white)){
                    possible[*fill]=tmp;
                    *fill += 1;
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
                if (((8 + white*8 <= test_piece && test_piece < 24 + white*8) || test_piece==32) && move_defend_king(pieces, grid, board, &tmp, &white)){
                    possible[*fill]=tmp;
                    *fill += 1;
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
                if (((8 + white*8 <= test_piece && test_piece < 24 + white*8) || test_piece==32) && move_defend_king(pieces, grid, board, &tmp, &white)){
                    possible[*fill]=tmp;
                    *fill += 1;
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
                if (((8 + white*8 <= test_piece && test_piece < 24 + white*8) || test_piece==32) && move_defend_king(pieces, grid, board, &tmp, &white)){
                    possible[*fill]=tmp;
                    *fill += 1;
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
                if (((8 + white*8 <= test_piece && test_piece < 24 + white*8) || test_piece==32) && move_defend_king(pieces, grid, board, &tmp, &white)){
                    possible[*fill]=tmp;
                    *fill += 1;
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
                if (((8 + white*8 <= test_piece && test_piece < 24 + white*8) || test_piece==32) && move_defend_king(pieces, grid, board, &tmp, &white)){
                    possible[*fill]=tmp;
                    *fill += 1;
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
                if (((8 + white*8 <= test_piece && test_piece < 24 + white*8) || test_piece==32) && move_defend_king(pieces, grid, board, &tmp, &white)){
                    possible[*fill]=tmp;
                    *fill += 1;
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
                possible[*fill]=tmp;
                *fill += 1;
            }
            tmp.x=1;
            tmp.y=1;
            if (is_on_the_board(&pieces[i],&tmp) && opponent_or_free_there(&pieces[i], grid, &tmp, &white) && not_defended(i, pieces, grid, &tmp, &white)){
                possible[*fill]=tmp;
                *fill += 1;
            }
            tmp.x=0;
            tmp.y=1;
            if (is_on_the_board(&pieces[i],&tmp) && opponent_or_free_there(&pieces[i], grid, &tmp, &white) && not_defended(i, pieces, grid, &tmp, &white)){
                possible[*fill]=tmp;
                *fill += 1;
            }
            tmp.x=-1;
            tmp.y=1;
            if (is_on_the_board(&pieces[i],&tmp) && opponent_or_free_there(&pieces[i], grid, &tmp, &white) && not_defended(i, pieces, grid, &tmp, &white)){
                possible[*fill]=tmp;
                *fill += 1;
            }
            tmp.x=-1;
            tmp.y=0;
            if (is_on_the_board(&pieces[i],&tmp) && opponent_or_free_there(&pieces[i], grid, &tmp, &white) && not_defended(i, pieces, grid, &tmp, &white)){
                possible[*fill]=tmp;
                *fill += 1;
            }
            tmp.x=-1;
            tmp.y=-1;
            if (is_on_the_board(&pieces[i],&tmp) && opponent_or_free_there(&pieces[i], grid, &tmp, &white) && not_defended(i, pieces, grid, &tmp, &white)){
                possible[*fill]=tmp;
                *fill += 1;
            }
            tmp.x=0;
            tmp.y=-1;
            if (is_on_the_board(&pieces[i],&tmp) && opponent_or_free_there(&pieces[i], grid, &tmp, &white) && not_defended(i, pieces, grid, &tmp, &white)){
                possible[*fill]=tmp;
                *fill += 1;
            }
            tmp.x=1;
            tmp.y=-1;
            if (is_on_the_board(&pieces[i],&tmp) && opponent_or_free_there(&pieces[i], grid, &tmp, &white) && not_defended(i, pieces, grid, &tmp, &white)){
                possible[*fill]=tmp;
                *fill += 1;
            }
            tmp.y=0;
            tmp.x=0;
            if (not_defended(i, pieces, grid, &tmp, &white)){
                if (white == 1){
                    if (board->castle_rights & WHITE_CASLTE_LEFT){
                        if (grid[1]==32 && grid[2]==32 && grid[3]==32){
                            if (possible_to_castle_white_left(pieces,grid)){
                                tmp.x=-2;
                                possible[*fill]=tmp;
                                *fill += 1;
                            }
                        }
                    }
                    if (board->castle_rights & WHITE_CASLTE_RIGHT)
                    {
                        if (grid[5]==32 && grid[6]==32){
                            if (possible_to_castle_white_right(pieces,grid)){
                                tmp.x=2;
                                possible[*fill]=tmp;
                                *fill += 1;
                            }
                        }
                    }
                }
                else{
                    if (board->castle_rights & BLACK_CASLTE_LEFT){
                        if (grid[57]==32 && grid[58]==32 && grid[59]==32){
                            if (possible_to_castle_black_left(pieces,grid)){
                                tmp.x=-2;
                                possible[*fill]=tmp;
                                *fill += 1;
                            }
                        }
                    }
                    if (board->castle_rights & BLACK_CASLTE_RIGHT)
                    {
                        if (grid[61]==32 && grid[62]==32){
                            if (possible_to_castle_black_right(pieces,grid)){
                                tmp.x=2;
                                possible[*fill]=tmp;
                                *fill += 1;
                            }
                        }
                    }
                }
            }
        }
    }
}

float minimax(short white, Piece *pieces, int grid[], Board *board, float alpha, float beta, int depth){
    Move possible[138] = {};
    float possible_best;
    Piece undo_piece = {};
    int fill = 0;

    if (depth == 0){
        return eval(pieces);
    }

    possible_moves(white, pieces, grid, board, possible, &fill);

    if (fill==0){
        if (not_defended(23-8*white, pieces, grid, &possible[0], &white)){
            //stalemate (possible[0] = no move)
            return 0;
        }
        return -1000*white;;
    }

    if (white==1)
    {
        for (int i=0; i < fill; i++){
            undo_piece.value=0;
            undo_piece.y=0; // used to stock if undo_move should restore castle rights
            board->en_passant=0;
            apply_move(pieces,grid,board,&possible[i], &undo_piece);
            possible_best = minimax(-white, pieces, grid, board, alpha, beta, depth - 1);
            undo_move(pieces,grid,board,&possible[i], &undo_piece);
            if (alpha < possible_best){
                alpha = possible_best;
            }
            if (beta <= alpha){
                break;
            }
        }
        return alpha;
    }
    else
    {
        for (int i=0; i < fill; i++){
            undo_piece.value=0;
            undo_piece.y=0; // used to stock if undo_move should restore castle rights
            board->en_passant=0;
            apply_move(pieces,grid,board,&possible[i], &undo_piece);
            possible_best = minimax(-white, pieces, grid, board, alpha, beta, depth - 1);
            undo_move(pieces,grid,board,&possible[i], &undo_piece);
            if (possible_best < beta){
                beta = possible_best;
            }
            if (beta <= alpha){
                break;
            }
        }
        return beta;
    }
}

void shuffle(Move *possible, int n) {
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);

        // swap arr[i] and arr[j]
        Move temp = possible[i];
        possible[i] = possible[j];
        possible[j] = temp;
    }
}

Move rnd_best_move(short white, Piece *pieces, int grid[], Board *board,  int depth){
    //minimax
    Move possible[138] = {};
    float possible_best;
    Piece undo_piece = {};
    int fill = 0;

    //first depth
    Move best = {};
    float best_value = -1000*white;
    float alpha = -100000;
    float beta = 100000;

    if (depth == 0){
        best_value = eval(pieces);
        //printf("value: %f\n", best_value);
        return best;
    }

    possible_moves(white, pieces, grid, board, possible, &fill);
    shuffle(possible, fill);

    if (fill==0){
        if (not_defended(23-8*white, pieces, grid, &possible[0], &white)){
            //stalemate (possible[0] = no move)
            best_value = 0;
        }
        best_value = -1000*white;;
        //printf("value: %f\n", best_value);
        return best;
    }

    if (white==1)
    {
        for (int i=0; i < fill; i++){
            undo_piece.value=0;
            undo_piece.y=0; // used to stock if undo_move should restore castle rights
            board->en_passant=0;
            apply_move(pieces,grid,board,&possible[i], &undo_piece);
            //possible_best = minimax(-white, pieces, grid, board, alpha, beta, depth - 1) + (((double)rand() / RAND_MAX) - 0.5)/10;
            possible_best = minimax(-white, pieces, grid, board, alpha, beta, depth - 1);
            undo_move(pieces,grid,board,&possible[i], &undo_piece);
            if (alpha < possible_best){
                alpha = possible_best;
                best = possible[i];
                best_value = possible_best;
            }
            if (beta < alpha){
                break; // useless beta = 100000 
            }
        }
    }
    else
    {
        for (int i=0; i < fill; i++){
            undo_piece.value=0;
            undo_piece.y=0; // used to stock if undo_move should restore castle rights
            board->en_passant=0;
            apply_move(pieces,grid,board,&possible[i], &undo_piece);
            //possible_best = minimax(-white, pieces, grid, board, alpha, beta, depth - 1) + (((double)rand() / RAND_MAX) - 0.5)/10;
            possible_best = minimax(-white, pieces, grid, board, alpha, beta, depth - 1);
            undo_move(pieces,grid,board,&possible[i], &undo_piece);
            if (possible_best < beta){
                beta = possible_best;
                best = possible[i];
                best_value = possible_best;
            }
            if (beta < alpha){
                break; // useless alpha = -100000 
            }
        }
    }
    printf("value: %f\n", best_value);
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

    Board board = {0,(WHITE_CASLTE_LEFT | WHITE_CASLTE_RIGHT | BLACK_CASLTE_LEFT | BLACK_CASLTE_RIGHT)};

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
    srand(time(NULL));  // Seed
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
        printf("board");
        //print_bits(board.castle_rights);
        //printf("en passant: %d\n",board.en_passant);
        printf("\n");
        white=-white;
    }
    return 0;
}