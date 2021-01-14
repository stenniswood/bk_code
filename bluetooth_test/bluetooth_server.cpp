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

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <bluetooth/rfcomm.h>
#include <pthread.h>


char buf[1024] = { 0 };
int s, client, bytes_read;


void bluetooth_nearby_devices() 
{
	inquiry_info *ii = NULL;
	int max_rsp, num_rsp;
	int dev_id, sock, len, flags;
	int i;
	char addr[19] = { 0 };
	char name[248] = { 0 };
	dev_id = hci_get_route(NULL);
	sock   = hci_open_dev( dev_id );

	if (dev_id < 0 || sock < 0) {
		perror("opening socket");
		exit(1);
	}
	
	len = 8;
	max_rsp = 255;
	flags = IREQ_CACHE_FLUSH;
	ii = (inquiry_info*)malloc(max_rsp * sizeof(inquiry_info));
	
	num_rsp = hci_inquiry(dev_id, len, max_rsp, NULL, &ii, flags);
	if( num_rsp < 0 ) perror("hci_inquiry");
	for (i = 0; i < num_rsp; i++) {
		ba2str(&(ii+i)->bdaddr, addr);
		memset(name, 0, sizeof(name));
		int result = hci_read_remote_name(sock, &(ii+i)->bdaddr, sizeof(name), name, 0);
		if (result < 0)	
			strcpy(name, "[unknown]");
		printf("%s %s\n", addr, name);
	}
	free( ii );
	close( sock );
}		
		
void bluetooth_server()  
{

	struct sockaddr_rc loc_addr = { 0 }, rem_addr = { 0 };
	int opt = sizeof(rem_addr);

	// allocate socket
	s = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);

	// bind socket to port 1 of the first available
	// local bluetooth adapter
	bdaddr_t bdaddr_any = {0, 0, 0, 0, 0, 0};
	bacpy(&bdaddr_any, &(loc_addr.rc_bdaddr)  );
	
	loc_addr.rc_family = AF_BLUETOOTH;
	loc_addr.rc_bdaddr = bdaddr_any;
	loc_addr.rc_channel = (uint8_t) 1;
	bind(s, (struct sockaddr *)&loc_addr, sizeof(loc_addr));

	// put socket into listening mode
	listen(s, 1);

	// accept one connection
	client = accept(s, (struct sockaddr *)&rem_addr, (socklen_t*)&opt);
	
	ba2str( &rem_addr.rc_bdaddr, buf );
	fprintf(stderr, "accepted connection from %s\n", buf);
	memset(buf, 0, sizeof(buf));
}



int read_socket() 
{
	memset(buf, 0, sizeof(buf) );
	
	//int count;
	//ioctl(client, FIONREAD, &count);
	//if (count)
	{
		// read data from the client
		bytes_read = read(client, buf, sizeof(buf));
		if( bytes_read > 0 ) {
			char* ptr   = strchr(buf, '\r');
			if (ptr) {
				*ptr = 0;
			}
			printf("received [%s]\n", buf);
		}
	}
	return bytes_read;
}


void close_socket() 
{
	// close connection
	close(client);
	close(s);
}


