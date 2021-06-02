/* blink.c
 *
 * Raspberry Pi GPIO example using sysfs interface.
 * Guillermo A. Amaral B. <g@maral.me>
 *
 * This file blinks GPIO 4 (P1-07) while reading GPIO 24 (P1_18).
 */

#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "gpio_in.hpp"

//========================= DIRECT REGISTER ========================
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <bcm_host.h>

// Access from ARM Running Linux
//#define BCM2708_PERI_BASE     =   0x20000000;
unsigned long BCM2708_PERI_BASE =   bcm_host_get_peripheral_address();
#define GPIO_BASE                (BCM2708_PERI_BASE + 0x200000) /* GPIO controller */

#define PAGE_SIZE (4*1024)
#define BLOCK_SIZE (4*1024)

int   mem_fd;
void *gpio_map;

// I/O access
volatile unsigned *gpio;

// GPIO setup macros. Always use INP_GPIO(x) before using OUT_GPIO(x) or SET_GPIO_ALT(x,y)
#define INP_GPIO(g) *(gpio+((g)/10)) &= ~(7<<(((g)%10)*3))
#define OUT_GPIO(g) *(gpio+((g)/10)) |=  (1<<(((g)%10)*3))
#define SET_GPIO_ALT(g,a) *(gpio+(((g)/10))) |= (((a)<=3?(a)+4:(a)==4?3:2)<<(((g)%10)*3))

#define GPIO_SET *(gpio+7)  // sets   bits which are 1 ignores bits which are 0
#define GPIO_CLR *(gpio+10) // clears bits which are 1 ignores bits which are 0

#define GET_GPIO(g) (*(gpio+13)&(1<<g)) // 0 if LOW, (1<<g) if HIGH

#define GPIO_PULL *(gpio+37) // Pull up/pull down
#define GPIO_PULLCLK0 *(gpio+38) // Pull up/pull down clock

void set_gpio( int mPin )   {	GPIO_SET = 1<<mPin;  };
void clr_gpio( int mPin )   {	GPIO_CLR = 1<<mPin;  };
//========================= DIRECT REGISTER ========================

pthread_t gpioThread_id;
bool Bumper_ls[4];

#define PRESSED  true
#define RELEASED false


int  flash_leds = FLASH_MODE_BT_WAIT_CONNECTION;
int  flash_led_id = 1;
int  flash_led_delay = LED_FLASH_DELAY;
int  flash_led_counter = LED_FLASH_COUNTER_RESTART;


#define PUNCH_TIMEOUT 3
bool left_punch  		 = false;
bool right_punch 		 = false;
int  left_punch_counter  = PUNCH_TIMEOUT;
int  right_punch_counter = PUNCH_TIMEOUT;


bool GPIO_read( unsigned char mPin )
{
	bool value = (GET_GPIO(mPin)==0);
	if (value) printf("GPIO reads : pin %d = %d\n", mPin, value );	
	return value;
}

void flash_bumper()
{
		flash_led_counter--;
		if (flash_led_counter==0) {
			flash_led_counter = LED_FLASH_COUNTER_RESTART;
			flash_leds = false;
		}

		flash_led_delay--;
		if (flash_led_delay==0)
		{
			flash_led_delay=LED_FLASH_DELAY;		// restart
			if (flash_led_id==1)
			{
				set_gpio( LED1_OUT );
				clr_gpio( LED2_OUT );		
			} 
			else {
				set_gpio( LED2_OUT );
				clr_gpio( LED1_OUT );		
			}
			flash_led_id = !flash_led_id;
		}
}

void flash_wait_BTConnection()
{
		set_gpio( LED1_OUT );
		flash_led_delay--;
		if (flash_led_delay==0)
		{
			flash_led_delay = LED_FLASH_DELAY;		// restart
			if (flash_led_id == 1)
			{
				clr_gpio( LED2_OUT );		
			} 
			else {
				set_gpio( LED2_OUT );
			}
			flash_led_id = !flash_led_id;
		}

}

