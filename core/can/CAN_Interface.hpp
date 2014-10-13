#ifndef CANInterface_h
#define CANInterface_h

#ifdef  __cplusplus
extern "C" {
#endif

// include types & constants of Wiring core API
#include "bk_system_defs.h"
#include "can_eid.h"


#define LED2 		8
#define LED3 		7

#define CANSPEED_125 	7		// CAN speed at 125 kbps
#define CANSPEED_250  	3		// CAN speed at 250 kbps
#define CANSPEED_500	1		// CAN speed at 500 kbps
#define	CANSPEED_95		6		// CAN speed at 95.238 kbps
#define	CANSPEED_33		5		// CAN speed at 95.238 kbps

// Flag from the CAN_isr() to the main loop.
extern uint8_t ReadyToSendAnother;

// CAN MESSAGE STRUCTURE DEFINITION:
#include "CAN_base.h"

// needed in CAN_protocol.c for CAN over wifi.
BOOL msg_callbacks			( struct sCAN* mMsg, byte mCANBufferNumber );
void set_system_rx_callback	( BOOL (*mCallback)(struct sCAN*) );
void set_model_rx_callback 	( BOOL (*mCallback)(struct sCAN*) );

/* not implemented!!  */
void can_remove_instance_from_filter( byte mMOb_Number );
void can_add_id_to_filter 	( byte mMOb_Number, word  mID_mask, word  mID_tag );
void can_setup_receive_mob	( byte mMobNumber,  short mMaskLow, short mMaskHigh, byte mDLC );
void setMask( uint8_t mByte, long int mMask );
/* end not implemented */

//Protected Functions:  
//void print_interrupt_type(byte status);
//void print_register_name( uint8_t mAddress )
//float print_baud_prescaler( int mCNF1 )
//void print_bit_timing( byte mCNF2, byte mCNF3, float tq )
//void print_speed( int mSpeed )

BOOL 	CAN_init(int mspeed, uint8_t mLoopbackMode );

void 	CAN_isr();
void 	enable_interrupt		( byte mMask );
void 	disable_interrupt		( byte mMask );
void 	enable_tx_ints 		(			 );
void 	enable_rx_ints 		(			 );
void 	disable_tx_ints		(			 );
void 	disable_rx_ints		(			 );

void 	enable_err_ints		(			 );
void 	enable_wakeup_ints	(			 );
void 	disable_err_ints	(			 );
void 	disable_wakeup_ints	(			 );


// CAN Controller Communications:
void 	Reset();
byte 	transfer			( uint8_t data    );
void    write_register		( uint8_t address, uint8_t data );
uint8_t read_register		( uint8_t address );
void 	MultiRegisterWrite	( uint8_t mAddress1, uint8_t* mValueArray, int mArrayLength );
void 	MultiRegisterRead	( uint8_t mAddress1, uint8_t* mValueArray, int mArrayLength );

byte	get_CNF_reg_values	( byte* mRegValues, int speed);
byte 	set_baud_rate		( int speed 		);
BOOL 	verify_baud_rate	( int speed			);


uint8_t 	check_message(void);
void    	bit_modify(uint8_t address, uint8_t mask, uint8_t data);
uint8_t 	read_status();
uint8_t 	read_rx_status();

uint8_t 	check_free_buffer(void);
uint8_t 	get_message(struct sCAN *message, byte mBuffNumber);

uint8_t 	Determine_tx_address(byte tx_buffer);	// returns first available : [1,2,4]
uint8_t 	Determine_tx_buffer();					// finds an empty buffer for sending.  
uint8_t		Request_To_Send( uint8_t mTxBufferBit );

void 		transfer_message(uint8_t mabc_address, struct sCAN *message);
uint8_t 	send_message(struct sCAN* message);
uint8_t 	can_send_msg_no_wait(byte unused, struct sCAN* message);

uint8_t 	print_message( struct sCAN* msg );


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
 //=========== HIGH FREQUENCY SAMPLING (low TQ) ======================
// ****** The smallest TQ was chosen in each configuration.  ********
#define BT1_1Mbps 0x00
#define BT2_1Mbps 0x0C
#define BT3_1Mbps 0x36

// Updated for 2515:
#define BT1_500Kbps 0x01			/* CNF3 */
#define BT2_500Kbps 0xCA			/* CNF2 */
#define BT3_500Kbps 0x01			/* CNF1 */

// Updated for 2515:
#define BT1_250Kbps 0x01			/* CNF3 */
#define BT2_250Kbps 0xCA			/* CNF2 */
#define BT3_250Kbps 0x03			/* CNF1 */

#define BT1_200Kbps 0x08
#define BT2_200Kbps 0x0C
#define BT3_200Kbps 0x37

// Updated for 2515:
#define BT1_125Kbps 0x01
#define BT2_125Kbps 0xCA
#define BT3_125Kbps 0x07


#define BT1_95K2bps 0x12
#define BT2_95K2bps 0x0C
#define BT3_95K2bps 0x37

#define BT1_33K3bps 0x12
#define BT2_33K3bps 0x0C
#define BT3_33K3bps 0x37


//=========== ALTERNATE LOWER FREQUENCY SAMPLING ======================
#define BT1_1Mbps_A 0x02
#define BT2_1Mbps_A 0x04
#define BT3_1Mbps_A 0x13

#define BT1_500Kbps_A 0x06
#define BT2_500Kbps_A 0x04
#define BT3_500Kbps_A 0x13

/*	Tq = 0.500us 
	8 = 3 + 2 + 2 + 1
*/
#define BT1_250Kbps_A 0x0E
#define BT2_250Kbps_A 0x04
#define BT3_250Kbps_A 0x13

#define BT1_200Kbps_A 0x12
#define BT2_200Kbps_A 0x04
#define BT3_200Kbps_A 0x13

#define BT1_125Kbps_A 0x1E
#define BT2_125Kbps_A 0x04
#define BT3_125Kbps_A 0x13

#define BT1_100Kbps_A 0x26
#define BT2_100Kbps_A 0x04
#define BT3_100Kbps_A 0x13


#ifdef  __cplusplus
}
#endif

#endif

