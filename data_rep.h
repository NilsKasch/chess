#ifndef DATA_REP_H
#define DATA_REP_H

#define WHITE_CASLTE_LEFT  (1 << 0)    // 0001
#define WHITE_CASLTE_RIGHT  (1 << 1)   // 0010
#define BLACK_CASLTE_LEFT  (1 << 2)    // 0100
#define BLACK_CASLTE_RIGHT  (1 << 3)   // 1000

typedef struct
{
   //Piece pieces[32];
   unsigned int en_passant; //=numbre of the piece when en passant is possible
   unsigned int castle_rights; //store castle rights
}Board;

typedef struct {
   char txt;
   float value;
   unsigned int x,y;
}Piece;

typedef struct {
   int piece;
   int x,y;
   char transform;
}Move;

#endif