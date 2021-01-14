#ifndef _SCAN_USB_DEVS_HPP_
#define _SCAN_USB_DEVS_HPP_


struct stDevInfo {
	char device[40];
	char board_name[20];
	char type[20];	
	DriveFive* df;
};

extern std::vector<struct stDevInfo> devices;

void  create_driveFive_objs();
void  scan_devices	 ( );
void  get_usb_devices( );
void  get_dev_types	 ( );
void  get_usb_names	 ( );
void  print_devices  ( );


#endif
