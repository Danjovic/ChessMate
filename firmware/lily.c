/*
 * lily.c
 *
  * Created: 27/10/2015 18:13:57
  *  Authors:  - Reinaldo de Sales Flamino - reinaldosflamino@gmail.com
  *            - Daniel Jose Viana - danjovic@hotmail.com
  *
  *  - Threefold repetition rule using Compact Chess Representation of history boards (valid for last 15 boards); 
  *     based on paper: "AN ALTERNATIVE EFFICIENT CHESSBOARD REPRESENTATION BASED ON 4-BIT PIECE CODING".
  *  - Included Fifty rule check (from fairy-Max).
  *	 - Chess Engine based on recursive negamax search micro-Max 4.8 by H.G. Muller (hash table removed by Andre Adrian).
  */

#ifndef LILY_C
#define LILY_C


#include <stdint.h>			/* uint32_t */
#include <avr/io.h>			/* TCNT2 timer */
#include <stdlib.h>			/* srand, rand */
#include  "lily.h"
#include "7seg_definitions.h"


	/* Compact Chess Representation of history boards */
	#define HISTORY 15
	uint32_t HistoryBoards[HISTORY][8];


	int16_t M=136,S=128,I=8e3,Q,O,K,N,R,J,k=16,*p,Z=0;			/* M=0x88      Z=recursion counter          */  /* R-> captured non pawn material  */
	uint8_t c[5];
	int8_t
	w_[]={0,2,2,7,-1,8,12,23},								/* relative piece values    */
		
	o_[]={-16,-15,-17,0,1,16,0,1,16,15,17,0,14,18,31,33,0,	/* step-vector lists */
		7,-1,11,6,8,3,6,									/* 1st dir. in o_[] per piece*/
		6,3,5,7,4,5,3,6};									/* initial piece setup      */
		
        uint8_t L=0;
		uint8_t b[129];												/* board: half of 16x8+dummy*/

		int RL=1200;											/* recursion limit (LEVEL) */
		int SCORE;											/* SCORE -> -7999 (mate W or B) */
		
		int Depth;											/* Counter of searched nodes */
//		unsigned int movenumber;
		unsigned int movedpieces;
		char WhiteMove;										/* White turn? */
		char LegalMove;										/* Last move was legal? */
		char CHECKMATE;										/* 1-> White CHECKMATE 2-> Black CHECKMATE 3->STALEMATE 4-> Fifty rule  5->Threefold repetition */
//		char NEWGAME;										/* Start a newgame */
		
		int FIFTY;											/* fifty move rule */
//		unsigned int Thinking;								/* The change of this variable indicates thinking */



unsigned short ramSeed(void)
{
	unsigned short seed = 0;
	unsigned short *pp = (unsigned short*) (RAMEND+1);
	extern unsigned short __heap_start;
	
	while (pp >= &__heap_start + 1)
	seed ^= * (--pp);
	
	return seed;
}


//////////////////////////////////////////////	
//     ___ _                   
//    / __| |_  ___ ______     
//   | (__| ' \/ -_|_-<_-<     
//    \___|_||_\___/__/__/     
//   | __|_ _  __ _(_)_ _  ___ 
//   | _|| ' \/ _` | | ' \/ -_)
//   |___|_||_\__, |_|_||_\___|
//            |___/            
/***************************************************************************/
/*                               micro-Max,                                */
/* A chess program smaller than 2KB (of non-blank source), by H.G. Muller  */
/***************************************************************************/
/* version 4.8 (1953 characters) features:                                 */
		
