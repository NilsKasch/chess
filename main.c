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

void apply_move(Piece *pieces, int grid[], Move *move, Piece *undo_piece, int *king_hit){
    if (move->x != 0 || move->y != 0) //safeguard TODO:remove it
    //printf("bare move: %c%d%d\n",pieces[move->piece].txt, move->x, move->y);
    {
        int target=grid[(pieces[move->piece].x+(move->x))+(pieces[move->piece].y+(move->y))*8];
        if (target!=32){
            undo_piece->txt = pieces[target].txt;
            undo_piece->value = pieces[target].value;
            undo_piece->x = target;
            undo_piece->y = target;
            pieces[target].value=-1;
            if (target == 15 || target == 31){
                *king_hit=1;
            }
        }
        pieces[move->piece].x += move->x;
        pieces[move->piece].y += move->y;
        update_grid(pieces, grid);

    }
}

void undo_move(Piece *pieces, int grid[], Move *move, Piece *undo_piece){
    if (move->x != 0 || move->y != 0) //safeguard
    //printf("bare move: %c%d%d\n",pieces[move->piece].txt, move->x, move->y);
    {
        if (undo_piece->value > 0){
            pieces[undo_piece->x].txt = undo_piece->txt;
            pieces[undo_piece->x].value = undo_piece->value;
            pieces[undo_piece->x].x = pieces[move->piece].x;
            pieces[undo_piece->x].y = pieces[move->piece].y;
        }
        pieces[move->piece].x -= move->x;
        pieces[move->piece].y -= move->y;
        update_grid(pieces, grid);
    }
}

