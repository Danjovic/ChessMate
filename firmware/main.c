//
////////////////////////////////////////////////////////////////////////////////
//                  ___ _              __  __      _                          //
//                 / __| |_  ___ _____|  \/  |__ _| |_ ___                    //
//                | (__| ' \/ -_|_-<_-< |\/| / _` |  _/ -_)                   //
//                 \___|_||_\___/__/__/_|  |_\__,_|\__\___|                   //
//                                                                            //
//                                                                            //
//       __/)         Microcontroller based Chess Computer                    //
//    .-(__(=:                                                                //
// |\ |    \)                                                                 //
// \ ||       Authors: Daniel Jose Viana - danjovic@hotmail.com               //
//  \||                Reinaldo de Sales Flamino - reinaldosflamino@gmail.com //
//ejm\|                                                                       //
// 97 |                   Version 0.1 - 15 Feb 2016		              //
////////////////////////////////////////////////////////////////////////////////
// Titles in ASCII art by Patrick Gillespi http://patorjk.com/software/taag/  //
// Lily ASCII art by ejm                                                      //
////////////////////////////////////////////////////////////////////////////////
#ifndef F_CPU 
#define F_CPU 25000000
#endif


////////////////////////////////////////////////////////////////////////////////
// required headers
#include <avr/io.h>
#include <stdio.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <string.h>
#include "lily.h"
#include "uart.h"

#define USART_BAUDRATE 19200
////////////////////////////////////////////////////////////////////////////////
// Definitions
#include "7seg_definitions.h"




////////////////////////////////////////////////////////////////////////////////
// Constants in ROM
const PROGMEM int difficult_level[8] = {10,100,200,500,700,1000,1500,2000};
const PROGMEM uint8_t ascii_char[16] = {'1','2','3','4','5','6','7','8','a','b','c','d','e','f','g','h'};
const PROGMEM uint8_t     digits[16] = {_1, _2, _3, _4, _5, _6, _7, _8, _A, _B, _C, _D, _E, _F, _G, _H};

///////////////////////////////////////////////////////////////////////////////
// Variables

//static uint8_t Display[4] = { _SPC, _SPC, _SPC, _SPC };   // blank display
static uint8_t KeyStatus = 0;                             // no key pressed

//set stream pointer
FILE usart0_str = FDEV_SETUP_STREAM(USART0SendByte, USART0ReceiveByte, _FDEV_SETUP_RW);

////////////////////////////////////////////////////////////////////////////////
// Functions

////////////////////////////////////////////////////////////////////////////////
//
void Initialize_Hardware (void) {
	DDRB=0x0f;  // bits 4..0 as outputs
	PORTB=0x0f; // All segments off
	
	DDRD = 0xc0; // bits 7..6 as outuputs
	PORTD = 0xfC; // All segments off, pullups on pins 2 and 3 and unused
	
	DDRC = 0x3f; // bits 5..0 as outputs
	PORTC = 0x3f; // All segments off
	
	// Configure timer
	TCCR2B = (1<<CS20) | (1<<CS21) | (1<<CS22);  // Timer 2 prescaler 1024
	// bits 7 turn at a rate of 191Hz (change once at each 5,2ms)

}




////////////////////////////////////////////////////////////////////////////////
// Refresh Display and scan keyboard
// return value in key status: bit  7 6 5 4 3 2 1 0
// NEW event -----------------------+ | | | | \   /
// CLR long=1/short=0 ----------------+ | | |  \ /
// CLR event----------------------------+ | |   V
// ENTER event----------------------------+ |   |
// Key[1-8] event---------------------------+   |
// Key Pressed (1..8)---------------------------+

#define NEW_EVENT   7
#define CLR_LONG    6
#define CLR_EVENT   5
#define ENTER_EVENT 4
#define KEY18_EVENT 3
#define treshold_long_press 228 // ~600ms
#define treshold_short_press 28 //



#define _MOVE_PIN 3
#define _CONTROL_PIN 2

#define MOVE ((PIND & (1<<_MOVE_PIN))==0)

#define CONTROL ((PIND & (1<<_CONTROL_PIN))==0)




void Clear_Display(void) {

	Display[0]= _SPC;     // Clear display
	Display[1]= _SPC;
	Display[2]= _SPC;
	Display[3]= _SPC;
	
	c[0]=32;              // Clear move vector (fill with spaces)
	c[1]=32;
	c[2]=32;
	c[3]=32;
	c[4]=10;

}




