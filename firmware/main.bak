 /*
 *   ___ _              __  __      _       
 *  / __| |_  ___ _____|  \/  |__ _| |_ ___ 
 * | (__| ' \/ -_|_-<_-< |\/| / _` |  _/ -_)
 *  \___|_||_\___/__/__/_|  |_\__,_|\__\___|
 *                                         
 *   
 *  Microcontroller based Chess Computer
 *  
 *
 *  Authors: Daniel Jose Viana - danjovic@hotmail.com
 *           Reinaldo Flamino  
 *  
 *  Version 0.1 - 05 December 2015
 */ 

////////////////////////////////////////////////////////////////////////////////
// required headers
#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <string.h>

////////////////////////////////////////////////////////////////////////////////
// Definitions
#include "board_definitions.h"
#include "IO_functions.h"

////////////////////////////////////////////////////////////////////////////////
// Variables

static uint8_t Display[4] = { _SPC, _SPC, _SPC, _SPC };   // blank display
static uint8_t KeyStatus = 0;                             // no key pressed


////////////////////////////////////////////////////////////////////////////////
// Functions
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
//                                    | | | | \   /
// CLR long=1/short=0 ----------------+ | | |  \ /
// CLR event----------------------------+ | |   V
// ENTER event----------------------------+ |   |
// Key[1-8] event---------------------------+   |
// Key Pressed (1..8)---------------------------+

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
				KeyStatus|=(1<<KEY18_EVENT) ;  
				KeyStatus &= 0b11111000; 
				//Display[3]=last_key;
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

			} else last_key=0;
 
		}
		
		
		
	   /////////////////////////////////////////////////////////////////////////
	   // Update Display
	   //DDRD = 0xc0; // bits 7..6 as outuputs again
	   //DDRC = 0x3f; // bits 5..0 as outputs again
	 
	   PORTB=0xff;	
	   PORTC = Display[digit_now] & 0x3f;
	   PORTD |= 0xc0;
	   PORTD &= (Display[digit_now] | 0x3f );
	   
	   switch (digit_now) {
		case 0:  PORTB=0xfe; break;
		case 1:  PORTB=0xfd; break;
		case 2:  PORTB=0xfb; break;
	   	case 3:  PORTB=0xf7; break; 		

	   }
   }

}




////////////////////////////////////////////////////////////////////////////////
//  __  __      _        ___             _   _          
// |  \/  |__ _(_)_ _   | __|  _ _ _  __| |_(_)___ _ _  
// | |\/| / _` | | ' \  | _| || | ' \/ _|  _| / _ \ ' \ 
// |_|  |_\__,_|_|_||_| |_| \_,_|_||_\__|\__|_\___/_||_|
//                                                      


int main()
{
	
	uint8_t i=0;
	
	wdt_disable(); // no watchdog 
    
	Initialize_Hardware ();
	 
	Display[0] = ~_P;
	Display[1] = ~_L;
	Display[2] = ~_A;
	Display[3] = ~_Y;	
	
	// Start the show!
	for (;;) {
	   Refresh_display();
	   //	Display[3] = ~KeyStatus;
		
	
	 
	   if (KeyStatus & (1<<CLR_EVENT)) {
	        
	   
			if (KeyStatus & (1<<CLR_LONG)) {
				 Display[0] = ~_L;
				 Display[1] = ~_O;
				 Display[2] = ~_N;
				 Display[3] = ~_G;				
			}  else {
				 Display[0] = ~_S;
				 Display[1] = ~_H;
				 Display[2] = ~_R;
				 Display[3] = ~_T;			
			} 
	   
	   
	        KeyStatus &= ~(1<<CLR_EVENT) ;
	   }
	   
	   if (KeyStatus & (1<<ENTER_EVENT)) {
	          

				 Display[0] = ~_E;
				 Display[1] = ~_N;
				 Display[2] = ~_T;
				 Display[3] = ~_R;				

	   
	        KeyStatus &= ~(1<<ENTER_EVENT) ;
	   }
	

	   if (KeyStatus & (1<<KEY18_EVENT)){
			Display[0]=~pgm_read_byte(digits+ (KeyStatus & 0x07) +1);
			Display[1]=~pgm_read_byte(digits+ (KeyStatus & 0x07) +1);
			Display[2]=~pgm_read_byte(digits+ (KeyStatus & 0x07) +1);
			Display[3]=~pgm_read_byte(digits+ (KeyStatus & 0x07) +1);			
	        KeyStatus &= ~(1<<KEY18_EVENT) ;
	   }	
	
	
	
	
	
	
	}; // for

	

}
