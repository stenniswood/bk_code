#ifndef _SCAN_USB_DEVS_HPP_
#define _SCAN_USB_DEVS_HPP_

#include <vector>
#include "drive_five.h"


struct stDevInfo {
	char device[40];
	char board_name[20];
	char type[20];	
	DriveFive* df;
};

extern std::vector<struct stDevInfo> devices;

void  create_driveFive_objs();
void  scan_devices	 ( );		// top level which runs all functions below:
void  get_usb_devices( );		// Reads /dev/ttyUSB* dir entries and populates "devices" 
void  get_dev_types	 ( );
void  get_usb_names	 ( );
void  print_devices  ( );

int   get_devname_path( const char* mName );


#endif
