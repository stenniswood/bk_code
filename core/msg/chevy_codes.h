#ifndef _CHEVY_CODES_
#define _CHEVY_CODES_

#ifdef  __cplusplus
extern "C" {
#endif

/*  Misc, from 2515 sample code.
#define ENGINE_COOLANT_TEMP 0x05
#define ENGINE_RPM          0x0C
#define VEHICLE_SPEED       0x0D
#define MAF_SENSOR          0x10
#define O2_VOLTAGE          0x14
#define THROTTLE	    	0x11

#define PID_REQUEST         0x7DF
#define PID_REPLY	    	0x7E8
*/

float parse_0C9( struct sCAN* mMsg );
float parse_0F1( struct sCAN* mMsg );
void  parse_135( struct sCAN* mMsg );
float parse_1A1( struct sCAN* mMsg );
float parse_1C3( struct sCAN* mMsg );
void  parse_1EF( struct sCAN* mMsg );
void  parse_1F5( struct sCAN* mMsg );
void  parse_206( struct sCAN* mMsg );
void  parse_32A( struct sCAN* mMsg );
void  parse_3E9( struct sCAN* mMsg );
int   is_powertrain_msg( struct sCAN* mMsg );
BOOL  parse_chevy_id   ( struct sCAN* mMsg );
void  code_description ( tID mID, char* mDescription );

#ifdef  __cplusplus
}
#endif

#endif
