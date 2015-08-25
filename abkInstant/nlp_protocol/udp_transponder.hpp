


#include <list>
#include <string>
using namespace std;

#define MACHINE_TYPE "Raspberry Pi"


void* udp_transponder_function(void*);
void* udp_receive_function    (void*);

// Client List Functions:
int		already_added		( struct in_addr mIP 	 );
void 	handle_client_list	( struct in_addr* client_ip_addr, char in_buf[] );
void	print_beacons		(						 );
void 	update_client_list	(						 );

extern list<struct in_addr> beacon_ip_list; 


