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

static uint8_t Display[4] = { 0x00, 0x00, 0x00, 0x00 };   // blank display


////////////////////////////////////////////////////////////////////////////////
// Functions
void Initialize_Hardware (void) {
	DDRB=0x0f;  // bits 4..0 as outputs
	PORTB=0x0f; // All segments off
	
	DDRD = 0xc0; // bits 7..6 as outuputs
	PORTD = 0xC0; // All segments off
	
	DDRC = 0x3f; // bits 5..0 as outputs
	PORTC = 0x3f; // All segments off
	
	// Configure timer 	
	TCCR1B = _BV(CS12) | _BV(CS11);  

}




void Refresh_display (void) {
   static uint8_t a=0;
   //uint8_t b=0;
   
   a=(a+1) & 0x03;
   
   
   PORTC = Display[a] & 0x3f;
   PORTD |= 0xc0;
   PORTD &= (Display[a] | 0x3f );
   
   
   
   switch (a) {
	case 0:  PORTB=0xfe; break;
	case 1:  PORTB=0xfd; break;
	case 2:  PORTB=0xfb; break;
	case 3:  PORTB=0xf7; break;
   
   
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
	wdt_disable(); // no watchdog 
    
	Initialize_Hardware ();
	 
	Display[0] = ~_P;
	Display[1] = ~_L;
	Display[2] = ~_A;
	Display[3] = ~_Y;	
	
	// Start the show!
	for (;;) {
	   Refresh_display();
	   _delay_ms(5);
	
		
	
	
	
	
	
	
	
	
	};

	

}
