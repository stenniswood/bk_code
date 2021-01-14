#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <linux/input.h>
#include "sserial.hpp"
#include "send_streams.hpp"
#include "load_cell.hpp"


extern SSerial pendant;


char response[1024] = "";
char Keyword[128]   = "";
char Value[128]     = "";


void split_keyword_value( char* mLine, char* mKeyword, char* mValue )
{
	char* ptr = strchr(mLine, ':');
	if (ptr==NULL) 
		strcpy( mKeyword, mLine );
	else {  
		*ptr = 0;
		strcpy( mKeyword, mLine );
		strcpy( mValue,   (ptr+1) );
	}
}


void pendant_active_stream(bool mValue) {

	if (mValue) {
		char msg[] = "stream\r\n";
		pendant.restart_buffer();		
		pendant.writeStr ( msg );
		send_pendant_data = true;
	} else {
		char msg[] = "stop\r\n";
		//pendant.acknowledge_rxd();		
		pendant.writeStr ( msg );	
		send_pendant_data = false;		
	}
	printf("send_pendant_data=%d\n", send_pendant_data );
}


void left_loadcells_active_stream(bool mValue) 
{
	if (mValue) {
		char msg[] = "stream\r\n";
		loadcell_left.restart_buffer();		
		loadcell_left.writeStr ( msg );
		send_l_load_data = true;
	} else {
		char msg[] = "stop\r\n";
		//pendant.acknowledge_rxd();		
		loadcell_left.writeStr ( msg );	
		send_l_load_data = false;		
	}
	printf("left_loadcells_active_stream=%d\n", send_l_load_data );
}

void right_loadcells_active_stream(bool mValue) 
{
	if (mValue) {
		char msg[] = "stream\r\n";
		loadcell_right.restart_buffer();		
		loadcell_right.writeStr ( msg );
		send_r_load_data = true;
	} else {
		char msg[] = "stop\r\n";
		//pendant.acknowledge_rxd();		
		loadcell_right.writeStr ( msg );	
		send_r_load_data = false;		
	}
	printf("right_loadcells_active_stream=%d\n", send_r_load_data );
}



int process_server_cmd( char* mKeyword, char* mValue )
{
	//printf("Rxd Keyword=%80s", mKeyword );
	//printf(";\tValue=%20s\n", mValue );
	
	int result = (strcmp(mKeyword, "DEV_TYPE")==0);
	if (result) {
		strcpy( response, "RPI");
	}

	result = (strcmp(mKeyword, "USB_DEVS")==0);
	if (result) {
		//for (int d=0; d<NumDevs; d++)
		{
//			strcat( response, device[d].type );
//			strcat( response, device[d].name );
		}
	}

	result = (strcmp(mKeyword, "SEND_PENDANT")==0);
	if (result) {
		int value = (strcmp(mValue, "ON")==0);
		if ((false==send_pendant_data) && (value))
			pendant_active_stream(value);
		else if ((true==send_pendant_data) && (value==0))
			pendant_active_stream(value);
	}

	result = (strcmp(mKeyword, "SEND_LEFT_LOADCELLS")==0);
	if (result) {
		int value = (strcmp(mValue, "ON")==0);	
		strcpy( response, "LEFT");
		if ((false==send_l_load_data) && (value))
			left_loadcells_active_stream(value);
		else if ((true==send_l_load_data) && (value==0))
			left_loadcells_active_stream(value);
	}
	
	result = (strcmp(mKeyword, "SEND_RIGHT_LOADCELLS")==0);
	if (result) {
		int value = (strcmp(mValue, "ON")==0);	
		strcpy( response, "RIGHT");
		if ((false==send_r_load_data) && (value))
			right_loadcells_active_stream(value);
		else if ((true==send_r_load_data) && (value==0))
			right_loadcells_active_stream(value);
	}
	
	result = (strcmp(mKeyword, "SEND_DRIVEFIVE_POSITIONS")==0);
	if (result) {
		int value = (strcmp(mValue, "ON")==0);		
		strcpy( response, "DF");
		//drivefive_active_stream(value);		
	}
	
	result = (strcmp(mKeyword, "EXIT")==0);
	if (result) {
		return 1;
	}
	result = (strcmp(mKeyword, "SHUTDOWN")==0);
	if (result) {
		return 2;
	}
	
	//printf( "Sending Response %s\n", response );
	return 0;
}

