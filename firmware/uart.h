//
// Function prototypes
//

#ifndef __UART_H__
#define __UART_H__
#include <stdint.h>
#include <stdio.h>

#ifndef F_CPU
#define F_CPU 25000000
#endif


#ifndef USART_BAUDRATE 
#define USART_BAUDRATE 19200
#endif

#define TXPIN 1

#define UBRR_VALUE (((F_CPU / (USART_BAUDRATE * 16UL))) - 1)

#define true 1
#define false 0


uint8_t kbhit(void);
void USART0Init(void);
int  USART0SendByte(char u8Data, FILE *stream);
int  USART0ReceiveByte(FILE *stream);


#endif 
