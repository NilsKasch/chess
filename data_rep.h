#ifndef DATA_REP_H
#define DATA_REP_H

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

#endif