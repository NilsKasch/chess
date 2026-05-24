#ifndef DATA_REP_H
#define DATA_REP_H

typedef struct
{
   //Piece pieces[32];
   int en_passant; //=numbre of the piece when en passant is possible
   int castle_right; //=1 when castling is possible
   int castle_left; //=1 when castling is possible
}Board;


typedef struct {
   char txt;
   float value;
   int x,y;
}Piece;

typedef struct {
   int piece;
   int x,y;
   char transform;
}Move;

#endif