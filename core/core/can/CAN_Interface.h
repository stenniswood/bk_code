#ifndef CANInterface_h
#define CANInterface_h

#ifdef  __cplusplus
extern "C" {
#endif



/*
  Test.h - Test library for Wiring - description
  Copyright (c) 2006 John Doe.  All right reserved.
*/
// include types & constants of Wiring core API
//#include "WConstants.h"
//#include "mcp2515_defs.h"
#include "bk_system_defs.h"
#include "can_eid.h"

//#include "global.h"
//#include <adrenaline_bsp.h>
//#include <inttypes.h>

#define LED2 		8
#define LED3 		7
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
#define THROTTLE	    	0x11

#define PID_REQUEST         0x7DF
#define PID_REPLY	    	0x7E8

//#ifndef	MCP2515_H
// CAN MESSAGE STRUCTURE DEFINITION:
struct sCAN
{
  tID 	id;					// CAN uses 29 bits
  struct {
  	byte  rtr    : 1;		// Remote Transmission Request (0=DataFrame; 1=RemoteFrame)
							// a node can request a msg from another node.
	byte DLC : 4;			// 4 bits => [0..15]
  } header;
  byte data[8];				// 
  word time_stamp;          // 
};

extern struct sCAN 	msg1;
extern struct sCAN 	msg2;
extern struct sCAN 	message;          // One buffer filled by various routines
extern struct sCAN 	rxmessage;        // One buffer filled by various routines

void set_system_rx_callback( BOOL (*mCallback)(struct sCAN*) );
void set_model_rx_callback ( BOOL (*mCallback)(struct sCAN*) );
//void msg_callbacks( struct sCAN* mMsg ); protected function


void can_remove_instance_from_filter( byte mMOb_Number );
void can_add_id_to_filter ( byte mMOb_Number, word  mID_mask, word  mID_tag );
void can_setup_receive_mob( byte mMobNumber,  short mMaskLow, short mMaskHigh, byte mDLC );
void can_add_id_to_filter ( byte mMOb_Number, word  mID_mask, word  mID_tag );

void enable_interrupt( byte mMask );
void disable_interrupt( byte mMask );
void enable_tx_ints ();
void enable_rx_ints ();
void disable_tx_ints();
void disable_rx_ints();


	void 	CAN_isr();
    void 	setMask( uint8_t mByte, long int mMask );
    BOOL 	Init(int mspeed, uint8_t mLoopbackMode );
    void 	Reset();
    byte 	transfer( uint8_t data );
    void 	MultiRegisterWrite(uint8_t mAddress1, uint8_t* mValueArray, int mArrayLength );
    void 	MultiRegisterRead( uint8_t mAddress1, uint8_t* mValueArray, int mArrayLength );
    byte 	SetChipSpeed( int speed );
    void 	CAN_Int();

    // CAN Controller Communications:
    void    	write_register( uint8_t address, uint8_t data );
    uint8_t 	read_register(uint8_t address);

    uint8_t 	check_message(void);
    void    	bit_modify(uint8_t address, uint8_t mask, uint8_t data);
    uint8_t 	read_status();
    uint8_t 	read_rx_status();

    uint8_t 	check_free_buffer(void);
    uint8_t 	get_message(struct sCAN *message, byte mBuffNumber);

	uint8_t 	can_send_msg_no_wait(byte unused, struct sCAN* message);
    uint8_t 	send_message(struct sCAN* message);
    uint8_t 	Determine_tx_address(byte tx_buffer);	// returns first available : [1,2,4]
    uint8_t 	Determine_tx_buffer();					// finds an empty buffer for sending.  
    void 		transfer_message(uint8_t mabc_address, struct sCAN *message);
    uint8_t		Request_To_Send( uint8_t mTxBufferBit );
    uint8_t 	print_message  ( struct sCAN* msg );

  // library-accessible "private" interface
/***** ATMEGA16m1 COMPATIBLE INTERFACE **************/
/*void set_baud( byte mSpeed, byte mPropSegs, byte Phase1Segs, byte Phase2Segs  );
void can_init_mobs		 	(				);
void can_init			 	( byte mSpeed  );
void can_setup_receive_mob	( byte mMobNumber, short mMaskLow, short mMaskHigh, byte mDLC );
void can_retrieve_id		( struct sCAN* mMsg );
byte can_retrieve_data	 	( struct sCAN* mMsg	);
word can_get_timestamp	 	(									  );
void can_send_msg		 	( byte mMOb_Number, struct sCAN* mMsg );
void can_send_msg_no_wait	( byte mMOb_Number, struct sCAN* mMsg );
void can_get_msg		 	( struct sCAN* mMsg 				  );

void can_set_mask			( byte mMOb_Number, short mMaskLow, short mMaskHigh );
void can_set_tag			( byte mMOb_Number, short mTagLow, short mTagHigh );
void can_set_id				( byte mMOb_Number, tID id );

void can_add_id_to_filter	( byte mMOb_Number, word mID_mask, word mID_tag );
void can_remove_id_from_filter(byte mMOb_Number );

void can_add_instance_to_filter		( byte mMOb_Number, word mInstance );
void can_remove_instance_from_filter( byte mMOb_Number 			);
*/

#ifdef  __cplusplus
}
#endif

#endif

