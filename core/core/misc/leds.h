
#ifdef  __cplusplus
extern "C" {
#endif

/******************************************************* 
 SYSTEM LED COMMAND STRUCTURE 
	
*******************************************************/
#define SYSTEM_LED_MODE_DEVICE		0x00
#define SYSTEM_LED_MODE_PATTERN		0x01
#define SYSTEM_LED_MODE_STROBE		0x02
#define SYSTEM_LED_MODE_MYINSTANCE	0x03
#define SYSTEM_LED_MODE_CONT_PATTERN 0x04

struct sSystemLED {
	byte pattern;			// 
	byte systemMode;		// See above #defines
};
extern struct sSystemLED System_LED_Control;


// Pin_definitions.h must define:
/*   LED_n_PORT, LED_n_DDR, and LED_n  		*/
#ifdef LED_1
#define SET_LED_1() 	{ digitalWrite( LED_1, LED_ON );  }
#define RESET_LED_1() 	{ digitalWrite( LED_1, LED_OFF ); }
#else
#define SET_LED_1() 	{ }
#define RESET_LED_1() 	{ }
#endif

#ifdef LED_2
#define SET_LED_2() 	{ digitalWrite( LED_2, LED_ON ); 	}
#define RESET_LED_2() 	{ digitalWrite( LED_2, LED_OFF );	}
#else
#define SET_LED_2() 	{ }
#define RESET_LED_2() 	{ }
#endif 

#ifdef LED_3
#define SET_LED_3() 	{ digitalWrite( LED_3, LED_ON  ); 	}
#define RESET_LED_3() 	{ digitalWrite( LED_3, LED_OFF ); 	}
#else 
#define SET_LED_3() 	{ }
#define RESET_LED_3() 	{ }
#endif

#ifdef LED_4
#define SET_LED_4() 	{ digitalWrite( LED_4, LED_ON ); 	}
#define RESET_LED_4() 	{ digitalWrite( LED_4, LED_OFF );	}
#else
#define SET_LED_4() 	{ }
#define RESET_LED_4() 	{ }
#endif



#define HIGH_NIBBLE 1
#define LOW_NIBBLE  0

byte strobe_leds		( byte mOn 	);
void init_leds			(			);
void led_on				(byte mindex);
void led_off			(byte mindex);
void set_led			(byte mindex, byte value);

void show_byte			(byte result, byte mHiNibble );
void show_result_toggle (byte mresult 				 );
void led_timeslice_100ms(							 );

void can_parse_led_request( struct sCAN* mMsg );

#ifdef  __cplusplus
}
#endif

