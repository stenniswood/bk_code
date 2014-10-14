#ifdef LED_1
#define SET_LED_1() 	{ digitalWrite(LED_1, 1); }
#define RESET_LED_1() 	{ digitalWrite(LED_1, 0); }
#else
#define SET_LED_1() 	{ }
#define RESET_LED_1() 	{ }
#endif

#ifdef LED_2
#define SET_LED_2() 	{ digitalWrite(LED_2, 1); 	}
#define RESET_LED_2() 	{ digitalWrite(LED_2, 0); 	}
#else
#define SET_LED_2() 	{ }
#define RESET_LED_2() 	{ }
#endif 


void led_init();
void led_set( char mLEDNumber, char value );
