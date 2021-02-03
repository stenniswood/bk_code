#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>

#include <fcntl.h>
#include <linux/input.h>
#include <unistd.h>
#include "crc_32.h"
#include "joystick_raw.hpp"
#include "ps4_hid_outputs.hpp"


int adr = 0;
	
/*void append_1b( int& mAdr, unsigned char mData )
{
	oBuffer[mAdr++] = (mData & 0x00FF);
}
void append_2b( int& mAdr, unsigned short mData )
{
	oBuffer[mAdr++] = (mData & 0x00FF);			// PS4 uses Little endian
	oBuffer[mAdr++] = (mData & 0xFF00) >> 8;
}

/*	HID Report header & footer  *
int form_header()
{
	append_2b( adr,  0x02  );
	append_2b( adr, 0x1520 );
	append_2b( adr, 0x5300 );
	append_2b( adr, 0x4F00 );
	append_2b( adr, 0x4200 );	
	return adr;
}

int form_HID_portion( unsigned char* mData )
{
	append_1b( adr,  0xA1  );
	append_1b( adr,  0x11  );
	append_1b( adr,  0xC0  );

	// Now Pack 72 bytes of the data here!
	return adr;
}
void form_full_packet( unsigned char* mData, int mLength )
{
	form_header();
	form_HID_portion( mData );
	//append_checksum ( mData, mLength );
}*/


JoystickRaw joy;
std::string path_name;
void help_useage() 
{
	printf(" **** PS4 TEST APP USING HID RAW INPUT DEVICE        ****\n");
	printf("This opens a connection to the /dev/hidraw3 user space device.\n");
	printf("Collects report data and has a joystickRaw class to parse the results.\n");
	printf("\n");	
	printf( "USEAGE:  hidraw <device_name>.   \n" );
	printf("\n");		
	printf("see 'ls /dev/hidraw*' or check output of ds4drv\n");
	printf("\n");
	printf(" **** PS4 CONTROLLER MAPPING ****\n");	
}

int main(int argc, char** argv )
{
	bool show_buttons = true;
	bool show_axis    = true;
	bool show_dump    = true;
	bool show_touchpad= true;
		
     if (argc != 2)
     {	
     	help_useage();
     	return -1;
     }
     	
	 path_name = "/dev/";
	 path_name += argv[1];
	 joy.open( path_name.c_str() );


	 byte red = 255;
	 byte green = 0;
	 byte blue  = 0;

	 byte Strong = 0xFF;
	 byte Weak   = 0xFF;
	 send_Rumble( joy.joystick_fd, Strong,  Weak );
	 sleep(3);
	 send_Rumble( joy.joystick_fd, Strong,  0 );
	 sleep(3);
	 send_Rumble( joy.joystick_fd, 0, Weak );
	 sleep(3);
	 send_Rumble( joy.joystick_fd, 0,  0 );
	 
	 while(1) { 
		 set_LED_color( joy.joystick_fd, red, green, 0x02 );	 
		 green += 10;
		 red   -= 10;
//		 if (red)
		usleep(100000);
		 
	 };
	 
	 while (1) {
		joy.update();
		if (show_buttons)	joy.print_button_info ( );
		if (show_axis)		joy.print_analogs_info( );
		if (show_dump)		joy.dump_i_buffer	  ( );
		if (show_touchpad)	joy.print_touchpad_info();

		ButtonState& b1 = joy.get_button( PS_SQUARE   );
		ButtonState& b2 = joy.get_button( PS_X        );
		ButtonState& b3 = joy.get_button( PS_CIRCLE   );
		ButtonState& b4 = joy.get_button( PS_TRIANGLE );		
		//printf("TOGGLE BUTTONS: %d %d %d %d\n", b1.pressed, b2.pressed, b3.pressed, b4.pressed );
		//printf("SHOW B,A,D,T  : %d %d %d %d\n", show_buttons, show_axis, show_dump, show_touchpad );
		
		if (b1.pressed)	{ b1.acknowledge();  show_buttons = !show_buttons;	printf("TOGGLE Bu\n");		sleep(1); }
		if (b2.pressed)	{ b2.acknowledge();  show_axis 	  = !show_axis;		printf("TOGGLE Ax\n");		sleep(1); }
		if (b3.pressed)	{ b3.acknowledge();  show_dump 	  = !show_dump;		printf("TOGGLE Du\n");		sleep(1); }
		if (b4.pressed)	{ b4.acknowledge();  show_touchpad= !show_touchpad;	printf("TOGGLE Tp\n");		sleep(1); }

     };

	return 0;
}
