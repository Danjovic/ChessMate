
// Buttons
#define BT_1A_PORT  PORTC  // Segment E
#define BT_1A_DDR   DDRC 
#define BT_1A_PIN   PINC
#define BT_1A_BIT   4

#define BT_2B_PORT  PORTC  // Segment A
#define BT_2B_DDR   DDRC 
#define BT_2B_PIN   PINC
#define BT_2B_BIT   3

#define BT_3C_PORT  PORTC  // Segment F 
#define BT_3C_DDR   DDRC 
#define BT_3C_PIN   PINC
#define BT_3C_BIT   1

#define BT_4D_PORT  PORTD  // Segment G
#define BT_4D_DDR   DDRD 
#define BT_4D_PIN   PIND
#define BT_4D_BIT   7

#define BT_5E_PORT  PORTC  // Segment D
#define BT_5E_DDR   DDRC 
#define BT_5E_PIN   PINC
#define BT_5E_BIT   5

#define BT_6F_PORT  PORTC  // Segment DP
#define BT_6F_DDR   DDRC 
#define BT_6F_PIN   PINC
#define BT_6F_BIT   2

#define BT_7G_PORT  PORTC  // Segment C
#define BT_7G_DDR   DDRC 
#define BT_7G_PIN   PINC
#define BT_7G_BIT   0

#define BT_8H_PORT  PORTD  // Segment B
#define BT_8H_DDR   DDRD 
#define BT_8H_PIN   PIND
#define BT_8H_BIT   6

#define BT_CLR_PORT  PORTD  // Segment G
#define BT_CLR_DDR   DDRD 
#define BT_CLR_PIN   PIND
#define BT_CLR_BIT   7

#define BT_ENT_PORT  PORTD  // Segment B
#define BT_ENT_DDR   DDRD 
#define BT_ENT_PIN   PIND
#define BT_ENT_BIT   6  


#define BT_1A_PRESS (BT_1A_PIN & (1<<BT_1A_BIT) == 0)
#define BT_2B_PRESS (BT_2B_PIN & (1<<BT_2B_BIT) == 0)
#define BT_3C_PRESS (BT_3C_PIN & (1<<BT_3C_BIT) == 0)
#define BT_4D_PRESS (BT_4D_PIN & (1<<BT_4D_BIT) == 0)
#define BT_5E_PRESS (BT_5E_PIN & (1<<BT_5E_BIT) == 0)
#define BT_6F_PRESS (BT_6F_PIN & (1<<BT_6F_BIT) == 0)
#define BT_7G_PRESS (BT_7G_PIN & (1<<BT_7G_BIT) == 0)
#define BT_8H_PRESS (BT_8H_PIN & (1<<BT_8H_BIT) == 0)


#define BT_CLR_PRESS (BT_CLR_PIN & (1<<BT_CLR_BIT) == 0)
#define BT_ENT_PRESS (BT_ENT_PIN & (1<<BT_ENT_BIT) == 0)



// Display

#define SEG_A_BIT   3
#define SEG_B_BIT   6
#define SEG_C_BIT   0
#define SEG_D_BIT   5
#define SEG_E_BIT   4
#define SEG_F_BIT   1
#define SEG_G_BIT   7
#define SEG_DP_BIT  2

#define K1_PORT PORTB
#define K1_DDR  DDRB
#define K1_BIT  0

#define K2_PORT PORTB
#define K2_DDR  DDRB
#define K2_BIT  1

#define K3_PORT PORTB
#define K3_DDR  DDRB
#define K3_BIT  2

#define K4_PORT PORTB
#define K4_DDR  DDRB
#define K4_BIT  3

#define K1_ON  K1_PORT |=  (1<<K1_BIT)
#define K1_OFF K1_PORT &= ~(1<<K1_BIT)

#define K2_ON  K2_PORT |=  (1<<K2_BIT)
#define K2_OFF K2_PORT &= ~(1<<K2_BIT)

#define K3_ON  K3_PORT |=  (1<<K3_BIT)
#define K3_OFF K3_PORT &= ~(1<<K3_BIT)

#define K4_ON  K4_PORT |=  (1<<K4_BIT)
#define K4_OFF K4_PORT &= ~(1<<K4_BIT)

