#include <iostream>
#include <list>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <time.h> 
#include "protocol.h"
#include "devices.h"
//#include "package_commands.h"
#include <termios.h>
#include <unistd.h>
#include <pthread.h>
#include <string>
//#include "udp_transponder.h"
#include "serverthread.h"
#include "CAMERA_device.hpp"

// Here we need to establish IPC to the atiltcam/ process.
// This is done via Shared memory.

// Pull in the PiCamScan IPC_shared_memory ()
void set_camera_tilt_to( float mDegrees )
{
	
}
void set_camera_pan_to( float mDegrees )
{
	
}

/*****************************************************/
/* Relative move: */
void lower_camera_by( float mDegrees )
{
	
}
/* Relative move: */
void raise_camera_by( float mDegrees )
{
	
}

/* Relative move: */
void move_camera_left_by( float mDegrees )
{
	
}
/* Relative move: */
void move_camera_right_by( float mDegrees )
{
	
}


float get_camera_tilt( float mDegrees )
{
    return 0.0;
}
float get_camera_pan( float mDegrees )
{
    return 0.0;	
}


