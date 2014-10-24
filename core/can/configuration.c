/*********************************************************************
Product of Beyond Kinetics, Inc

--------------------------------------------
DATE 	:  9/23/2013
AUTHOR	:  Stephen Tenniswood
********************************************************************/
#include <string.h>
#include "bk_system_defs.h"
#include "can_id_list.h"
#include "can_eid.h"
#include "CAN_base.h"
//#include "pgmspace.h"
//#include <inttypes.h>
//#include "pin_definitions.h"
//#include "can.h"
//#include "can_instance.h"
//#include "calibrations.h"
//#include "eeprom.h"
#include "can_board_msg.h"
#include "configuration.h"


/**************** TYPE DEFS *************************************/

/**************** VARIABLES *************************************/
byte config_byte_1 	= 0x00;			// the actual use of the bits herein shall be defined 
byte config_byte_2 	= 0x00;			// by the application through use of the isConfigured() 
byte config_byte_3 	= 0x00;			// macro.
byte config_byte_4 	= 0x00;


/**********************************************
save_configuration_eeprom()
Return	:	The module configuration bytes
Also want to save:  Current thresholds.
Report DTCs (overtemperature, over voltage)
***********************************************/
void can_parse_configure_request( struct sCAN* mMsg )
{	
	byte mask;
	byte new_config;
	byte dlc = mMsg->header.DLC;
	int i;
	for (i=0; i<dlc; i+=2) 
	{
		 mask        =  mMsg->data[i];		// mask (which bits are changing)
		 new_config  =  mMsg->data[i+1];	// new values 
		 switch (i)		// alternatively could make config_bytes[4] an array.
		{
		case 0 : config_byte_1 |= (mask & new_config);
		 	config_byte_1 &= ~(mask & (~new_config));
			break;
		case 2 : config_byte_2 |= (mask & new_config);
		 	config_byte_2 &= ~(mask & (~new_config));
			break;
		case 4 : config_byte_3 |= (mask & new_config);
		 	config_byte_3 &= ~(mask & (~new_config));
			break;
		case 6 : config_byte_4 |= (mask & new_config);
		 	config_byte_4 &= ~(mask & (~new_config));
			break;
		default: break;
		}			 
	}
	save_cal(EEPROM_BASEADDRESS);		// save everything anyway.
}

byte isConfigured (byte Test)	{  return ((config_byte_1 & Test)>0); };
byte isConfigured2(byte Test)	{  return ((config_byte_2 & Test)>0); };
byte isConfigured3(byte Test)	{  return ((config_byte_3 & Test)>0); };
byte isConfigured4(byte Test)	{  return ((config_byte_4 & Test)>0); };

byte getConfiguredCANBaudRate()			{  return ((config_byte_4 & 0xF0)>0); };
void ConfigureCANBaudRate(byte BaudRate)	{  config_byte_4 = (BaudRate & 0xF0); };

byte* save_cal(byte* addr ) { return 0; };		// Prototyped here as abstract headers.
byte* read_cal(byte* addr ) { return 0; };		// each board must implement these.


// for Raspberry Pi, these don't do anything!
byte eeprom_write_byte( byte* addr, byte d){ return (byte)0;}  //addr; }
byte eeprom_read_byte ( byte* addr )		{ return (byte)0;} //addr; }


/**********************************************
save_configuration_eeprom()
Return	:	The module configuration bytes
Also want to save:  Current thresholds.
Report DTCs (overtemperature, over voltage)
***********************************************/
byte* save_configuration_eeprom(byte* addr)
{
	eeprom_write_byte( addr++, config_byte_1 );
	eeprom_write_byte( addr++, config_byte_2 );	
	return addr;
}

/**********************************************************
read_configuration_eeprom(byte mStartAddress)
Reads from EEPROM, and stores into the RAM
sEndPoint structures defined in pot.h
***********************************************************/
byte* read_configuration_eeprom(byte* addr)
{
	config_byte_1 = eeprom_read_byte( addr++ );
	config_byte_2 = eeprom_read_byte( addr++ );
	return addr;
}


