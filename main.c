#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct {
    char txt;
    float value;
    int x,y;
}Piece;

typedef struct {
    int piece;
    int x,y;
    float value;
}Move;

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
    if (move->x != 0 || move->y != 0) //safeguard
    //printf("bare move: %c%d%d\n",pieces[move->piece].txt, move->x, move->y);
    {
        int target=grid[(pieces[move->piece].x+(move->x))+(pieces[move->piece].y+(move->y))*8];
        if (target!=32){
            undo_piece->txt = pieces[target].txt;
            undo_piece->value = pieces[target].value;
            undo_piece->x = target;
            undo_piece->y = target;
            pieces[target].value=-1;
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

    undo_piece.value=-1;
    best.value=-1000*white;
    apply_move(pieces,grid,&move, &undo_piece);
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
        if (pieces[i].txt == 'p'){
            // move foreward
            tmp.x=0;
            tmp.y=1*white;
            tmp.piece=i;
            if (is_on_the_board(&pieces[i],&tmp) && piece_there(&pieces[i], grid, &tmp)==32)
            {
                tmp.value = next(-white,pieces, grid, tmp, depth - 1).value;
                possible[fill]=tmp;
                fill += 1;
            }
            // eat on the left
            tmp.x=-1;
            tmp.y=1*white;
            tmp.piece=i;
            if (is_on_the_board(&pieces[i],&tmp) && piece_there(&pieces[i], grid, &tmp)!=32)
            {
                tmp.value = next(-white,pieces, grid, tmp, depth - 1).value;
                possible[fill]=tmp;
                fill += 1;
            }

        }
    }
    undo_move(pieces,grid,&move, &undo_piece);
    // select
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
    //printf("%d\n",equal);
    return best;
}

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
    short white=1;
    srand(time(NULL));  // Seed
    //main loop
    for (int i = 1; i <= n; i++) {
        move = next(white,pieces,grid,move,d);
        lettre = 'a' + pieces[move.piece].x+move.x;
        //printf("bare move: %c%d%d\n",pieces[move.piece].txt, move.x, move.y);
        printf("%d.%c%c%d\n",i,pieces[move.piece].txt, lettre, pieces[move.piece].y+move.y+1);
        apply_move(pieces,grid,&move, &undo_piece);
        plot_grid(pieces,grid);
        printf("\n");
        white=-white;
        move.x=0;
        move.y=0;
    }
    
    return 0;
}