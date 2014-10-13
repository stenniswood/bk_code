#ifndef _CLIENT_H_
#define _CLIENT_H_

#ifdef  __cplusplus
extern "C" {
#endif


extern BOOL TransportCAN;
extern BOOL WifiConnected;


int 		isTransporting_CAN();

// Rebroadcast a CAN message.
int 		Cmd_client_CAN_Start( );
int 		Cmd_client_send_CAN( struct sCAN* Msg );

// COMMON CONNECTIONS
int 		connect_to_robot(char *ip_address );
int 		client_send_buffer();
int 		wait_for_ack();
int 		disconnect_from_robot();



#ifdef  __cplusplus
}
#endif

#endif
