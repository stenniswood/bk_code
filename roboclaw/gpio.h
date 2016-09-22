#ifdef __cplusplus
extern "C" {
#endif 

extern char b1,b2,b3;
#define PUSHED 0
#define NOT_PUSHED 1

void gpio_init();
void gpio_read_buttons();
void gpio_SetLED( char mLEDNumber, char value );
void gpio_Timeslice();


#ifdef __cplusplus
 }
#endif 
