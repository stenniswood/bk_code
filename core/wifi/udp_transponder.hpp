
/*#include <sys/types.h>    // Needed for sockets stuff
#include <netinet/in.h>   // Needed for sockets stuff
#include <sys/socket.h>   // Needed for sockets stuff
#include <arpa/inet.h>    // Needed for sockets stuff
#include <fcntl.h>        // Needed for sockets stuff
#include <netdb.h>        // Needed for sockets stuff
*/

//#include <list>
//#include <string>

using namespace std;

#define MACHINE_TYPE "Raspberry Pi"


/*struct stBK_Header 
{
	char ip_address[40];	
	char hostname[40];
	char machine_type[40];
	struct in_addr addr;
};*/


void* udp_transponder_function(void*);
void* udp_receive_function    (void*);

// Client List Functions:

int		already_added		( struct in_addr mIP 	 );
void 	handle_client_list	( struct in_addr* client_ip_addr, char in_buf[] );
void	print_beacons		(						 );
void 	update_client_list	(						 );

int 	convert_to_string_array( char* mText, char mDelim='\n' );

//extern list<struct stBK_Header> beacon_ip_list; 


