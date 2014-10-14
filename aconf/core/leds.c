#include "bk_system_defs.h"
#include <wiringPi.h>
//#include "inttypes.h"
//#include "interrupt.h"
//#include "pin_definitions.h"
#include "can_eid.h"
#include "can.h"
#include "can_instance.h"
#include "can_board_msg.h"

#include "../pican_defines.h"
#include "leds.h"


struct sSystemLED System_LED_Control;


void init_leds()
{	// OUTPUTS:
	pinMode( LED_1, OUTPUT );
	pinMode( LED_2, OUTPUT );
	pinMode( LED_3, OUTPUT );
	pinMode( LED_4, OUTPUT );
	
  	RESET_LED_1();
	RESET_LED_2();
	RESET_LED_3();
	RESET_LED_4();
}

/* Not to be called by any user outside of this file! */
void protected_led_on(byte mindex)
{
  switch(mindex)
  {
   case 1:  SET_LED_1();  break;
   case 2:  SET_LED_2();  break;
   case 3:  SET_LED_3();  break;
   case 4:  SET_LED_4();  break;
   default: break;
  }
}
/* Not to be called by any user outside of this file! */
void protected_led_off(byte mindex)
{
  switch(mindex)
  {
   case 1:  RESET_LED_1();  break;
   case 2:  RESET_LED_2();  break;
   case 3:  RESET_LED_3();  break;
   case 4:  RESET_LED_4();  break;
   default: break;
  }
}

/* Not to be called by any user outside of this file! */
void protected_set_led( byte mindex, byte value )
{
	if (value)
		led_on (mindex);
	else
		led_off(mindex);
}
//delay(one_second/STROBE_SPEED);

/* Useful for Verifying Pin connections (Initial board test) */
void strobe_leds( byte mOn )
{
	static byte counter= STROBE_SPEED;	
	static byte i=1;

	if ((System_LED_Control.systemMode == SYSTEM_LED_MODE_DEVICE) ||
		(System_LED_Control.systemMode == SYSTEM_LED_MODE_STROBE))
	{
		if (counter-->0)	return;
		counter = STROBE_SPEED;
		i++;
		if (mOn) protected_led_on( i ); else protected_led_off( i );
		if (i>=5) i=1;
	}
}

void led_on(byte mindex)
{
	if (System_LED_Control.systemMode)	return;		// no operations in system mode!
	protected_led_on(mindex);
}

void led_off(byte mindex)
{
	if (System_LED_Control.systemMode)	return;		// no operations in system mode!
	protected_led_off(mindex);
}

void set_led( byte mindex, byte value )
{
    if (System_LED_Control.systemMode)	return;		// no operations in system mode!
	protected_set_led( mindex, value );
}

/********************************************************************
Light the indicator LEDs to show the bit pattern specified.
INPUT:
	result	  : the byte to show (note only 1/2 of this can be shown at a time)
	mHiNibble : TRUE => show the upper nibble
				FALSE => show the lower nibble
********************************************************************/
void protected_show_byte( byte result, byte mHiNibble )
{
  if (mHiNibble) {
	  if ((result & 0x80)>0)  SET_LED_4()  else RESET_LED_4() ;
	  if ((result & 0x40)>0)  SET_LED_3()  else RESET_LED_3() ;
	  if ((result & 0x20)>0)  SET_LED_2()  else RESET_LED_2() ;
	  if ((result & 0x10)>0)  SET_LED_1()  else RESET_LED_1() ;
  } else {
	  if ((result & 0x08)>0)  SET_LED_4()  else RESET_LED_4() ;
	  if ((result & 0x04)>0)  SET_LED_3()  else RESET_LED_3() ;
	  if ((result & 0x02)>0)  SET_LED_2()  else RESET_LED_2() ;
	  if ((result & 0x01)>0)  SET_LED_1()  else RESET_LED_1() ;    
  }
}

/********************************************************************
Light the indicator LEDs to show the bit pattern specified.
INPUT:
	result	  : the byte to show (note only 1/2 of this can be shown at a time)
	mHiNibble : TRUE => show the upper nibble
				FALSE => show the lower nibble
********************************************************************/
void show_byte( byte result, byte mHiNibble )
{
	if (System_LED_Control.systemMode)	{
	  result = System_LED_Control.pattern;		// we override the value with the system requested pattern.
	}
    protected_show_byte( result, mHiNibble );
}

byte hi_low = 1;
void show_result_toggle( byte mresult )
{
    if (System_LED_Control.systemMode)	return;		// no operations in system mode!

	if (hi_low)
      protected_show_byte( mresult, 1 );
	else 
      protected_show_byte( mresult, 0 );

	hi_low = ! hi_low;		// toggle
}

/***************************************************************
 A system LED request message is intended for this board.  
 ID_SYSTEM_LED_REQUEST
 
INPUT:

	data[0] Lower nibble contains the desired action (MODE) [0,1]
			0 = release system control of the led indicators (to whatever functionality the specific module uses them for)
			1 = override device purpose to show the system requested pattern.
			2 = override device purpose to strobe the leds
			3 = override device purpose to flash the MyInstance assigned to this board.
	data[1] Lower nibble
			contains the 4 bit pattern.  If the board only has 3 leds, then they 
			will use the lowest 3 bits of this nibble.
**************************************************************/
void can_parse_led_request( struct sCAN* mMsg )
{
	System_LED_Control.systemMode = (mMsg->data[0] & 0x0F);
	System_LED_Control.pattern    = (mMsg->data[1] & 0x0F);

	switch (System_LED_Control.systemMode)
	{
	case SYSTEM_LED_MODE_DEVICE 	: break;
	case SYSTEM_LED_MODE_PATTERN 	: show_byte(System_LED_Control.pattern, LOW_NIBBLE);
									  break;
	case SYSTEM_LED_MODE_STROBE 	: strobe_leds( System_LED_Control.pattern );
									  break;
	case SYSTEM_LED_MODE_MYINSTANCE : System_LED_Control.pattern = MyInstance;
									  show_result_toggle( MyInstance );
									  break;
	default :   break;
	}
}

#define BLINK_RATE 10
byte    LED_state = 0;

void led_timeslice_100ms()
{
	static byte counter = 1;
	switch (System_LED_Control.systemMode)
	{
	case SYSTEM_LED_MODE_DEVICE 	: 
									  break;
	case SYSTEM_LED_MODE_PATTERN 	: show_byte(System_LED_Control.pattern, LOW_NIBBLE);	
									  break;
	case SYSTEM_LED_MODE_STROBE 	: strobe_leds( System_LED_Control.pattern );										
									  break;
	case SYSTEM_LED_MODE_MYINSTANCE : if (counter--==0) {
										counter = BLINK_RATE;
										show_result_toggle( MyInstance );
									}
									break;
	default :   break;
	}
}