/***************************************************************************/
/*                               micro-Max,                                */
/* A chess program smaller than 2KB (of non-blank source), by H.G. Muller  */
/* Port to Atmel ATMega644 and AVR GCC, by Andre Adrian                    */
/***************************************************************************/		
		
	int16_t D(int16_t q, int16_t l, int16_t e, int16_t E,int16_t z, int16_t n)          /* recursive minimax search */
	/* (q,l)=window, e=current eval. score, */
	/* E=e.p. sqr.z=prev.dest, n=depth; return score */
	{
		int16_t m=0,v=0,i=0,P=0,V=0,s=0;
		uint8_t t=0,p=0,u=0,x=0,y=0,X=0,Y=0,H=0,B=0,j=0,d_=0,h=0,F=0,G=0,C=0;
		int8_t r=0;
		
		 if (++Z>30) {                                     /* stack underrun check --  */
			 //printf_P(PSTR("\n Stack underrun"));
			 --Z;return e;
		 }	
		
		q--;                                          /* adj. window: delay bonus */
		k^=24;                                        /* change sides             */
		d_=Y=0;                                        /* start iter. from scratch */
		X=rand()&~M;                                /* start at random field    */
		while(d_++<n||d_<3||                                /* iterative deepening loop */
		z&K==I&&(N<RL&d_<98||                         /* root: deepen upto time   */
		(K=X,L=Y&~M,d_=3)))                          /* time's up: go do best    */
		{x=B=X;                                       /* start scan at prev. best */
			h=Y&S;                                       /* request try noncastl. 1st*/
			P=d_<3?I:D(-l,1-l,-e,S,0,d_-3);                /* Search null move         */
			m=-P<l|R>35?d_>2?-I:e:-P;                     /* Prune or stand-pat       */
			++N;                                         /* node count (for timing)  */
			
			do{u=b[x];                                   /* scan board looking for   */
				if(u&k)                                     /*  own piece (inefficient!)*/
				{r=p=u&7;                                   /* p = piece type (set r>0) */
					j=o_[p+16];                                 /* first step vector f.piece*/
					while(r=p>2&r<0?-r:-o_[++j])                    /* loop over directions o_[] */
					{A:                                        /* resume normal after best */
						y=x;F=G=S;                                /* (x,y)=move, (F,G)=castl.R*/
						do{                                       /* y traverses ray, or:     */
							H=y=h?Y^h:y+r;                           /* sneak in prev. best move */
							if(y&M)break;                            /* board edge hit           */
							m=E-S&b[E]&&y-E<2&E-y<2?I:m;             /* bad castling             */
							if(p<3&y==E)H^=16;                       /* shift capt.sqr. H if e.p.*/
							t=b[H];if(t&k|p<3&!(y-x&7)-!t)break;     /* capt. own, bad pawn mode */
							i=37*w_[t&7]+(t&192);                     /* value of capt. piece t   */
							m=i<0?I:m;                               /* K capture                */
							if(m>=l&d_>1)goto C;                      /* abort on fail high       */
							v=d_-1?e:i-p;                             /* MVV/LVA scoring          */
							if(d_-!t>1)                               /* remaining depth          */
							{v=p<6?b[x+8]-b[y+8]:0;                  /* center positional pts.   */
								b[G]=b[H]=b[x]=0;b[y]=u|32;             /* do move, set non-virgin  */
								if(!(G&M))b[F]=k+6,v+=50;               /* castling: put R & score  */
								v-=p-4|R>29?0:20;                       /* penalize mid-game K move */
								if(p<3)                                 /* pawns:                   */
								{v-=9*((x-2&M||b[x-2]-u)+               /* structure, undefended    */
									(x+2&M||b[x+2]-u)-1              /*        squares plus bias */
									+(b[x^16]==k+36))                 /* kling to non-virgin King */
									-(R>>2);                          /* end-game Pawn-push bonus */
									V=y+r+1&S?647-p:2*(u&y+16&32);         /* promotion or 6/7th bonus */
									b[y]+=V;i+=V;                          /* change piece, add score  */
								}
								v+=e+i;V=m>q?m:q;                       /* new eval and alpha       */
								C=d_-1-(d_>5&p>2&!t&!h);
								C=R>29|d_<3|P-I?C:d_;                     /* extend 1 ply if in check */
								do
								s=C>2|v>V?-D(-l,-V,-v,                 /* recursive eval. of reply */
								F,0,C):v;        /* or fail low if futile    */
								while(s>q&++C<d_);v=s;
								if(z&&K-I&&v+I&&x==K&y==L)              /* move pending & in root:  */
								{Q=-e-i;O=F;                            /*   exit if legal & found  */
									R+=i>>7;
									FIFTY = t|p<3?0:FIFTY+1;   /* fifty move rule */
									--Z;return l;                  /* captured non-P material  */
								}
								b[G]=k+6;b[F]=b[y]=0;b[x]=u;b[H]=t;     /* undo move,G can be dummy */
							}
							if(v>m)                                  /* new best, update max,best*/
							m=v,X=x,Y=y|S&F;                        /* mark double move with S  */
							if(h){h=0;goto A;}                       /* redo after doing old best*/
							if(x+r-y|u&32|                           /* not 1st step,moved before*/
							p>2&(p-4|j-7||                        /* no P & no lateral K move,*/
							b[G=x+3^r>>1&7]-k-6                   /* no virgin R in corner G, */
							||b[G^1]|b[G^2])                      /* no 2 empty sq. next to R */
							)t+=p<5;                               /* fake capt. for nonsliding*/
							else F=y;                                /* enable e.p.              */
							}while(!t);                                   /* if not capt. continue ray*/
							}}}while((x=x+9&~M)-B);                          /* next sqr. of board, wrap */
							C:if(m>I-M|m<M-I)d_=98;                         /* mate holds to any depth  */
							m=m+I|P==I?m:0;                              /* best loses K: (stale)mate*/
							if(z)
							{
								*c='a'+(X&7);c[1]='8'-(X>>4);c[2]='a'+(Y&7);c[3]='8'-(Y>>4&7);c[4]=0;
							}
							}                                             /*    encoded in X S,8 bits */
							if (N%50==0) PORTB^=9;// blink thinking leds
							k^=24;                                        /* change sides back        */
							--Z;return m+=m<e;                            /* delayed-loss bonus       */

						}

						
