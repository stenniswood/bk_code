#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <wiringPi.h>
#include "pican_defines.h"
#include "button.h"

/***************************************************************
*  This file Tests the LEDs & buttons on the PiCAN board
*  3 LEDS    :
*  3 BUTTONS :
***************************************************************/
#define OE_pin 7

char b1,b2,b3;
int  pulse_on = 0;
char on_off   = 0;

void Button1_isr()
{
	printf("Button 1 Pressed Interrupt - reg_dump()\n");
	register_dump   ( );
	tx_register_dump(0);	
} 
void Button2_isr()
{
	printf("Button 2 Pressed Interrupt\n");	
	printf("--------------->>>>>>>\n");
	char txt[] = {"hello!!!"};
	pack_lcd_write_text( &msg1, 0, txt);
	AddToSendList( &msg1 );
	printf("--------------->>>>>>>\n");
} 
void Button3_isr()
{
	printf("Button 3 Pressed Interrupt\n");
}

/*  Setup Buttons, & LEDs	*/
void button_init()
{
	pinMode( BUTTON_1, INPUT );
	pinMode( BUTTON_2, INPUT );
	pinMode( BUTTON_3, INPUT );
}


void button_read()
{
	b1 = digitalRead( BUTTON_1 );
	b2 = digitalRead( BUTTON_2 );
	b3 = digitalRead( BUTTON_3 );
}

void button_Timeslice()
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