void* GPIO_thread_func ( void* )
{
	while (1==1) {	

		if (flash_leds)
		{		
			clr_gpio( LED_OK_OUT );

			
			switch (flash_leds)
			{
			case FLASH_MODE_BUMPER 			   : flash_bumper();			break;
			case FLASH_MODE_BT_WAIT_CONNECTION : flash_wait_BTConnection(); break;
			
			default : break;
			};
			
			
		} 
		else 
		{
			set_gpio( LED_OK_OUT );
				clr_gpio( LED1_OUT );		
				clr_gpio( LED2_OUT );										
	    } 
	    
	    if (left_punch)
	    {
	    	left_punch_counter--;
	    	if (left_punch_counter==0)
	    	{
	    		left_punch_counter = PUNCH_TIMEOUT;
	    		left_punch = false;
		    	set_gpio( PUNCH1_OUT );
	    	} else 
		    	clr_gpio( PUNCH1_OUT );
	    }

	    if (right_punch)
	    {
	    	right_punch_counter--;
	    	if (right_punch_counter==0)
	    	{
	    		right_punch_counter = PUNCH_TIMEOUT;
	    		right_punch = false;
		    	set_gpio( PUNCH2_OUT );
	    	} else 
		    	clr_gpio( PUNCH2_OUT );
	    }

		/** Read GPIO value */
		Bumper_ls[0] = GPIO_read(B_PIN1);
		Bumper_ls[1] = GPIO_read(B_PIN2);
		Bumper_ls[2] = GPIO_read(B_PIN3);
		Bumper_ls[3] = GPIO_read(B_PIN4);

		if (Bumper_ls[0])
			flash_leds = true;

		usleep(50 * 1000);		// 50ms  or 20x per second
	}

	/** Disable GPIO pins */
	//if (-1 == GPIOUnexport(POUT) || -1 == GPIOUnexport(PIN))
	//	return(4);	
	/* result = GPIOUnexport(B_PIN1);
	result = GPIOUnexport(B_PIN2);
	result = GPIOUnexport(B_PIN3);
	result = GPIOUnexport(B_PIN4);  */
	return NULL;
}

void set_pullup( unsigned char mPin )
{
	// enable pull-up on GPIO24&25
   GPIO_PULL = 2;			// PULL UP
   usleep(1000*50);
   
   // clock on GPIO 24 & 25 (bit 24 & 25 set)
	int mask = (1 << mPin);
   GPIO_PULLCLK0 = mask;   // 0x00200000; // 16+8=24
	usleep(1000*50);
	
   GPIO_PULL = 0;
   GPIO_PULLCLK0 = 0;
}

int start_gpio_thread()
{
	setup_io  ( );
	INP_GPIO  ( B_PIN1 );
	set_pullup( B_PIN1 );

	
	INP_GPIO  ( LED1_OUT );		OUT_GPIO  ( LED1_OUT );
	INP_GPIO  ( LED2_OUT );		OUT_GPIO  ( LED2_OUT );
	INP_GPIO  ( LED_OK_OUT );	OUT_GPIO  ( LED_OK_OUT );
	
	INP_GPIO  ( PUNCH1_OUT );	OUT_GPIO  ( PUNCH1_OUT );
	INP_GPIO  ( PUNCH2_OUT );	OUT_GPIO  ( PUNCH2_OUT );

	set_gpio( PUNCH1_OUT );
	set_gpio( PUNCH2_OUT );		    	

	INP_GPIO( MOTOR_REVERSE_DIRECTION );	OUT_GPIO( MOTOR_REVERSE_DIRECTION );
	set_gpio( MOTOR_REVERSE_DIRECTION );
	
	int result = pthread_create( &gpioThread_id, NULL, &GPIO_thread_func, NULL );
	if (result != 0) 
	{
		printf( "Unable to create the gpio thread.\n" );
		//exit(1);
		return 1;
	} else {
		printf( "Created the gpio thread.\n" );		
	}	
	return 0;
}

//=================== DIRECT REGISTER CODE =================================

//
// Set up a memory regions to access GPIO
//
void setup_io()
{
	printf("Direct REgister GPIO \n" );

   /* open /dev/mem */
   if ((mem_fd = open("/dev/mem", O_RDWR|O_SYNC) ) < 0) {
      printf("can't open /dev/mem \n");
      exit(-1);
   }

   /* mmap GPIO */
   gpio_map = mmap(
      NULL,             //Any adddress in our space will do
      BLOCK_SIZE,       //Map length
      PROT_READ|PROT_WRITE,// Enable reading & writting to mapped memory
      MAP_SHARED,       //Shared with other processes
      mem_fd,           //File to map
      GPIO_BASE         //Offset to GPIO peripheral
   );

   close(mem_fd); 		// No need to keep mem_fd open after mmap

   if (gpio_map == MAP_FAILED) {
      printf("mmap error %d\n", (int)gpio_map);//errno also set!
      exit(-1);
   }

   // Always use volatile pointer!
   gpio = (volatile unsigned *)gpio_map;

   printf("Direct Register GPIO= %p\n", gpio );
}

