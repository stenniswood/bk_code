#ifndef _CLIENT_H_
#define _CLIENT_H_

#ifdef  __cplusplus
extern "C" {
#endif


extern BOOL TransportCAN;
extern BOOL WifiConnected;

int 		isTransporting_CAN();

// Rebroadcast a CAN message.
int 		send_file_transmit_request();
int 		Cmd_client_CAN_Start( );			// need to recode for nlp!
int 		Cmd_client_send_CAN( struct sCAN* Msg ); // need to recode for nlp!

// COMMON CONNECTIONS
int 		connect_to_robot(char *ip_address );
int 		client_send_buffer();
int 		wait_for_ack();
int 		disconnect_from_robot();



#ifdef  __cplusplus
}
#endif

#endif