//////////////////////////////////////////////						
//    ___ _           
//   | _ \ |__ _ _  _ 
//   |  _/ / _` | || |
//   |_| |_\__,_|\_, |
//               |__/ 
uint8_t playlily (void) {
	
	unsigned int seed;
		
	seed=(unsigned int)(ramSeed()+TCNT2);		  /* Use SRAM and Timer/Counter Register TCNT2 as seed/entropy for rand */
	srand(seed);
	
	
	//if (CHECKMATE==0) {
		
		CopyBoard(movedpieces%HISTORY);
		 
		K=I;                                            /* invalid move       */
		if(*c-10)K=*c-16*c[1]+799,L=c[2]-16*c[3]+799;	/* parse entered move */

		N=0;											/* zeros recursion variable */
		SCORE=D(-I,I,Q,O,1,3);							/* think or check & do*/
		Depth=N;
		
		if (SCORE==I) {LegalMove=1;} else {LegalMove=0;}
		
		if (LegalMove) {
			
			movedpieces++;
//			if (movedpieces % 2 ==0) {
//				movenumber++;;
//			}
			
		}
		
		if (k==WHITE) {WhiteMove=1;} else {WhiteMove=0;}
		
		if (SCORE==-I+1)											/* CHECKMATE! */
		{
			if (WhiteMove) {
				CHECKMATE=_Black_Wins;										/* BLACK WINS! */
				} else {
				CHECKMATE=_White_Wins;										/* WHITE WINS! */
			}
		}
		if (SCORE>-I+1 && K==0 && L==0) {							/* STALEMATE!*/
			CHECKMATE=_Stale_Mate;											/* DRAW!*/
		}
		if(FIFTY >=100) {											/* Fifty move rule! */
			CHECKMATE=_50th_Move;											/* DRAW! */
		}

		if ((checkTR(movedpieces % HISTORY)==1)&&(movedpieces>2)) { /* Threefold repetition */
			CHECKMATE=_Threefold;											/* DRAW!*/
		}		
		
	//}
	return CHECKMATE;
}


