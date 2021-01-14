#include <stdio.h>
#include <math.h>
#include <sys/types.h>
#include <dirent.h>
#include <string>
#include <string.h>
#include <vector>

#include "drive_five.h"
//#include "motor_mapping.h"
//#include "cal_file.hpp"
#include "scan_usb_devs.hpp"

#include "sserial.hpp"


std::vector<struct stDevInfo> devices;

/* Reads /dev/ttyUSB* device list and
 Populates the "devices" vector defined above.
*/
void get_usb_devices()
{
	struct stDevInfo tmp;
    DIR* d = opendir( "/dev/" );
	struct dirent*  de = readdir( d );
	while (de != NULL)
	{
		int result = strncmp( de->d_name, "ttyUSB", 6 );
		int result2 = strncmp( de->d_name, "ttyACM", 6 );
		if ((result==0) || (result2==0))
		{
			// FOUND 1, add to the list :
			strcpy (tmp.device, "/dev/" );
			strcat (tmp.device, de->d_name );
			strcpy(tmp.board_name, "unknown");
			strcpy(tmp.type,       "unknown");
			//printf("%s\n",tmp.device );
			devices.push_back( tmp );
		}
		de = readdir( d );
	}
}

void print_devices()
{
	printf("=================USB DriveFive Devices Found ==========================\n");
	printf("Device\tPath  \t\tType  \t\tName \n");
	for (int i=0; i<devices.size(); i++)
	{
		printf("%2d \t%-15s\t%-15s\t%-20s\n", i, devices[i].device, devices[i].type, devices[i].board_name );
	}
	printf("====================================================================\n");
}

void get_dev_types()
{
	SSerial dv;
	for (int i=0; i<devices.size(); i++)
	{
		dv.open( devices[i].device );
		dv.flush_inbuff();

		// GET DEVICE TYPE:
		char* type = dv.get_device_type();
		if (type)
			strcpy( devices[i].type, type );
		// GET NAME:
		char* name = dv.get_device_name();
		if (name)
			strcpy( devices[i].board_name, name );
		dv.close();
	}
}


void create_driveFive_objs()
{
	//printf("========== CREATING DRIVE FIVES =====================\n");
	DriveFive* df = NULL;
	
	// For each USB of type DriveFive, create a DriveFive object : 
	for (int d=0 ; d<devices.size(); d++)
	{
		int typeMatch = strncmp( devices[d].type, "DriveFive", 9 )==0;
		//printf("Type = |%s|;  Match=%d\n", devices[d].type, typeMatch );
		
		if ( typeMatch )
		{
			//printf("Creating DriveFive at %s\n", devices[d].device);
			df = new DriveFive( devices[d].device );
			df->open( );
			
			devices[d].df = df;
		}
	}
}


int get_devname_path( const char* mName )
{
	for (int d=0 ; d<devices.size(); d++)
	{
		int result = strcmp( devices[d].board_name, mName );
		if (result==0)
		{
			return d;
		}
	}
	return -1;
}


void scan_devices()
{
 	get_usb_devices();
	get_dev_types  ();

	//create_driveFive_objs();
	print_devices  ();
}

