#ifndef _CONFIGURATION_H_
#define _CONFIGURATION_H_

/************* CONFIGURATION *****************/
extern byte config_byte_1 ;
extern byte config_byte_2 ;
extern byte config_byte_3 ;
extern byte config_byte_4 ;

extern byte isConfigured (byte Test);
extern byte isConfigured2(byte Test);
extern byte isConfigured3(byte Test);
extern byte isConfigured4(byte Test);


/**** CONFIG BYTE 4 - BIT DEFS ****/
// There are 16 configurable baudrates for CAN:
#define CAN_BAUD_100K 0x10
#define CAN_BAUD_125K 0x20
#define CAN_BAUD_200K 0x30
#define CAN_BAUD_250K 0x40
#define CAN_BAUD_500K 0x50
#define CAN_BAUD_1M   0x60
extern byte getConfiguredCANBaudRate()			;
extern void ConfigureCANBaudRate(byte BaudRate)	;


void can_parse_configure_request( struct sCAN* mMsg );
byte* save_configuration_eeprom ( byte* addr );
byte* read_configuration_eeprom ( byte* addr );

// The following 2 functions are abstract.  Each Module must define them.
// The should call all sub EEPROM saving functions.  The start
#define MAX_EEPROM 			512
#define EEPROM_BASEADDRESS	(byte*)1			// myinstance is at 0 and goes on its own.

//= EEPROM_BASEADDRESS
byte* save_cal(byte* addr ) ;		// Prototyped here as abstract headers.
byte* read_cal(byte* addr ) ;		// each board must implement these.


#endif
