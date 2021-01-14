
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
#include <pthread.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <bluetooth/rfcomm.h>

#include "sserial.hpp"
#include "bluetooth_server.hpp"
#include "server_cmds.hpp"
#include "send_streams.hpp"
#include "load_cell.hpp"
#include "scan_usb_devs.hpp"


SSerial pendant;
enum ebtStates { UNCONNECTED, CONNECTED, EXIT, SHUTDOWN };
enum ebtStates BT_State = UNCONNECTED;
bool new_cmd_available  = false;
bool okay_to_change_new_cmd_available = true;		// thread safe 

pthread_t tid;
int btt = 1;


void wait_semaphore()
{
	while (okay_to_change_new_cmd_available==false) {};
	okay_to_change_new_cmd_available=false;	// The other thread has to set this!
}
void signal_semaphore()
{
	okay_to_change_new_cmd_available=true;
}


void BT_timeslice() 
{
	switch (BT_State)
	{
	case UNCONNECTED : 
			printf("Calling bluetooth Server\n");	
			bluetooth_server();			
			BT_State = CONNECTED;
			break;
	case CONNECTED : 
			if (read_socket() >= 0)  {
				char* delim = strchr(buf, ':');
				if (delim) {
					wait_semaphore();
					split_keyword_value( buf, Keyword, Value );
					new_cmd_available = true;
					printf("BT New Cmd Available!\n");
				}
			};			
			break;
	case EXIT : 
			printf("Closing connection!\n\n");
			close_socket();			
			BT_State = UNCONNECTED;
			break;
	case SHUTDOWN : 
			close_socket();
			break;
	default : 
			break;
	}
}

void* bt_thread(void* )
{
	while(1) {
		BT_timeslice();	
	}
}

void setup() 
{
	printf("r = %x;   n=%x; \n", '\r', '\n' );

	scan_devices();
	
	int left_load_index = get_devname_path( "LEFT" );
	int right_load_index = get_devname_path( "RIGHT" );
	int pendant_index = get_devname_path( "PEN1" );
	printf("left_li=%d;  right_li=%d;  pendant_i=%d\n",
		left_load_index, right_load_index, pendant_index	);
	
	
//	printf("LEFT  LC=%d - %s\n", left_load_index, devices[left_load_index].device);
//	printf("RIGHT LC=%d - %s\n", right_load_index, devices[right_load_index].device);

		
	setup_load_cell(devices[left_load_index].device,  
					devices[right_load_index].device );


	if (pendant_index>-1)
	{
		pendant.open( devices[pendant_index].device );
		usleep( 10000 );	
		char msg[] = "stop\r\n";
		pendant.writeStr(msg);
	}

	//bluetooth_nearby_devices();
	BT_State = UNCONNECTED;
	
	btt = pthread_create( &tid, NULL, bt_thread, NULL);
	printf("BT Thread Create btt= %d\n", btt );
}


void loop() 
{
	//if (send_l_load_data) read_data_left ();
	//if (send_r_load_data) read_data_right();	
	
	int full_line = 0;

	if (pendant.available() ) {
		full_line = pendant.readLine1();
		if (full_line)  printf("PENDANT serial rxd:%s\r\n", pendant.rx_line );
	}
	send_streams();
	
	if (new_cmd_available)		// BlueTooth leave us a CMD?
	{
		int exit_socket = process_server_cmd( Keyword, Value );
		if (exit_socket==1)  BT_State = EXIT;
		if (exit_socket==2)  BT_State = SHUTDOWN;	
		new_cmd_available = false;
		signal_semaphore();
	}
}


int main() 
{	
	char line2[]= "1023 0, 522, 519, 0, 1023,842, 895, 853, 833, ";
	char line[] = "1023, 0, 522, 519, 0, 1023,843, 896, 853, 833, ";
	char xsum = compute_xor_xsum( line );	
	printf("xsum = %c; %x\n", xsum, xsum );

	setup();
	while(1)	loop();
	pendant.close();	  
	return 1;
}
