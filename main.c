#include <stdio.h>

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
    Piece undo_piece = {};
    undo_piece.value=-1;
    best.value=-1000;
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
            if (grid[pieces[i].x+(pieces[i].y+1*white)*8]==32 && 0 <= pieces[i].y+1*white && pieces[i].y+1*white < 8){
                //printf("ok\n");
                tmp.x=0;
                tmp.y=1*white;
                tmp.piece=i;
                //printf("ok.%c%d%d\n",pieces[tmp.piece].txt, tmp.x, tmp.y);
                tmp = next(-white,pieces, grid, tmp, depth - 1); //clacul value
                tmp.x=0;
                tmp.y=1*white;
                tmp.piece=i;
                keep(&best, &tmp, &white);
            }
        }
    }
    undo_move(pieces,grid,&move, &undo_piece);
    return best;
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
                printf("%c ", pieces[grid[i]].txt);
            }
            if ((i+1)%8==0){
                printf("\n");
            }
        }
    }
}

int main (int argc, char *argv[]){
    if (argc == 2)
    {
        printf("%s\n", argv[1]);
    }
    else
    {
        printf("Usage :\n");
        printf("%s <number of steps>\n", argv[0]);
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
    printf("eval: %f\n",eval(pieces));
    update_grid(pieces,grid);
    /*
    for (int i = 0; i < 64; i++) {
        printf("grid: %d\n",grid[i]);
    }
    */
    
    Move move = {0,0,0,0};
    Piece undo_piece = {}; //useless here
    char lettre;
    short white=1;
    //main loop
    for (int i = 1; i < 4; i++) {
        move = next(white,pieces,grid,move,2);
        apply_move(pieces,grid,&move, &undo_piece);
        lettre = 'a' + pieces[move.piece].x+move.x;
        //printf("bare move: %c%d%d\n",pieces[move.piece].txt, move.x, move.y);
        printf("%d.%c%c%d\n",i,pieces[move.piece].txt, lettre, pieces[move.piece].y+move.y+1);
        plot_grid(pieces,grid);
        white=-white;
    }
    
    return 0;
}