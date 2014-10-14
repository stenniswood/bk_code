#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <wiringPi.h>
#include "pican_defines.h"
#include "leds.h"


void led_init()
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
}

void led_set( char mLEDNumber, char value )
{
	digitalWrite( mLEDNumber,  value );
}

