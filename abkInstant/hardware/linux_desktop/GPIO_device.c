#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <string>
#include <list>
#include <sys/types.h>
#include <time.h> 
#include "protocol.h"
//#include "devices.h"
#include "GENERAL_protocol.h"
#include "bk_system_defs.h"
#include "GPIO_device.h"


#ifdef RPI_GPIO
#include <wiringPi.h>
#else
#define INPUT 0
#define OUTPUT 1
#define PWM_OUTPUT 2
#define GPIO_CLOCK 3
#define LOW 0
#define HIGH 1
#endif


BOOL set_GPIO( byte mPin, byte mValue )
{
  #ifdef RPI_GPIO					  
	//  if (PinModes[Pin]==OUTPUT)		// if not configured, do nothing.
		digitalWrite( mPin, mValue);
  #endif
}

BOOL read_GPIO( byte mPin )
{
  #ifdef RPI_GPIO					  
		*mValue = digitalRead( mPin );
  #endif
}

BOOL set_GPIO_DDR( byte mPin, byte mInput )
{
#ifdef RPI_GPIO 
	if (mInput)
		pinMode( mPin, INPUT);
	else
		pinMode( mPin, OUTPUT);
#endif
}
