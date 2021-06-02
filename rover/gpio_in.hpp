#ifndef _GPIO_IN_HPP_
#define _GPIO_IN_HPP_


#define IN  0
#define OUT 1

#define LOW  0
#define HIGH 1

#define B_PIN1  21 /* P1-18 */
#define B_PIN2  21 /* P1-18 */
#define B_PIN3  21 /* P1-18 */
#define B_PIN4  21 /* P1-18 */

#define LED1_OUT 16 /* RED FLASH BUMPER INDICATOR 1 */		
#define LED2_OUT 19 /* RED FLASH BUMPER INDICATOR 2 */
#define LED_OK_OUT 26 /* GREEN GO LED!  */

#define PUNCH1_OUT 6  /* LEFT PUNCHER  */
#define PUNCH2_OUT 12 /* RIGHT PUNCHER */

#define MOTOR_REVERSE_DIRECTION 20 /* MOTOR_REVERSE_DIRECTION */

/////////////////////////////////////////////////////

// LED FLASHING DEFINES:
#define LED_FLASH_DELAY 3
#define LED_FLASH_COUNTER_RESTART 45
#define FLASH_MODE_NONE   0
#define FLASH_MODE_BUMPER 1
#define FLASH_MODE_BT_WAIT_CONNECTION 2
#define FLASH_MODE_BT_PAIRING 3
extern int  flash_leds;


extern bool Bumper_ls[4];


extern void set_gpio( int mPin );
extern void clr_gpio( int mPin );

int   start_gpio_thread( );
void* GPIO_thread_func ( void* );


extern bool left_punch ;
extern bool right_punch;


// DIRECT REGISTER : 
void setup_io();


#endif