////////////////////////////////////////////////////////////////////////////////
//
void Refresh_display (void) {

	uint8_t temp,Keys18;
	
	static uint8_t last_key=0;
	static uint8_t last_digit=0;
	static uint8_t key18_ticks = 0;
	static uint8_t key_clr_ticks  = 0;
	static uint8_t key_ent_ticks  = 0;

	uint8_t digit_now = (TCNT2 >>6 ) & 0x03; // switch each digit ~10,5ms


	// check if it's time to change the segment and scan the keyboard

	if (digit_now != last_digit) { // will occur once at each 2,6 ms
		last_digit = digit_now;
		
		
		/////////////////////////////////////////////////////////////////////////
		// change pins to read keyboard
		
		PORTB=0x0f; // All segments off
		
		PORTD |=0xfc; // Pins B, G in high, pullups on pins
		PORTC |=0x3f; // pins D, F, Dp, A, E, D in high

		
		
		/////////////////////////////////////////////////////////////////////////
		// Process CLR/NEW KEY
		PORTD &= ~(1<<7);   // Drop line and wait for settle down
		asm ("nop");
		asm ("nop");
		asm ("nop");
		
		Keys18=MOVE ;  // save state of move button for this line
		
		
		if (CONTROL) { // count how many cycles (2,6ms each) button CLR/NEW remain pressed
			if ( key_clr_ticks <255 ) {// advance and saturate
				key_clr_ticks++;
			}
			KeyStatus &= ~(1<<CLR_EVENT);  // CLR not pressed;
			} else {                           // button CLR is released
			temp=key_clr_ticks;
			key_clr_ticks=0;
			KeyStatus &= ~(1<<CLR_EVENT | 1<<CLR_LONG); // by default no event if press under treshold
			if (temp > treshold_long_press) KeyStatus|=(1<<CLR_EVENT | 1<<CLR_LONG) ;
			if (temp > treshold_short_press) KeyStatus|=(1<<CLR_EVENT) ;
		}
		PORTD |= (1<<7);   // Raise line pin again
		
		
		
		/////////////////////////////////////////////////////////////////////////
		// Process ENTER Key
		PORTD &= ~(1<<6);   // Drop line and wait for settle down
		asm ("nop");
		asm ("nop");
		asm ("nop");
		
		Keys18=(Keys18<<1)| MOVE ;  // save state of move button for this line
		
		if (CONTROL) { // count how many cycles (2,6ms each) button CLR/NEW remain pressed
			if ( key_ent_ticks <255 ) {// advance and saturate
				key_ent_ticks++;
			}
			KeyStatus &= ~(1<<ENTER_EVENT);  // ENTER not pressed;
			} else {                           // button ENTER is released
			temp=key_ent_ticks;
			key_ent_ticks=0;
			KeyStatus &= ~(1<<ENTER_EVENT); // by default no event if press under treshold
			if (temp > treshold_short_press) KeyStatus|=(1<<ENTER_EVENT) ;
		}
		PORTD |= (1<<6);   // Raise line pin again


		/////////////////////////////////////////////////////////////////////////
		// Process Keys 1-8
		PORTC &= ~(1<<5);   // Drop line and wait for settle down
		asm ("nop");
		asm ("nop");
		asm ("nop");
		Keys18=(Keys18<<1) | MOVE ;  // save state of move button for this line
		PORTC |= (1<<5);   // Raise line pin again
		
		PORTC &= ~(1<<4);   // Drop line and wait for settle down
		asm ("nop");
		asm ("nop");
		asm ("nop");
		Keys18=(Keys18<<1) | MOVE ;  // save state of move button for this line
		PORTC |= (1<<4);   // Raise line pin again
		
		PORTC &= ~(1<<3);   // Drop line and wait for settle down
		asm ("nop");
		asm ("nop");
		asm ("nop");
		Keys18=(Keys18<<1) | MOVE ;  // save state of move button for this line
		PORTC |= (1<<3);   // Raise line pin again
		
		PORTC &= ~(1<<2);   // Drop line and wait for settle down
		asm ("nop");
		asm ("nop");
		asm ("nop");
		Keys18=(Keys18<<1) | MOVE ;  // save state of move button for this line
		PORTC |= (1<<2);   // Raise line pin again
		
		PORTC &= ~(1<<1);   // Drop line and wait for settle down
		asm ("nop");
		asm ("nop");
		asm ("nop");
		Keys18=(Keys18<<1) | MOVE ;  // save state of move button for this line
		PORTC |= (1<<1);   // Raise line pin again
		
		PORTC &= ~(1<<0);   // Drop line and wait for settle down
		asm ("nop");
		asm ("nop");
		asm ("nop");
		Keys18=(Keys18<<1) | MOVE ;  // save state of move button for this line
		PORTC |= (1<<0);   // Raise line pin again
		

		
		if (Keys18) { // count how many any button remain pressed
			if ( key18_ticks <255 ) {// advance and saturate
				key18_ticks++;
			}
			last_key=Keys18;             // save button pressed

			KeyStatus &= ~(1<<KEY18_EVENT);  // No key 1-8 not pressed;
			} else {                           // button ENTER is released
			temp=key18_ticks;
			key18_ticks=0;
			KeyStatus &= ~(1<<KEY18_EVENT); // by default no event if press under treshold
			if (temp > treshold_short_press) {
				KeyStatus|=(1<<KEY18_EVENT) ;    // Set Key Event Flag
				
				
				KeyStatus &= 0b11111000;         // Clear key number
				
				switch (last_key) {
					case 0b00010000: KeyStatus |= 0;   // Key 1
					break;
					case 0b00001000: KeyStatus |= 1;   // Key 2
					break;
					case 0b00000010: KeyStatus |= 2;   // Key 3
					break;
					case 0b10000000: KeyStatus |= 3;   // Key 4
					break;
					case 0b00100000: KeyStatus |= 4;   // Key 5
					break;
					case 0b00000100: KeyStatus |= 5;   // Key 6
					break;
					case 0b00000001: KeyStatus |= 6;   // Key 7
					break;
					case 0b01000000: KeyStatus |= 7;   // Key 8
					break;
					
					default:         KeyStatus &= ~(1<<KEY18_EVENT); //// more than one key pressed simultaneously
					
					
				}
				
				// Process NEW event
				if ((KeyStatus & (1<<KEY18_EVENT)) && (key_clr_ticks > treshold_long_press))
				KeyStatus |= (1<<NEW_EVENT);
				else (KeyStatus &= ~(1<<NEW_EVENT));

			} else last_key=0;

		}
		
		
		
		/////////////////////////////////////////////////////////////////////////
		// Update Display
		//DDRD = 0xc0; // bits 7..6 as outuputs again
		//DDRC = 0x3f; // bits 5..0 as outputs again
		
		PORTB=0xff;
		PORTC = ~Display[digit_now] & 0x3f;
		PORTD |= 0xc0;
		PORTD &= (~Display[digit_now] | 0x3f );
		
		switch (digit_now) {           // Enable current display anode
			case 0:  PORTB=0xfe; break;
			case 1:  PORTB=0xfd; break;
			case 2:  PORTB=0xfb; break;
			case 3:  PORTB=0xf7; break;

		}
	}

}