//////////////////////////////////////////////
//    ___      _ _   
//   |_ _|_ _ (_) |_ 
//    | || ' \| |  _|
//   |___|_||_|_|\__|
//                   
	void initlily (void) {


		CHECKMATE=0;
		Depth=0;
		WhiteMove=1;
		LegalMove=0;
		movedpieces=0;
		SCORE=10;

		k=WHITE;																/* initial Side white*/
		Q=0;
		O=S;
		R=0;

		for(int i_=0;i_<129;i_++) {
			b[i_]=0;															/* clear board   */
			if (i_<HISTORY) for(int k_=0;k_<8;k_++)	HistoryBoards[i_][k_]=0;	/* clear history   */
		}


		K=8;while(K--)
		{b[K]=(b[K+112]=o_[K+24]+8)+8;b[K+16]=18;b[K+96]=9;						/* initial board setup*/
			L=8;while(L--)b[16*L+K+8]=(K-4)*(K-4)+(L-3.5)*(L-3.5);				/* center-pts table   */
			}																	/*(in unused half b[])*/
			


		}


	/* Check Threefold repetition */
	uint8_t checkTR (int mp) {
		int j, k_, cnt=0;

		if (LegalMove&&(movedpieces>4)) {
			/* search states for third repeat */
			for(j=4; j<=HISTORY; j+=4)
			{
				for(k_=0; k_<8; k_++) {
					if(HistoryBoards[(mp-1)][k_] != HistoryBoards[(mp-1-j)][k_] )
					{
						goto differs;
					}
				}
				/* is the same, count it */
				if(++cnt == 2) /* third repeat */
				{
					/* Draw by repetition") */

					return 1;
				}
				differs: ;
			}
		}

		return 0;
	}

	
//////////////////////////////////////////////	
//     ___                    
//    / __|___ _ __ _  _      
//   | (__/ _ \ '_ \ || |     
//    \___\___/ .__/\_, |   _ 
//   | _ ) ___|_|_ _|__/ __| |
//   | _ \/ _ \/ _` | '_/ _` |
//   |___/\___/\__,_|_| \__,_|
//                            	
	void CopyBoard (int s) {

		uint32_t tempboard[8][8],tb;
		unsigned char wb,bb;
		
		/* C.C.R. (Compact Chessboard Representation) 4 bit piece coding */
		
		for(int i_=0;i_<8;i_++){
			for(int k_=0;k_<8;k_++){
				
					wb=b[16*i_+k_]&15;
					bb=b[16*i_+k_]&31;

					tb=0;		//empty space (default)

					switch(wb) {
						case(9):
						tb=1;	//white pawn
						break;
						case(11):
						tb=2;	//white knight
						break;
						case(13):
						tb=3;	//white bishop
						break;
						case(14):
						tb=4;	//white rook
						break;
						case(15):
						tb=5;	//white queen
						break;
						case(12):
						tb=6;	//white king
						break;
						default:;
						
					}

					switch(bb) {
						case(18):
						tb=9;	//black pawn
						break;
						case(19):
						tb=10;	//black knight
						break;
						case(21):
						tb=11;	//black bishop
						break;
						case(22):
						tb=12;	//black rook
						break;
						case(23):
						tb=13;	//black queen
						break;
						case(20):
						tb=14;	//black king
						break;
						default:;						
					}
					tempboard[i_][k_]=tb;

			}

		}

		/* Encode the board with the C.C.R. */
		for(int i_=0;i_<8;i_++){
			HistoryBoards[s][i_]=tempboard[i_][7]|tempboard[i_][6]<<4|tempboard[i_][5]<<8|	\
			tempboard[i_][4]<<12|tempboard[i_][3]<<16|tempboard[i_][2]<<20|					\
			tempboard[i_][1]<<24|tempboard[i_][0]<<28;
		}

		//return 0;
	}




#endif /* LILY_C_ */
