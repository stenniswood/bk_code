#ifndef _PICAN_DEFINES_
#define _PICAN_DEFINES_

#define OE_pin 7

#define LED_ON  1
#define LED_OFF 0

#define LED_1 0
#define LED_2 1
#define LED_3 2
#define LED_4 11

#define BUTTON_1 4
#define BUTTON_2 5
#define BUTTON_3 6

#define BUTTON_OPEN		1		// Normally Open
#define BUTTON_CLOSED	0

// wiringPi pin numbers
#define TX0RTS 7 
#define TX1RTS 15
#define TX2RTS 16
#define RX0BF  8  
#define RX1BF  9 

#define CAN_INT_PIN 3			// Pi Header pin 15, #22
extern int button_state[3];		// Where defined?! only in main.c now



#endif