//
////////////////////////////////////////////////////////////////////////////////




// Print Board at UART
uint8_t printBoard(void) {
	
	printf_P(PSTR("\nChessMate\n\n"));
	
	int NN=-1;
	while(++NN<121) {
		printf("%c",NN&8&&(NN+=7)?10:".?+nkbrq?*?NKBRQ"[b[NN]&15]); /* print board */
	}
	
	printf_P(PSTR("\nCHECKMATE	Legal?	Last	Depth	Score	(Wht=16)\n"));
	printf("%d		%d		%d		%d	%d\n",CHECKMATE,LegalMove,Depth,SCORE,k);
	return 0;
}

void setDisplay() {


			switch (CHECKMATE) {	// Display message according to Check
				case _White_Wins:
				Display[0]= _W;
				Display[1]= _H;
				Display[2]= _I;
				Display[3]= _T;
				break;
				case _Black_Wins:
				Display[0]= _B;
				Display[1]= _L;
				Display[2]= _A;
				Display[3]= _C;
				break;
				case _Stale_Mate:
				Display[0]= _S;
				Display[1]= _T;
				Display[2]= _A;
				Display[3]= _L;
				break;
				case _50th_Move:
				Display[0]= _5;
				Display[1]= _0;
				Display[2]= _T;
				Display[3]= _H;
				break;
				case _Threefold:
				Display[0]= _3;
				Display[1]= _F;
				Display[2]= _L;
				Display[3]= _D;
				break;
				default:
				Display[0]= _E;
				Display[1]= _R;
				Display[2]= _R;
				Display[3]= _O;
			};
}

////////////////////////////////////////////////////////////////////////////////
//  __  __      _        ___             _   _
// |  \/  |__ _(_)_ _   | __|  _ _ _  __| |_(_)___ _ _
// | |\/| / _` | | ' \  | _| || | ' \/ _|  _| / _ \ ' \
// |_|  |_\__,_|_|_||_| |_| \_,_|_||_\__|\__|_\___/_||_|
//


