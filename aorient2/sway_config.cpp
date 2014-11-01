#include <string.h>
#include <string>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <wiringPiSPI.h>
#include <wiringPi.h>
#include "pican_defines.h"
#include "CAN_Interface.hpp"
#include "packer_lcd.h"
#include "can_txbuff.h"
#include "packer.h"
#include "system_msgs_callback.h"
#include "board_list.h"
#include "can_id_list.h"
#include "cmd_process.h"
#include "buttons.h"
#include "leds.h"
#include "vector_math.h"
#include "parser_tilt.h"
#include "catagorization.h"
#include "fuse_ag.h"
#include "fuse_gyro.h"
#include "preferences.hpp"
#include "help.h"
#include <pthread.h>
#include "packer_motor.h"
#include "fuse_accel.h"
#include "sway_config.hpp"



SwayConfig sc;


SwayConfig::SwayConfig(char* mFileName)
: Preferences(mFileName)
{
	loaded = false;
	printf("SwayConfig()...\n");
}

void SwayConfig::load_file(char* mFileName )
{
	printf("load_file...\n");
	if (file_exists())	{
		load_all_keys(); 
		loaded = true;
	} else 	
		use_default_config();		
}

byte  SwayConfig::use_default_config()
{
	struct sKeyValuePair* ptr = NULL;

	ptr = new  sKeyValuePair( (const char*)"LeftMotorInstance", (const char*)"21" );
	key_table.push_back( ptr );
	ptr = new  sKeyValuePair( "RightMotorInstance", "21" );
	key_table.push_back( ptr );
	ptr = new  sKeyValuePair( "LeftAlpha", "0.01" );
	key_table.push_back( ptr );
	ptr = new  sKeyValuePair( "RightAlpha", "0.01" );
	key_table.push_back( ptr );

	ptr = new  sKeyValuePair( "Axis", "x" );
	key_table.push_back( ptr );	
}

byte	SwayConfig::get_LeftMotorInstance()
{
	byte retval = -1;
	try {	
		retval = (byte)find_int("LeftMotorInstance" );
		return retval;
	} 
	catch(char const* str)
	{
		printf("exception %s\n", str);
		return -1;
	};
}
byte	SwayConfig::get_RightMotorInstance()
{
	byte retval = -1;
	try {	
		retval = (byte)find_int("RightMotorInstance" );
		return retval;
	}
	catch(char const* str)
	{
		printf("exception %s\n", str);
		return -1;
	};
}

float	SwayConfig::get_LeftAlpha(byte mBinNumber)
{
	char alpha_bin[12];
	sprintf(alpha_bin, "Alpha%dL",mBinNumber );
	//string alpha_bin = "Alpha"+mBinNumber+"L"; 
	return (float)find_float( alpha_bin );
}
float	SwayConfig::get_RightAlpha(byte mBinNumber)
{
	char alpha_bin[12];
	sprintf(alpha_bin, "Alpha%dL",mBinNumber );

//	std::string alpha_bin = "Alpha"+mBinNumber+"R"; 
	return (float)find_float( alpha_bin );
}

void SwayConfig::set_LeftAlpha(byte mBinNumber, float mNewValue)
{
	char alpha_bin[12];
	sprintf(alpha_bin, "Alpha%dL",mBinNumber );
	find_float( alpha_bin );
	
	// NOW CHANGE IT:
	struct sKeyValuePair* ptr = find_key(alpha_bin);
	if (ptr==NULL)
		throw "Not found";
	sprintf(alpha_bin, "%1.5f", mNewValue);
	ptr->value = alpha_bin;
	// Should not have to update the ptr in the list or anything.
}
void SwayConfig::set_RightAlpha(byte mBinNumber, float mNewValue)
{
	char alpha_bin[12];
	sprintf(alpha_bin, "Alpha%dR",mBinNumber );
	find_float( alpha_bin );
	
	// NOW CHANGE IT:
	struct sKeyValuePair* ptr = find_key(alpha_bin);
	if (ptr==NULL)
		throw "Not found";
	sprintf(alpha_bin, "%1.5f", mNewValue);
	ptr->value = alpha_bin;
	// Should not have to update the ptr in the list or anything.
}


// These angels are hard coded for now!
byte	SwayConfig::get_bin_number( float angle )
{
	if (angle>45.)	return 8;
	if (angle>20.)	return 7;
	if (angle>10.)	return 6;
	if (angle>0)	return 5;
	
	if (angle>-10.)	return 4;
	if (angle>-20.)	return 3;
	if (angle>-45.)	return 2;
	if (angle<-45.)	return 1;
	return 0;
}

// use 'L' or 'R'
char* SwayConfig::get_bin_name( float angle, char mLeftOrRight )
{
	static char bin_name[16];
	byte bin = get_bin_number(angle);
	sprintf(bin_name, "Alhpa%d%c", angle, mLeftOrRight );
	printf ("get_bin_name:  %s\n", bin_name );
	return bin_name;
}

char	SwayConfig::get_Axis()
{
	char* axis = (char*)find_string("TiltAxis");
	// force to lower case!	
	return *axis;
}

void SwayConfig::print_all_params()
{
	std::list<sKeyValuePair*>::iterator iter = key_table.begin();
	for (; iter != key_table.end(); iter++)
	{
		printf( "%s\t\t= %s\n", (*iter)->key.c_str(), (*iter)->value.c_str() );
	}

/*	printf("LeftInstance = %d\n",  	get_LeftMotorInstance () );
	printf("RightInstance = %d\n", 	get_RightMotorInstance() );
	for (int i=0; i<8; i++)
		printf("Alpha%dL = %6.4f\n", i,	get_LeftAlpha(i)    );
	for (int i=0; i<8; i++)
		printf("RightAlpha = %6.4f\n", i, get_RightAlpha(i) );
	printf("Axis = %c\n", 			get_Axis()				 );
	printf("===============================================\n");	*/
}


