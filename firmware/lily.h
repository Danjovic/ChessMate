/*
 * lily.h
 *
 * Created: 08/12/2015 15:16:28
 *  Author: RFLAMINO
 */ 

#include <avr/pgmspace.h>
#include "7seg_definitions.h"

#ifndef LILY_H_
#define LILY_H_
/*typedef*/ enum  {
	_No_CheckMate=0,
	_White_Wins,
	_Black_Wins,
	_Stale_Mate,
	_50th_Move,
	_Threefold,
} Check_Mate ;

typedef enum  {
	_Startup_Screen=0,
	_New_Game,
	_Human_Move,
	_Lily_Move,
	_Check_Mate
} LilyStatus ;

	#define BLACK 8
	#define WHITE 16
	
    static uint8_t Display[4] = { _SPC, _SPC, _SPC, _SPC }; 


	extern int16_t M,S,I,Q,O,K,N,R,J,Z,k,*p;
	
	uint8_t c[5];

	extern int8_t w_[8],o_[32];

	extern uint8_t L,
	b[129];                                 /* board: half of 16x8+dummy */

	extern   int RL;					    /* recursion limit (level) */
	extern   int SCORE;						/* SCORE -> -7999 (mate W or B) */
	extern   int Depth;						/* Depth of thinking */
	
	extern   unsigned int movenumber;
	extern   unsigned int movedpieces;
		
	extern   char WhiteMove;				/* White turn?*/
	extern   char LegalMove;				/* Last move was legal? */
	
	extern   char CHECKMATE;				/* 1-> White CHECKMATE 2-> Black CHECKMATE 3->STALEMATE 4-> Fifty rule  5->Threefold repetition */
	extern	 char NEWGAME;					/* Start a newgame */	
	extern	 int FIFTY;						/* fifty move rule */
	
extern int16_t D(int16_t q, int16_t l, int16_t e, int16_t E,int16_t z, int16_t n);                          /* recursive minimax search */
extern void initlily (void);
extern uint8_t playlily (void);
extern uint8_t checkTR (int mp);
extern void CopyBoard (int s);

#endif /* LILY_H_ */