void keep(Move *best, Move *tmp, short *white){
    if ((best->value)*(*white) < (tmp->value)*(*white))
    {
        best->value = tmp->value;
        best->piece = tmp->piece;
        best->x = tmp->x;
        best->y = tmp->y;
    }
    else if ((best->value) == (tmp->value))
    {
        best->value = tmp->value;
        best->piece = tmp->piece;
        best->x = tmp->x;
        best->y = tmp->y;
    }
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

int sign(int a){
    return a >= 0 ? 1 : -1;
}

int clean_full_way(Piece *piece, int grid[], Move *move){
    for (int x=0; x <= abs(move->x); x++){
        for (int y=0; y <= abs(move->y); y++){
            if ((x!=0 || y!=0) && grid[piece->x+x*sign(move->x)+(piece->y+y*sign(move->y))*8]!=32){
                return 0;
            }
        }
    }
    return 1;
}

int max(int a, int b){
    return a > b ? a : b;
}

int clean_way_diag(Piece *piece, int grid[], Move *move){
    for (int x=1; x <= max(abs(move->x)-1,0); x++){
        if (grid[piece->x+x*sign(move->x)+(piece->y+x*sign(move->y))*8]!=32){
            return 0;
        }
    }
    return 1;
}

int clean_way(Piece *piece, int grid[], Move *move){
    for (int x=0; x <= max(abs(move->x)-1,0); x++){
        for (int y=0; y <= max(abs(move->y)-1,0); y++){
            if ((x!=0 || y!=0) && grid[piece->x+x*sign(move->x)+(piece->y+y*sign(move->y))*8]!=32){
                return 0;
            }
        }
    }
    return 1;
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
    int target_x = pieces[piece].x+move->x;
    int target_y = pieces[piece].y+move->y;
    Move tmp={};
    for (int i = 8 + *white*8 ; i < 24 + *white*8 ; i++) {
        //printf("%d\n",i);
        if (pieces[i].value < 0){
            continue;
        }
        if (pieces[i].txt == 'p'){
            if (target_x == pieces[i].x - 1 || target_x == pieces[i].x + 1){
                if(target_y == pieces[i].y + 1){
                    return  0;
                }
            }
        }
        else if (pieces[i].txt == 'N'){
            if (target_x == pieces[i].x - 1 || target_x == pieces[i].x + 1){
                if(target_y == pieces[i].y - 2 || target_y == pieces[i].y + 2){
                    return  0;
                }
            }
            if (target_x == pieces[i].x - 2 || target_x == pieces[i].x + 2){
                if(target_y == pieces[i].y - 1 || target_y == pieces[i].y + 1){
                    return  0;
                }
            }
        }
        else if (pieces[i].txt == 'B'){
            for (int x=1; x<8; x++){
                if (target_x == pieces[i].x + x || target_x == pieces[i].x - x){
                    if(target_y == pieces[i].y + x || target_y == pieces[i].y - x){
                        tmp.piece=i;
                        tmp.x=target_x-pieces[i].x;
                        tmp.y=target_y-pieces[i].y;
                        if (clean_way_diag(&pieces[i], grid, &tmp)){
                            return 0;
                        }
                        else{
                            break;
                        }
                    }
                }
            }
        }
        else if (pieces[i].txt == 'R'){
            for (int x=1; x<8; x++){
                if ((target_x == pieces[i].x + x || target_x == pieces[i].x - x) && target_y == pieces[i].y){
                    tmp.piece=i;
                    tmp.x=target_x-pieces[i].x;
                    tmp.y=target_y-pieces[i].y;
                    if (clean_way(&pieces[i], grid, &tmp)){
                        return 0;
                    }
                    else{
                        break;
                    }
                }
                if ((target_y == pieces[i].y + x || target_y == pieces[i].y - x) && target_x == pieces[i].x){
                    tmp.piece=i;
                    tmp.x=target_x-pieces[i].x;
                    tmp.y=target_y-pieces[i].y;
                    if (clean_way(&pieces[i], grid, &tmp)){
                        return 0;
                    }
                    else{
                        break;
                    }
                }
            }
        }
        else if (pieces[i].txt == 'Q'){
            for (int x=1; x<8; x++){
                //rook
                if ((target_x == pieces[i].x + x || target_x == pieces[i].x - x) && target_y == pieces[i].y){
                    tmp.piece=i;
                    tmp.x=target_x-pieces[i].x;
                    tmp.y=target_y-pieces[i].y;
                    if (clean_way(&pieces[i], grid, &tmp)){
                        return 0;
                    }
                    else{
                        break;
                    }
                }
                if ((target_y == pieces[i].y + x || target_y == pieces[i].y - x) && target_x == pieces[i].x){
                    tmp.piece=i;
                    tmp.x=target_x-pieces[i].x;
                    tmp.y=target_y-pieces[i].y;
                    if (clean_way(&pieces[i], grid, &tmp)){
                        return 0;
                    }
                    else{
                        break;
                    }
                }
                //bishop
                if (target_x == pieces[i].x + x || target_x == pieces[i].x - x){
                    if(target_y == pieces[i].y + x || target_y == pieces[i].y - x){
                        tmp.piece=i;
                        tmp.x=target_x-pieces[i].x;
                        tmp.y=target_y-pieces[i].y;
                        if (clean_way_diag(&pieces[i], grid, &tmp)){
                            return 0;
                        }
                        else{
                            break;
                        }
                    }
                }
            }
        }
        else if (pieces[i].txt == 'K'){
            //rook
            if ((target_x == pieces[i].x + 1 || target_x == pieces[i].x - 1) && target_y == pieces[i].y){
                return 0;
            }
            if ((target_y == pieces[i].y + 1 || target_y == pieces[i].y - 1) && target_x == pieces[i].x){
                return 0;
            }
            //bishop
            if (target_x == pieces[i].x + 1 || target_x == pieces[i].x - 1){
                if(target_y == pieces[i].y + 1 || target_y == pieces[i].y - 1){
                    return 0;
                }
            }
        }
    }
    return 1;
}

int move_defend_king(Piece pieces[], int grid[], Move *move, short *white){
    int king_hit = 0;
    Piece undo_piece = {};
    Move tmp = {};
    apply_move(pieces,grid,move, &undo_piece, &king_hit);
    int defended = not_defended(23-8*(*white), pieces, grid, &tmp, white);
    undo_move(pieces,grid,move, &undo_piece);
    return defended;
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
    int king_hit = 0;

    undo_piece.value=-1;
    best.value=-1000*white;
    apply_move(pieces,grid,&move, &undo_piece, &king_hit);
    if (depth == 0 || king_hit){
        best.piece=move.piece;
        best.x=move.x;
        best.y=move.y;
        best.value=eval(pieces);
        undo_move(pieces,grid,&move, &undo_piece);
        return best;
    }
    if (!not_defended(23-8*white, pieces, grid, &tmp, &white))
    {
        // king under attack
        for (int i = 8 - white*8 ; i < 24 - white*8 ; i++) {
            //printf("%d\n",i);
            if (pieces[i].value < 0){
                continue;
            }
            tmp.piece=i;
            if (pieces[i].txt == 'p'){
                // move forward
                tmp.x=0;int defended = 0;
                tmp.y=1*white;
                if (is_on_the_board(&pieces[i],&tmp) && piece_there(&pieces[i], grid, &tmp)==32 && move_defend_king(pieces, grid, &tmp, &white))
                {
                    tmp.value = next(-white,pieces, grid, tmp, depth - 1).value;
                    possible[fill]=tmp;
                    fill += 1;
                }
                // move forward +2
                if ((white==1 && pieces[i].y==1) || (white==-1 && pieces[i].y==6)){
                    tmp.x=0;
                    tmp.y=2*white;
                    if (is_on_the_board(&pieces[i],&tmp) && clean_full_way(&pieces[i], grid, &tmp) && move_defend_king(pieces, grid, &tmp, &white))
                    {
                        tmp.value = next(-white,pieces, grid, tmp, depth - 1).value;
                        possible[fill]=tmp;
                        fill += 1;
                    }
                }
                // eat on the left (for white, and right for black)
                tmp.x=-1;
                tmp.y=1*white;
                if (is_on_the_board(&pieces[i],&tmp) && opponent_piece_there(&pieces[i], grid, &tmp, &white) && move_defend_king(pieces, grid, &tmp, &white))
                {
                    tmp.value = next(-white,pieces, grid, tmp, depth - 1).value;
                    possible[fill]=tmp;
                    fill += 1;
                }
                // eat on the right (for white, and left for black)
                tmp.x=1;
                tmp.y=1*white;
                if (is_on_the_board(&pieces[i],&tmp) && opponent_piece_there(&pieces[i], grid, &tmp, &white) && move_defend_king(pieces, grid, &tmp, &white))
                {
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
                for (int x=0; x<8; x++){
                    tmp.x=x;
                    tmp.y=x;
                    if (!is_on_the_board(&pieces[i],&tmp)){
                        break;
                    }
                    if (clean_way_diag(&pieces[i], grid, &tmp) && opponent_or_free_there(&pieces[i], grid, &tmp, &white) && move_defend_king(pieces, grid, &tmp, &white)){
                        tmp.value = next(-white,pieces, grid, tmp, depth - 1).value;
                        possible[fill]=tmp;
                        fill += 1;
                    }
                }
                for (int x=0; x<8; x++){
                    tmp.x=-x;
                    tmp.y=x;
                    if (!is_on_the_board(&pieces[i],&tmp)){
                        break;
                    }
                    if (clean_way_diag(&pieces[i], grid, &tmp) && opponent_or_free_there(&pieces[i], grid, &tmp, &white) && move_defend_king(pieces, grid, &tmp, &white)){
                        tmp.value = next(-white,pieces, grid, tmp, depth - 1).value;
                        possible[fill]=tmp;
                        fill += 1;
                    }
                }
                for (int x=0; x<8; x++){
                    tmp.x=x;
                    tmp.y=-x;
                    if (!is_on_the_board(&pieces[i],&tmp)){
                        break;
                    }
                    if (clean_way_diag(&pieces[i], grid, &tmp) && opponent_or_free_there(&pieces[i], grid, &tmp, &white) && move_defend_king(pieces, grid, &tmp, &white)){
                        tmp.value = next(-white,pieces, grid, tmp, depth - 1).value;
                        possible[fill]=tmp;
                        fill += 1;
                    }
                }
                for (int x=0; x<8; x++){
                    tmp.x=-x;
                    tmp.y=-x;
                    if (!is_on_the_board(&pieces[i],&tmp)){
                        break;
                    }
                    if (clean_way_diag(&pieces[i], grid, &tmp) && opponent_or_free_there(&pieces[i], grid, &tmp, &white) && move_defend_king(pieces, grid, &tmp, &white)){
                        tmp.value = next(-white,pieces, grid, tmp, depth - 1).value;
                        possible[fill]=tmp;
                        fill += 1;
                    }
                }
            }
            else if (pieces[i].txt == 'R'){
                for (int x=0; x<8; x++){
                    tmp.x=x;
                    tmp.y=0;
                    if (!is_on_the_board(&pieces[i],&tmp)){
                        break;
                    }
                    if (clean_way(&pieces[i], grid, &tmp) && opponent_or_free_there(&pieces[i], grid, &tmp, &white) && move_defend_king(pieces, grid, &tmp, &white)){
                        tmp.value = next(-white,pieces, grid, tmp, depth - 1).value;
                        possible[fill]=tmp;
                        fill += 1;
                    }
                }
                for (int x=0; x<8; x++){
                    tmp.x=-x;
                    tmp.y=0;
                    if (!is_on_the_board(&pieces[i],&tmp)){
                        break;
                    }
                    if (clean_way(&pieces[i], grid, &tmp) && opponent_or_free_there(&pieces[i], grid, &tmp, &white) && move_defend_king(pieces, grid, &tmp, &white)){
                        tmp.value = next(-white,pieces, grid, tmp, depth - 1).value;
                        possible[fill]=tmp;
                        fill += 1;
                    }
                }
                for (int x=0; x<8; x++){
                    tmp.x=0;
                    tmp.y=x;
                    if (!is_on_the_board(&pieces[i],&tmp)){
                        break;
                    }
                    if (clean_way(&pieces[i], grid, &tmp) && opponent_or_free_there(&pieces[i], grid, &tmp, &white) && move_defend_king(pieces, grid, &tmp, &white)){
                        tmp.value = next(-white,pieces, grid, tmp, depth - 1).value;
                        possible[fill]=tmp;
                        fill += 1;
                    }
                }
                for (int x=0; x<8; x++){
                    tmp.x=0;
                    tmp.y=-x;
                    if (!is_on_the_board(&pieces[i],&tmp)){
                        break;
                    }
                    if (clean_way(&pieces[i], grid, &tmp) && opponent_or_free_there(&pieces[i], grid, &tmp, &white) && move_defend_king(pieces, grid, &tmp, &white)){
                        tmp.value = next(-white,pieces, grid, tmp, depth - 1).value;
                        possible[fill]=tmp;
                        fill += 1;
                    }
                }
            }
            else if (pieces[i].txt == 'Q'){
                //BISHOP
                for (int x=0; x<8; x++){
                    tmp.x=x;
                    tmp.y=x;
                    if (!is_on_the_board(&pieces[i],&tmp)){
                        break;
                    }
                    if (clean_way_diag(&pieces[i], grid, &tmp) && opponent_or_free_there(&pieces[i], grid, &tmp, &white) && move_defend_king(pieces, grid, &tmp, &white)){
                        tmp.value = next(-white,pieces, grid, tmp, depth - 1).value;
                        possible[fill]=tmp;
                        fill += 1;
                    }
                }
                for (int x=0; x<8; x++){
                    tmp.x=-x;
                    tmp.y=x;
                    if (!is_on_the_board(&pieces[i],&tmp)){
                        break;
                    }
                    if (clean_way_diag(&pieces[i], grid, &tmp) && opponent_or_free_there(&pieces[i], grid, &tmp, &white) && move_defend_king(pieces, grid, &tmp, &white)){
                        tmp.value = next(-white,pieces, grid, tmp, depth - 1).value;
                        possible[fill]=tmp;
                        fill += 1;
                    }
                }
                for (int x=0; x<8; x++){
                    tmp.x=x;
                    tmp.y=-x;
                    if (!is_on_the_board(&pieces[i],&tmp)){
                        break;
                    }
                    if (clean_way_diag(&pieces[i], grid, &tmp) && opponent_or_free_there(&pieces[i], grid, &tmp, &white) && move_defend_king(pieces, grid, &tmp, &white)){
                        tmp.value = next(-white,pieces, grid, tmp, depth - 1).value;
                        possible[fill]=tmp;
                        fill += 1;
                    }
                }
                for (int x=0; x<8; x++){
                    tmp.x=-x;
                    tmp.y=-x;
                    if (!is_on_the_board(&pieces[i],&tmp)){
                        break;
                    }
                    if (clean_way_diag(&pieces[i], grid, &tmp) && opponent_or_free_there(&pieces[i], grid, &tmp, &white) && move_defend_king(pieces, grid, &tmp, &white)){
                        tmp.value = next(-white,pieces, grid, tmp, depth - 1).value;
                        possible[fill]=tmp;
                        fill += 1;
                    }
                }
                //ROOK
                for (int x=0; x<8; x++){
                    tmp.x=x;
                    tmp.y=0;
                    if (!is_on_the_board(&pieces[i],&tmp)){
                        break;
                    }
                    if (clean_way(&pieces[i], grid, &tmp) && opponent_or_free_there(&pieces[i], grid, &tmp, &white) && move_defend_king(pieces, grid, &tmp, &white)){
                        tmp.value = next(-white,pieces, grid, tmp, depth - 1).value;
                        possible[fill]=tmp;
                        fill += 1;
                    }
                }
                for (int x=0; x<8; x++){
                    tmp.x=-x;
                    tmp.y=0;
                    if (!is_on_the_board(&pieces[i],&tmp)){
                        break;
                    }
                    if (clean_way(&pieces[i], grid, &tmp) && opponent_or_free_there(&pieces[i], grid, &tmp, &white) && move_defend_king(pieces, grid, &tmp, &white)){
                        tmp.value = next(-white,pieces, grid, tmp, depth - 1).value;
                        possible[fill]=tmp;
                        fill += 1;
                    }
                }
                for (int x=0; x<8; x++){
                    tmp.x=0;
                    tmp.y=x;
                    if (!is_on_the_board(&pieces[i],&tmp)){
                        break;
                    }
                    if (clean_way(&pieces[i], grid, &tmp) && opponent_or_free_there(&pieces[i], grid, &tmp, &white) && move_defend_king(pieces, grid, &tmp, &white)){
                        tmp.value = next(-white,pieces, grid, tmp, depth - 1).value;
                        possible[fill]=tmp;
                        fill += 1;
                    }
                }
                for (int x=0; x<8; x++){
                    tmp.x=0;
                    tmp.y=-x;
                    if (!is_on_the_board(&pieces[i],&tmp)){
                        break;
                    }
                    if (clean_way(&pieces[i], grid, &tmp) && opponent_or_free_there(&pieces[i], grid, &tmp, &white) && move_defend_king(pieces, grid, &tmp, &white)){
                        tmp.value = next(-white,pieces, grid, tmp, depth - 1).value;
                        possible[fill]=tmp;
                        fill += 1;
                    }
                }
            }
            else if (pieces[i].txt == 'K'){
                tmp.x=1;
                tmp.y=0;
                if (is_on_the_board(&pieces[i],&tmp) && not_defended(i, pieces, grid, &tmp, &white)){
                    tmp.value = next(-white,pieces, grid, tmp, depth - 1).value;
                    possible[fill]=tmp;
                    fill += 1;
                }
                tmp.x=1;
                tmp.y=1;
                if (is_on_the_board(&pieces[i],&tmp) && not_defended(i, pieces, grid, &tmp, &white)){
                    tmp.value = next(-white,pieces, grid, tmp, depth - 1).value;
                    possible[fill]=tmp;
                    fill += 1;
                }
                tmp.x=0;
                tmp.y=1;
                if (is_on_the_board(&pieces[i],&tmp) && not_defended(i, pieces, grid, &tmp, &white)){
                    tmp.value = next(-white,pieces, grid, tmp, depth - 1).value;
                    possible[fill]=tmp;
                    fill += 1;
                }
                tmp.x=-1;
                tmp.y=1;
                if (is_on_the_board(&pieces[i],&tmp) && not_defended(i, pieces, grid, &tmp, &white)){
                    tmp.value = next(-white,pieces, grid, tmp, depth - 1).value;
                    possible[fill]=tmp;
                    fill += 1;
                }
                tmp.x=-1;
                tmp.y=0;
                if (is_on_the_board(&pieces[i],&tmp) && not_defended(i, pieces, grid, &tmp, &white)){
                    tmp.value = next(-white,pieces, grid, tmp, depth - 1).value;
                    possible[fill]=tmp;
                    fill += 1;
                }
                tmp.x=-1;
                tmp.y=-1;
                if (is_on_the_board(&pieces[i],&tmp) && not_defended(i, pieces, grid, &tmp, &white)){
                    tmp.value = next(-white,pieces, grid, tmp, depth - 1).value;
                    possible[fill]=tmp;
                    fill += 1;
                }
                tmp.x=0;
                tmp.y=-1;
                if (is_on_the_board(&pieces[i],&tmp) && not_defended(i, pieces, grid, &tmp, &white)){
                    tmp.value = next(-white,pieces, grid, tmp, depth - 1).value;
                    possible[fill]=tmp;
                    fill += 1;
                }
                tmp.x=1;
                tmp.y=-1;
                if (is_on_the_board(&pieces[i],&tmp) && not_defended(i, pieces, grid, &tmp, &white)){
                    tmp.value = next(-white,pieces, grid, tmp, depth - 1).value;
                    possible[fill]=tmp;
                    fill += 1;
                }
            }
        }
    }
    else
    {
        // king not under attack
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
                if (is_on_the_board(&pieces[i],&tmp) && piece_there(&pieces[i], grid, &tmp)==32)
                {
                    tmp.value = next(-white,pieces, grid, tmp, depth - 1).value;
                    possible[fill]=tmp;
                    fill += 1;
                }
                // move forward +2
                if ((white==1 && pieces[i].y==1) || (white==-1 && pieces[i].y==6)){
                    tmp.x=0;
                    tmp.y=2*white;
                    if (is_on_the_board(&pieces[i],&tmp) && clean_full_way(&pieces[i], grid, &tmp))
                    {
                        tmp.value = next(-white,pieces, grid, tmp, depth - 1).value;
                        possible[fill]=tmp;
                        fill += 1;
                    }
                }
                // eat on the left (for white, and right for black)
                tmp.x=-1;
                tmp.y=1*white;
                if (is_on_the_board(&pieces[i],&tmp) && opponent_piece_there(&pieces[i], grid, &tmp, &white))
                {
                    tmp.value = next(-white,pieces, grid, tmp, depth - 1).value;
                    possible[fill]=tmp;
                    fill += 1;
                }
                // eat on the right (for white, and left for black)
                tmp.x=1;
                tmp.y=1*white;
                if (is_on_the_board(&pieces[i],&tmp) && opponent_piece_there(&pieces[i], grid, &tmp, &white))
                {
                    tmp.value = next(-white,pieces, grid, tmp, depth - 1).value;
                    possible[fill]=tmp;
                    fill += 1;
                }
            }
            else if (pieces[i].txt == 'N'){
                tmp.x=1;
                tmp.y=2;
                if (is_on_the_board(&pieces[i],&tmp) && opponent_or_free_there(&pieces[i], grid, &tmp, &white)){
                    tmp.value = next(-white,pieces, grid, tmp, depth - 1).value;
                    possible[fill]=tmp;
                    fill += 1;
                }
                tmp.x=-1;
                tmp.y=2;
                if (is_on_the_board(&pieces[i],&tmp) && opponent_or_free_there(&pieces[i], grid, &tmp, &white)){
                    tmp.value = next(-white,pieces, grid, tmp, depth - 1).value;
                    possible[fill]=tmp;
                    fill += 1;
                }
                tmp.x=1;
                tmp.y=-2;
                if (is_on_the_board(&pieces[i],&tmp) && opponent_or_free_there(&pieces[i], grid, &tmp, &white)){
                    tmp.value = next(-white,pieces, grid, tmp, depth - 1).value;
                    possible[fill]=tmp;
                    fill += 1;
                }
                tmp.x=-1;
                tmp.y=-2;
                if (is_on_the_board(&pieces[i],&tmp) && opponent_or_free_there(&pieces[i], grid, &tmp, &white)){
                    tmp.value = next(-white,pieces, grid, tmp, depth - 1).value;
                    possible[fill]=tmp;
                    fill += 1;
                }
                tmp.x=2;
                tmp.y=1;
                if (is_on_the_board(&pieces[i],&tmp) && opponent_or_free_there(&pieces[i], grid, &tmp, &white)){
                    tmp.value = next(-white,pieces, grid, tmp, depth - 1).value;
                    possible[fill]=tmp;
                    fill += 1;
                }
                tmp.x=2;
                tmp.y=-1;
                if (is_on_the_board(&pieces[i],&tmp) && opponent_or_free_there(&pieces[i], grid, &tmp, &white)){
                    tmp.value = next(-white,pieces, grid, tmp, depth - 1).value;
                    possible[fill]=tmp;
                    fill += 1;
                }
                tmp.x=-2;
                tmp.y=1;
                if (is_on_the_board(&pieces[i],&tmp) && opponent_or_free_there(&pieces[i], grid, &tmp, &white)){
                    tmp.value = next(-white,pieces, grid, tmp, depth - 1).value;
                    possible[fill]=tmp;
                    fill += 1;
                }
                tmp.x=-2;
                tmp.y=-1;
                if (is_on_the_board(&pieces[i],&tmp) && opponent_or_free_there(&pieces[i], grid, &tmp, &white)){
                    tmp.value = next(-white,pieces, grid, tmp, depth - 1).value;
                    possible[fill]=tmp;
                    fill += 1;
                }
            }
            else if (pieces[i].txt == 'B'){
                for (int x=0; x<8; x++){
                    tmp.x=x;
                    tmp.y=x;
                    if (!is_on_the_board(&pieces[i],&tmp)){
                        break;
                    }
                    if (clean_way_diag(&pieces[i], grid, &tmp) && opponent_or_free_there(&pieces[i], grid, &tmp, &white)){
                        tmp.value = next(-white,pieces, grid, tmp, depth - 1).value;
                        possible[fill]=tmp;
                        fill += 1;
                    }
                }
                for (int x=0; x<8; x++){
                    tmp.x=-x;
                    tmp.y=x;
                    if (!is_on_the_board(&pieces[i],&tmp)){
                        break;
                    }
                    if (clean_way_diag(&pieces[i], grid, &tmp) && opponent_or_free_there(&pieces[i], grid, &tmp, &white)){
                        tmp.value = next(-white,pieces, grid, tmp, depth - 1).value;
                        possible[fill]=tmp;
                        fill += 1;
                    }
                }
                for (int x=0; x<8; x++){
                    tmp.x=x;
                    tmp.y=-x;
                    if (!is_on_the_board(&pieces[i],&tmp)){
                        break;
                    }
                    if (clean_way_diag(&pieces[i], grid, &tmp) && opponent_or_free_there(&pieces[i], grid, &tmp, &white)){
                        tmp.value = next(-white,pieces, grid, tmp, depth - 1).value;
                        possible[fill]=tmp;
                        fill += 1;
                    }
                }
                for (int x=0; x<8; x++){
                    tmp.x=-x;
                    tmp.y=-x;
                    if (!is_on_the_board(&pieces[i],&tmp)){
                        break;
                    }
                    if (clean_way_diag(&pieces[i], grid, &tmp) && opponent_or_free_there(&pieces[i], grid, &tmp, &white)){
                        tmp.value = next(-white,pieces, grid, tmp, depth - 1).value;
                        possible[fill]=tmp;
                        fill += 1;
                    }
                }
            }
            else if (pieces[i].txt == 'R'){
                for (int x=0; x<8; x++){
                    tmp.x=x;
                    tmp.y=0;
                    if (!is_on_the_board(&pieces[i],&tmp)){
                        break;
                    }
                    if (clean_way(&pieces[i], grid, &tmp) && opponent_or_free_there(&pieces[i], grid, &tmp, &white)){
                        tmp.value = next(-white,pieces, grid, tmp, depth - 1).value;
                        possible[fill]=tmp;
                        fill += 1;
                    }
                }
                for (int x=0; x<8; x++){
                    tmp.x=-x;
                    tmp.y=0;
                    if (!is_on_the_board(&pieces[i],&tmp)){
                        break;
                    }
                    if (clean_way(&pieces[i], grid, &tmp) && opponent_or_free_there(&pieces[i], grid, &tmp, &white)){
                        tmp.value = next(-white,pieces, grid, tmp, depth - 1).value;
                        possible[fill]=tmp;
                        fill += 1;
                    }
                }
                for (int x=0; x<8; x++){
                    tmp.x=0;
                    tmp.y=x;
                    if (!is_on_the_board(&pieces[i],&tmp)){
                        break;
                    }
                    if (clean_way(&pieces[i], grid, &tmp) && opponent_or_free_there(&pieces[i], grid, &tmp, &white)){
                        tmp.value = next(-white,pieces, grid, tmp, depth - 1).value;
                        possible[fill]=tmp;
                        fill += 1;
                    }
                }
                for (int x=0; x<8; x++){
                    tmp.x=0;
                    tmp.y=-x;
                    if (!is_on_the_board(&pieces[i],&tmp)){
                        break;
                    }
                    if (clean_way(&pieces[i], grid, &tmp) && opponent_or_free_there(&pieces[i], grid, &tmp, &white)){
                        tmp.value = next(-white,pieces, grid, tmp, depth - 1).value;
                        possible[fill]=tmp;
                        fill += 1;
                    }
                }
            }
            else if (pieces[i].txt == 'Q'){
                //BISHOP
                for (int x=0; x<8; x++){
                    tmp.x=x;
                    tmp.y=x;
                    if (!is_on_the_board(&pieces[i],&tmp)){
                        break;
                    }
                    if (clean_way_diag(&pieces[i], grid, &tmp) && opponent_or_free_there(&pieces[i], grid, &tmp, &white)){
                        tmp.value = next(-white,pieces, grid, tmp, depth - 1).value;
                        possible[fill]=tmp;
                        fill += 1;
                    }
                }
                for (int x=0; x<8; x++){
                    tmp.x=-x;
                    tmp.y=x;
                    if (!is_on_the_board(&pieces[i],&tmp)){
                        break;
                    }
                    if (clean_way_diag(&pieces[i], grid, &tmp) && opponent_or_free_there(&pieces[i], grid, &tmp, &white)){
                        tmp.value = next(-white,pieces, grid, tmp, depth - 1).value;
                        possible[fill]=tmp;
                        fill += 1;
                    }
                }
                for (int x=0; x<8; x++){
                    tmp.x=x;
                    tmp.y=-x;
                    if (!is_on_the_board(&pieces[i],&tmp)){
                        break;
                    }
                    if (clean_way_diag(&pieces[i], grid, &tmp) && opponent_or_free_there(&pieces[i], grid, &tmp, &white)){
                        tmp.value = next(-white,pieces, grid, tmp, depth - 1).value;
                        possible[fill]=tmp;
                        fill += 1;
                    }
                }
                for (int x=0; x<8; x++){
                    tmp.x=-x;
                    tmp.y=-x;
                    if (!is_on_the_board(&pieces[i],&tmp)){
                        break;
                    }
                    if (clean_way_diag(&pieces[i], grid, &tmp) && opponent_or_free_there(&pieces[i], grid, &tmp, &white)){
                        tmp.value = next(-white,pieces, grid, tmp, depth - 1).value;
                        possible[fill]=tmp;
                        fill += 1;
                    }
                }
                //ROOK
                for (int x=0; x<8; x++){
                    tmp.x=x;
                    tmp.y=0;
                    if (!is_on_the_board(&pieces[i],&tmp)){
                        break;
                    }
                    if (clean_way(&pieces[i], grid, &tmp) && opponent_or_free_there(&pieces[i], grid, &tmp, &white)){
                        tmp.value = next(-white,pieces, grid, tmp, depth - 1).value;
                        possible[fill]=tmp;
                        fill += 1;
                    }
                }
                for (int x=0; x<8; x++){
                    tmp.x=-x;
                    tmp.y=0;
                    if (!is_on_the_board(&pieces[i],&tmp)){
                        break;
                    }
                    if (clean_way(&pieces[i], grid, &tmp) && opponent_or_free_there(&pieces[i], grid, &tmp, &white)){
                        tmp.value = next(-white,pieces, grid, tmp, depth - 1).value;
                        possible[fill]=tmp;
                        fill += 1;
                    }
                }
                for (int x=0; x<8; x++){
                    tmp.x=0;
                    tmp.y=x;
                    if (!is_on_the_board(&pieces[i],&tmp)){
                        break;
                    }
                    if (clean_way(&pieces[i], grid, &tmp) && opponent_or_free_there(&pieces[i], grid, &tmp, &white)){
                        tmp.value = next(-white,pieces, grid, tmp, depth - 1).value;
                        possible[fill]=tmp;
                        fill += 1;
                    }
                }
                for (int x=0; x<8; x++){
                    tmp.x=0;
                    tmp.y=-x;
                    if (!is_on_the_board(&pieces[i],&tmp)){
                        break;
                    }
                    if (clean_way(&pieces[i], grid, &tmp) && opponent_or_free_there(&pieces[i], grid, &tmp, &white)){
                        tmp.value = next(-white,pieces, grid, tmp, depth - 1).value;
                        possible[fill]=tmp;
                        fill += 1;
                    }
                }
            }
            else if (pieces[i].txt == 'K'){
                tmp.x=1;
                tmp.y=0;
                if (is_on_the_board(&pieces[i],&tmp) && not_defended(i, pieces, grid, &tmp, &white)){
                    tmp.value = next(-white,pieces, grid, tmp, depth - 1).value;
                    possible[fill]=tmp;
                    fill += 1;
                }
                tmp.x=1;
                tmp.y=1;
                if (is_on_the_board(&pieces[i],&tmp) && not_defended(i, pieces, grid, &tmp, &white)){
                    tmp.value = next(-white,pieces, grid, tmp, depth - 1).value;
                    possible[fill]=tmp;
                    fill += 1;
                }
                tmp.x=0;
                tmp.y=1;
                if (is_on_the_board(&pieces[i],&tmp) && not_defended(i, pieces, grid, &tmp, &white)){
                    tmp.value = next(-white,pieces, grid, tmp, depth - 1).value;
                    possible[fill]=tmp;
                    fill += 1;
                }
                tmp.x=-1;
                tmp.y=1;
                if (is_on_the_board(&pieces[i],&tmp) && not_defended(i, pieces, grid, &tmp, &white)){
                    tmp.value = next(-white,pieces, grid, tmp, depth - 1).value;
                    possible[fill]=tmp;
                    fill += 1;
                }
                tmp.x=-1;
                tmp.y=0;
                if (is_on_the_board(&pieces[i],&tmp) && not_defended(i, pieces, grid, &tmp, &white)){
                    tmp.value = next(-white,pieces, grid, tmp, depth - 1).value;
                    possible[fill]=tmp;
                    fill += 1;
                }
                tmp.x=-1;
                tmp.y=-1;
                if (is_on_the_board(&pieces[i],&tmp) && not_defended(i, pieces, grid, &tmp, &white)){
                    tmp.value = next(-white,pieces, grid, tmp, depth - 1).value;
                    possible[fill]=tmp;
                    fill += 1;
                }
                tmp.x=0;
                tmp.y=-1;
                if (is_on_the_board(&pieces[i],&tmp) && not_defended(i, pieces, grid, &tmp, &white)){
                    tmp.value = next(-white,pieces, grid, tmp, depth - 1).value;
                    possible[fill]=tmp;
                    fill += 1;
                }
                tmp.x=1;
                tmp.y=-1;
                if (is_on_the_board(&pieces[i],&tmp) && not_defended(i, pieces, grid, &tmp, &white)){
                    tmp.value = next(-white,pieces, grid, tmp, depth - 1).value;
                    possible[fill]=tmp;
                    fill += 1;
                }
            }
        }
    }
    undo_move(pieces,grid,&move, &undo_piece);
    // select
    if (fill==0){
        return best;
    }
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
    int king_hit = 0;
    srand(3);
    //srand(time(NULL));  // Seed
    //main loop
    for (int i = 1; i <= n; i++) {
        move = next(white,pieces,grid,move,d);
        lettre = 'a' + pieces[move.piece].x+move.x;
        //printf("bare move: %c%d%d\n",pieces[move.piece].txt, move.x, move.y);
        printf("%d.%c%c%d\n",i,pieces[move.piece].txt, lettre, pieces[move.piece].y+move.y+1);
        if (move.x == 0 && move.y == 0){
            printf("Pat\n");
            break;
        }
        apply_move(pieces,grid,&move, &undo_piece, &king_hit);
        if (king_hit){
            undo_move(pieces,grid,&move,&undo_piece);
            king_hit = piece_there(pieces, grid, &move);
            if (king_hit == 15){
                printf("Black win\n");
            }
            else{
                printf("White Win\n");
            }
            break;
        }
        plot_grid(pieces,grid);
        printf("\n");
        white=-white;
        move.x=0;
        move.y=0;
    }
    
    return 0;
}