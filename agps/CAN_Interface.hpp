/*
  Test.h - Test library for Wiring - description
  Copyright (c) 2006 John Doe.  All right reserved.
*/
 
// ensure this library description is only included once
#ifndef CANInterface_h
#define CANInterface_h

// include types & constants of Wiring core API
//#include "WConstants.h"
#include "mcp2515_defs.h"

//#include "global.h"
//#include <adrenaline_bsp.h>
//#include <inttypes.h>

#define LED2 			8
#define LED3 			7
#define	MCP2515_INT 	2

#define CANSPEED_125 	7		// CAN speed at 125 kbps
#define CANSPEED_250  	3		// CAN speed at 250 kbps
#define CANSPEED_500	1		// CAN speed at 500 kbps
#define	CANSPEED_95		6		// CAN speed at 95.238 kbps

#define ENGINE_COOLANT_TEMP 0x05
#define ENGINE_RPM          0x0C
#define VEHICLE_SPEED       0x0D
#define MAF_SENSOR          0x10
#define O2_VOLTAGE          0x14
#define THROTTLE			0x11

#define PID_REQUEST         0x7DF
#define PID_REPLY			0x7E8

//#define CAN_SELECT		10	defined in adrenaline_bsp.h
#define CAN_CS				CAN_SELECT

#ifndef	MCP2515_H
typedef struct
{
	uint16_t id;				// CAN uses 11 bits
	struct {
		int8_t  rtr    : 1;		// Remote Transmission Request (0=DataFrame; 1=RemoteFrame)
								// a node can request a msg from another node.
		uint8_t length : 4;		// [0..15]
	} header;
	uint8_t data[8];			// 
} tCAN; 
#endif

// library interface description
class CANInterface
{
  // user-accessible "public" interface
  public:
    CANInterface();
    bool Init(int);
	void Reset();
    void MultiRegisterWrite(uint8_t mAddress1, uint8_t* mValueArray, int mArrayLength );
    void SetChipSpeed( int speed );
    
    void CAN_Int();	//

	// CAN Controller Communications:
    void    write_register( uint8_t address, uint8_t data );
    uint8_t read_register(uint8_t address);

    uint8_t check_message(void);
    void    bit_modify(uint8_t address, uint8_t mask, uint8_t data);
    uint8_t read_status(uint8_t type);
    uint8_t check_free_buffer(void);
    uint8_t get_message(tCAN *message, unsigned long *timestamp);
    uint8_t send_message(tCAN *message);

  // library-accessible "private" interface
  private:
    int value;
};

extern CANInterface CAN;

#endif

