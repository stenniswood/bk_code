#include <stdio.h>
#include <math.h>
#include <sys/types.h>
#include <dirent.h>
#include <string>
#include <string.h>
#include <vector>

#include "drive_five.h"
#include "motor_mapping.h"
#include "cal_file.hpp"
#include "scan_usb_devs.hpp"



std::vector<struct stDevInfo> devices;

/* Reads /dev/ttyUSB* device list and
 Populates the "devies" vector defined above.
*/
void get_usb_devices()
{
	struct stDevInfo tmp;
    DIR* d = opendir( "/dev/" );
	struct dirent*  de = readdir( d );
	while (de != NULL)
	{
		int result = strncmp( de->d_name, "ttyUSB", 6 );
		if (result==0) 
		{
			// FOUND 1, add to the list :
			strcpy (tmp.device, "/dev/" );
			strcat (tmp.device, de->d_name );
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
	DriveFive df;
	for (int i=0; i<devices.size(); i++)
	{
		df.open( devices[i].device );
		char* type;
		df.send_command("device type");
		df.read_response();
		//printf("get_dev_types: response= %s \n", df.m_response);
		
		// split at :  extract after that.
		char* comma = strchr(df.m_response, ':')+1;
		char* eol   = strchr(df.m_response, '\r');		*eol = 0;
		strcpy( devices[i].type, comma );

		remove_leading_whitespace( devices[i].type );
		
		//printf("USB Dev:%s = %s\n", devices[i].device, devices[i].type );		
		df.close();
		//printf("---  %s\n",  devices[i].board_name );
	}
}

void get_usb_names()
{
	DriveFive df;
	for (int i=0; i<devices.size(); i++)
	{
		df.open( devices[i].device );
		char* name = df.get_device_name();
		//printf( "USB Name=%s\n", name );
		strcpy( devices[i].board_name, name+1 );
		df.close();
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

void scan_devices()
{
 	get_usb_devices();
	get_dev_types  ();
	get_usb_names  ();
	create_driveFive_objs();
	print_devices  ();
}

