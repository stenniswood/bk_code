#ifndef Packer_h
#define Packer_h


/******************************************************* 
 SYSTEM LED COMMAND STRUCTURE 	
*******************************************************/
#define SYSTEM_LED_MODE_DEVICE		0x00
#define SYSTEM_LED_MODE_PATTERN		0x01
#define SYSTEM_LED_MODE_STROBE		0x02
#define SYSTEM_LED_MODE_MYINSTANCE	0x03
#define SYSTEM_LED_MODE_CONT_PATTERN 0x04

#define ASK_PRESENCE		0
#define ASK_REVISION		1
#define ASK_SERIAL_NUMBER	2
#define ASK_DESCRIPTION		3
void pack_board_presence_request( struct sCAN* mMsg, byte mResponseTypeRequested );

void pack_configure			(struct sCAN* mMsg, byte mDestInstance, byte mIndex, byte mMask, byte mValue );
void pack_system_led		(struct sCAN* mMsg, byte mDestInstance, byte mMode, byte mPattern );
void pack_instance_restart 	(struct sCAN* mMsg, byte mDestInstance );
void pack_instance_reassign	(struct sCAN* mMsg, byte mDestInstance, byte mNewInstance );
void pack_system_shutdown  	(struct sCAN* mMsg 					   );


#endif
