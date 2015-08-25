#ifndef _CLIENT_H_
#define _CLIENT_H_

#ifdef  __cplusplus
extern "C" {
#endif

extern BOOL CAN_ListeningOn;	// if true we will be receiving CAN Traffic.
extern BOOL CAN_SendingOn;		// if true we will be sending   CAN Traffic.
extern BOOL WifiConnected;

int		are_importing_CAN();
int		are_exporting_CAN();

int 		send_file_transmit_request();

// Rebroadcast a CAN message.
int 		Cmd_client_CAN_Start( );
int			Cmd_client_CAN_listen( );
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