int main( void)
{
	uint8_t cm=0 ; // save check mate state
	uint8_t i=0;
	uint8_t digit_now=0;
	uint8_t First_key=0;
	LilyStatus LilyState = _Startup_Screen;
	
	
	wdt_disable(); // no watchdog
	Initialize_Hardware ();

	///////////////// init USART
	USART0Init();
	stdout = &usart0_str;
	stdin  = &usart0_str;
	printf_P(PSTR("\nLets Play"));
	////////////////////////////////	

	// Start the show!
	
	for (;;) {
		
		switch (LilyState) {
			
			///////////////////////////////////////////////////////////////////////
			case _Startup_Screen:
			Display[0] = _P;   // Now is fixed but can be animated
			Display[1] = _L;   // but now I am simply overwriting
			Display[2] = _A;
			Display[3] = _Y;
			
			i=KeyStatus;                             // Save Key Events
			
			if ( KeyStatus & (1<<NEW_EVENT) ) {      // Check for a NEW event
				KeyStatus=0;                         // If that's the case start a new game
				LilyState = _New_Game;
				RL = pgm_read_word(&difficult_level[(i & 0x07)] );      // Configure recursion (difficulty) level
			}
			break;
			
			
			///////////////////////////////////////////////////////////////////////
			case _New_Game:
			Display[0] = _D;        // Update display. This message can be animated
			Display[1] = _I;        // but now I am simply overwriting
			Display[2] = _F;
			Display[3]=pgm_read_byte(digits+ (i & 0x07) ); // show difficulty level
			
			//if ((KeyStatus & (1<<KEY18_EVENT)) | (KeyStatus & (1<<ENTER_EVENT))) {
				if ((KeyStatus & (1<<KEY18_EVENT)) ) {
				Clear_Display();
				digit_now=0;          // Set position as the first digit
				
				initlily();

				//if (KeyStatus & (1<<ENTER_EVENT)) {
				//	LilyState=_Lily_Move;
					//k=BLACK;//today
					//	printf_P(PSTR("\nLily first"));
				//	} else {
				//	LilyState=_Human_Move;
					//k=WHITE;//today
					//	printf_P(PSTR("\nHuman first"));
				//}
				
				LilyState = _Human_Move;
				KeyStatus=0;
			}
			break;
			
			///////////////////////////////////////////////////////////////////////
			case _Human_Move:
			// Check if NEW was pressed
			i=KeyStatus;                             // Save Key Events
			if ( KeyStatus & (1<<NEW_EVENT) ) {      // Check for a NEW event
				LilyState = _New_Game;               // If that's the case start a new game
				RL = pgm_read_word(&difficult_level[(i & 0x07)] );      // set difficulty level
				KeyStatus=0;
				break;
			}

			// Check if need to clear the display, e.g. after lily has played
			if (i!=0 && First_key) {
				//printf_P(PSTR("\nYour Move "));
				First_key=0;
				Clear_Display();
				digit_now=0;
			}



			// Check if ENTER was pressed
			if (KeyStatus & (1<<ENTER_EVENT)) {
				
				
				//if ( (c[0]==32) && (RL<2000)) {   // if clear screen with difficulty level 1..4
				if ( (c[0]==97&&c[1]==32 && (RL<difficult_level[4]))) {  //if "A" key + enter pressed 
					c[0]=10;                     // then tell Lily to play for you
				    //printf_P(PSTR("\nLily will play for you "));					
				}
				

				Display[0] = _DP;  
				PORTB=0xff;
				PORTC = ~Display[0] & 0x3f;
				PORTD |= 0xc0;
				PORTD &= (~Display[0] | 0x3f );
				PORTB=0xfe;

				cm=playlily();        // Make Human Move
				
				printBoard();		// Print Board

					// Test for Check mate
					if (cm!=0) {
						//setDisplay();
						//printf_P(PSTR("\nGame Over "));
						LilyState = _Check_Mate;	// Yes, show result
						KeyStatus=0;
						
						break;
					}

				if (!LegalMove) {				// Human move was legal?
					//printf_P(PSTR("\nHuman bad move. "));
					Clear_Display();			// No, clear screen,
					//Display[0]=_DP;
					Display[0] = _E;  
					Display[1] = _R;   
					Display[2] = _R;
					Display[3] = _O;					
					
					
					digit_now=0;         		// set position as the first digit
					//LilyState = _Human_Move;	// and return control to human
					KeyStatus=0;
					First_key=1;
					break;
					}	else {					//Human move was legal
					
						//printf_P(PSTR("\ngood move."));
						
						if (k==WHITE) {
							LilyState = _Human_Move;
							First_key=1;                // for clear display
							KeyStatus=0;
							} else {
							LilyState = _Lily_Move;           // Send your move to Lily
							//c[0]=10;
							KeyStatus=0;
						}

						break;
						

					
				}			
				
			}			

			
			// Check if CLEAR was Long pressed
			if (KeyStatus & (1<<CLR_EVENT) && (KeyStatus & (1<<CLR_LONG)) ) {
				
				Clear_Display();
				digit_now=0;          // Set position as the first digit
				
				KeyStatus=0;
				break;
			}
			
			
			// Check if CLEAR was Short pressed
			if (KeyStatus & (1<<CLR_EVENT) && !(KeyStatus & (1<<CLR_LONG)) ) {
				// clear display and decrement pointer according with position
				
				switch(digit_now & 0x03) {
					case 2:
					case 1: digit_now--;
					case 0:	c[digit_now]=32;
					Display[digit_now]=_SPC;
					break;
					
					case 3:
					if (c[digit_now]==32) {  // if haven't keyed yet on the last position
						digit_now--;          // return to previous position
						Display[digit_now]=_SPC;
						} else {
						c[digit_now]=32;          // otherwise clear current position only
						Display[digit_now]=_SPC;
						break;
					}
				}
				KeyStatus=0;
				break;
			}

			
			// Check if a KEY 1..8 was  pressed
			if (KeyStatus & (1<<KEY18_EVENT) ) {
				i=KeyStatus & 0x07;
				// Fill in c[] and Display[]
				switch(digit_now & 0x03) {
					case 0:
					case 2: c[digit_now]=pgm_read_byte(ascii_char+i+8);
					Display[digit_now]=pgm_read_byte(digits+i+8);
					break;
					case 1:
					case 3: c[digit_now]=pgm_read_byte(ascii_char+i);
					Display[digit_now]=pgm_read_byte(digits+i);
					break;
				}
				if (digit_now<3) digit_now++; // limit to the last digit
				
				KeyStatus=0;
				break;
			}
			
			
			break;
			
			
			///////////////////////////////////////////////////////////////////////
			case _Lily_Move:

			//printf_P(PSTR("\nNow Lily will play."));
			
			c[0]=10;
			
			Display[0] = _DP;  
			PORTB=0xff;
			PORTC = ~Display[0] & 0x3f;
			PORTD |= 0xc0;
			PORTD &= (~Display[0] | 0x3f );
			PORTB=0xfe; 
									
			
			cm=playlily();        // Make Lily Move
			
			//printf_P(PSTR(" Done.\n"));

			printBoard();		// Print Board

			// Check for Check mate
			if (cm!=0) {
				//setDisplay();
				//printf_P(PSTR("\nGame Over "));
				LilyState = _Check_Mate;	// Yes, show result
				KeyStatus=0;
				
				break;
			}
			
				
			// Check for Terminated game (bit 7 is set). In this case playlily returns new difficulty level
			if (cm & 0x80 ) {
				i=cm & 0x07;
				RL = pgm_read_word(&difficult_level[(cm & 0x07)] );  //  Configure recursion (difficulty) level
				LilyState = _New_Game;
				KeyStatus=0;
				break;
				
			}
			
                  
				
				// Must convert from ASCII to code;
				Display[0] = pgm_read_byte( &digits [ ((c[0]-1) & 0x07) + 8 ]); // first is a letter
				Display[1] = pgm_read_byte( &digits [ ((c[1]-1) & 0x07)     ]); // second is a number
				Display[2] = pgm_read_byte( &digits [ ((c[2]-1) & 0x07) + 8 ]); // third is a letter
				Display[3] = pgm_read_byte( &digits [ ((c[3]-1) & 0x07)     ]); // fourth is a number
				Display[3] |= _DP;
				digit_now=0;         		// set position as the first digit
				
				if (k==WHITE) {
					LilyState = _Human_Move;
					First_key=1;                // for clear display
					KeyStatus=0;
				} else {
						LilyState = _Lily_Move;           // Send your move to Lily
						//c[0]=10;
						KeyStatus=0;
				}				
			
			break;
			
			
			///////////////////////////////////////////////////////////////////////
			case _Check_Mate:
			
			setDisplay();
		
			digit_now=0;
			
			// Check if NEW was pressed to restart the game
			i=KeyStatus;                             // Save Key Events
			if ( KeyStatus & (1<<NEW_EVENT) ) {      // Check for a NEW event
				LilyState = _New_Game;               // If that's the case start a new game
				RL = pgm_read_word(&difficult_level[(i & 0x07)] );      // set difficulty level
				KeyStatus=0;
				break;
			}

			break;
			
			///////////////////////////////////////////////////////////////////////
			default:
			
			
			break;
			
			
			
		} // switch
		Refresh_display();
	} ; // for(;;)
}
