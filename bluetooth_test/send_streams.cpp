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

#include "bluetooth_server.hpp"

extern SSerial pendant;
extern int btt;


char obuf[1024] = { 0 };
bool send_pendant_data = false;
bool send_l_load_data  = false;
bool send_r_load_data  = false;
bool send_dfive_data   = false;

void send_pendant_stream() 
{
	if (pendant.new_full_extraction) {
		//printf("new_full_extraction=%d\n", pendant.new_full_extraction);	
		char stream_prefix[]  = "STREAM:PENDANT:";
		int prefix_length = strlen(stream_prefix);	
		int bytes_written = write(client, stream_prefix, prefix_length );

		int len = pendant.rx_line_bytes;
		pendant.rx_line[len]   = '\r';
		pendant.rx_line[len+1] = '\n';
		pendant.rx_line[len+2] = 0;
		bytes_written = write(client, pendant.rx_line, len+3 );
		pendant.acknowledge_rxd();
		printf("%s%s", stream_prefix, pendant.rx_line );
	}
}

void send_l_load_stream() {
	char stream_prefix[]  = "STREAM:L LOAD:";
	int prefix_length = strlen(stream_prefix);	
	int bytes_written = write(client, stream_prefix, prefix_length );

	bytes_written = write(client, pendant.rx_buffer, pendant.rx_bytes );
}

void send_r_load_stream() {
	char stream_prefix[]  = "STREAM:R LOAD:";
	int prefix_length = strlen(stream_prefix);	
	int bytes_written = write(client, stream_prefix, prefix_length );

	bytes_written = write(client, pendant.rx_buffer, pendant.rx_bytes );
}

void send_dfive_stream() {
	char stream_prefix[]  = "STREAM:DFIVE:";
	int prefix_length = strlen(stream_prefix);	
	int bytes_written = write(client, stream_prefix, prefix_length );

	bytes_written = write(client, pendant.rx_buffer, pendant.rx_bytes );
}


void send_streams() 
{
	if (btt!=0)	return;

	if (send_pendant_data)		send_pendant_stream();
	if (send_l_load_data)		send_l_load_stream();
	if (send_r_load_data)		send_r_load_stream();
	if (send_dfive_data)		send_dfive_stream();	
}
