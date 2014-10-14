#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <wiringPi.h>

/***************************************************************
*  This file Tests the LEDs & buttons on the PiCAN board
*  3 LEDS:
*
*  3 BUTTONS:
*
***************************************************************/

#define OE_pin 7
int fd;

#define LED_1 0		// Pi Header Pin 11
#define LED_2 1		// Pi Header Pin 12
#define LED_3 2		// Pi Header Pin 13
#define LED_4 11		// Pi Header Pin 14

#define LED_ON  1
#define LED_OFF 0

#define BUTTON_1  4	// Pi Header Pin 16
#define BUTTON_2  5	// Pi Header Pin 18
#define BUTTON_3  6	// Pi Header Pin 22
#define BUTTON_OPEN   1	// Normally open
#define BUTTON_CLOSED 0

int button_state[3] = {0,0,0};


void init_gpio()
{
	// For use with the PWM expander:
	//pinMode( OE_pin, OUTPUT );
	//digitalWrite( OE_pin, 1 );		// make outputs active

	pinMode( LED_1, OUTPUT );
	pinMode( LED_2, OUTPUT );
	pinMode( LED_3, OUTPUT );
	pinMode( LED_4, OUTPUT );
	digitalWrite( LED_1, LED_ON );		// make outputs active
	digitalWrite( LED_2, LED_ON );		// make outputs active
	digitalWrite( LED_3, LED_ON );		// make outputs active
	digitalWrite( LED_4, LED_ON );		// make outputs active

	pinMode( BUTTON_1, INPUT );
	pinMode( BUTTON_2, INPUT );
	pinMode( BUTTON_3, INPUT );
}

char prev_b[3];
char b[3];

void read_buttons()
{
	b[0] = digitalRead( BUTTON_1 );
	b[1] = digitalRead( BUTTON_2 );
	b[2] = digitalRead( BUTTON_3 );
}

void wiggle_leds()
{

	static int led_bits = 0x01;

	digitalWrite( LED_1, (0x01 & led_bits) );
	digitalWrite( LED_2, (0x02 & led_bits) );
	digitalWrite( LED_3, (0x04 & led_bits) );
	digitalWrite( LED_4, (0x08 & led_bits) );

	// Shift which LED is lit to the next:
	led_bits = (led_bits<<1);
	if (led_bits == 0x10)
		led_bits = 0x01;
}

/********************************
 * Could be high or low
 * Set the 12 bit number 
 ********************************/
void set_pwm_time( int mBaseRegister, int mValue )
{
/*	mValue = mValue & 0x0FFF;
	int result1 = wiringPiI2CWriteReg8( fd,  mBaseRegister,   mValue );
	int result2 = wiringPiI2CWriteReg8( fd,  mBaseRegister+1, mValue );
	if ((result1) || (result2)) printf("(%d,%d)", result1, result2 ); */
}

int main()
{
	int result = wiringPiSetup();
	init_gpio();
	int pulse_on = 0;
	int value = 0;
	while(1==1)
	{
		pulse_on += 0x01;
		if (pulse_on == 0x03FF)	{

			wiggle_leds();

			read_buttons( );
			printf("\nButtons(1,2,3) = (%d,%d,%d)", b[0],b[1],b[2] );
			pulse_on = 0x0000;	// restart
		}
		usleep(500);
	}	
}

