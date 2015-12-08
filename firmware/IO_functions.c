



#define QUESTION_MARK 0x00	


void refresh_display( uint8_t* ascii_digits) {

	uint8_t i = 0x3f; // Question Mark
	uint8_t digit_now = (TCNT2 >> 5) & 0x03;
	
	uint8_t c = *(ascii_digits + digit_now); // get current ascii digit
	
	// convert digit in segments
	
	if ( (c >='0') && (c <='9') )  // digit in 0..9
		i= c-'0';                  // convert to 0..9
	else {	
			c |= (1<<6) ;                 // not digit, then make uppercase 
			if ( (c >='A') && (c <='F') ) // digit in A..F
		i= 10 + c-'A';            // convert to  10..15  
	} else {	
	if (i==	0x3f)                 // if i not changed value, then c is out of 0..9/aA..fF 
		c = QUESTION_MARK;        // then show a question mark
	else 
		c = pgm_read_byte digits[(i & 0x0f)];   // assure index within 0..15
	}	
	// now update number
	PORTC = c & 0x3f;     // update bits 0..5 in port C
	PORTD &= 0x3f;        // clear 2 MSBs from port D
	PORTC |= (c & 0xc0);  // update bits 6..7 in port D	
	
		// turn on/off digit according with state of digit
	switch (digit_now) {
	case 0:	
			K4_OFF; // erase previous digit
			K1_ON; // activate 
		break;
	case 1:
			K1_OFF; // erase previous digit
			K2_ON; // activate 
		break;
	case 2:
			K2_OFF; // erase previous digit
			K3_ON; // activate 
		break;
	case 3:
			K3_OFF; // erase previous digit
			K4_ON; // activate
		break;
	}
}


// Debounce keyboard and return current keystroke
//
uint8_t read_keyboard (void) {


}








uint8_t debounce_B2 (void) {

	static uint8_t last_timer_toggle=0;
	static uint8_t time_button_pressed=0;
	uint8_t temp;

	if (B2_Pressed) { // count how many cycles button remained press
		temp=TCNT2 & (1<<5); // 
		if ( (temp!=last_timer_toggle) & (time_button_pressed <255) ) {// advance and saturate VX7333b6450X
			time_button_pressed++; 
			last_timer_toggle=temp;
		}  
		return _no_press;     
	} else {  // button is released
		temp=time_button_pressed;
		time_button_pressed=0;
		if (temp < treshold_short_press) return _no_press;     // return no press when time below noise treshold 
		if (temp < treshold_long_press)  return _short_press;  // return short press when time above noise treshold but below long press treshold
		else return _long_press ;                           // return long press wuen time is above long press treshold 
	}
	return _no_press; // redundant  
}



















