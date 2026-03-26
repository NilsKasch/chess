#include <stdio.h>

typedef struct {
    char txt;
    float value;
    int x,y;
}Piece;

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
        {'p',1,1,2},
        {'p',1,2,2},
        {'p',1,3,2},
        {'p',1,4,2},
        {'p',1,5,2},
        {'p',1,6,2},
        {'p',1,7,2},
        {'p',1,8,2},
        {'R',5,1,1},
        {'R',5,8,1},
        {'N',3,2,1},
        {'N',3,7,1},
        {'B',3,3,1},
        {'B',3,6,1},
        {'Q',9,4,1},
        {'K',1000,5,1},
        {'p',1,1,7},
        {'p',1,2,7},
        {'p',1,3,7},
        {'p',1,4,7},
        {'p',1,5,7},
        {'p',1,6,7},
        {'p',1,7,7},
        {'p',1,8,7},
        {'R',5,1,8},
        {'R',5,8,8},
        {'N',3,2,8},
        {'N',3,7,8},
        {'B',3,3,8},
        {'B',3,6,8},
        {'Q',9,4,8},
        {'K',1000,5,8}
    };

    printf("start\n");
    printf("eval: %f\n",eval(pieces));

    return 0;
}