#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <wiringPi.h>
#include "pican_defines.h"
#include "gpio.h"

/***************************************************************
*  This file Tests the LEDs & buttons on the PiCAN board
*  3 LEDS    :
*  3 BUTTONS :
***************************************************************/
#define OE_pin 7


char b1,b2,b3;
int  pulse_on = 0;
char on_off   = 0;

/*  Setup Buttons, & LEDs	*/
void gpio_init()
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
	
	pinMode( CAN_INT_PIN, INPUT );
}


void gpio_read_buttons()
{
	b1 = digitalRead( BUTTON_1 );
	b2 = digitalRead( BUTTON_2 );
	b3 = digitalRead( BUTTON_3 );
}

void gpio_SetLED( char mLEDNumber, char value )
{
	digitalWrite( mLEDNumber,  value );
}

void gpio_Timeslice()
{
	pulse_on += 0x01;

	if (pulse_on == 0x03FF)	{
		on_off = !on_off;
		gpio_SetLED( LED_1, on_off);
		gpio_read_buttons( );
		printf("\nButtons(1,2,3) = (%d,%d,%d)", b1,b2,b3 );
		pulse_on = 0x0000;
	}
}



