// include core Wiring API
// #include "WProgram.h"
// include this library's description file
//#include "SPI.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <wiringPiSPI.h>
#include <wiringPi.h>
#include <inttypes.h>
#include "mcp2515.h"
#include "mcp2515_defs.h"
#include "CAN_Interface.hpp"
#include <wiringPiSPI.h>
#include <wiringPi.h>
#include "pican_defines.h"
#include "can_txbuff.hpp"
#include "can_rxbuff.hpp"
#include "can_eid.h"
#include "can_id_list.h"
#ifdef WIFI_SOCKET
#include "client.h"
#endif


uint8_t ReadyToSendAnother = FALSE; 

//int value;
uint8_t channel=0;	// SPI channel on Pi.  ie. CE0 not CE1
// Wired to CE0 not CE1!  See Pican Schematics.

struct sCAN 	msg1;
struct sCAN 	msg2;
struct sCAN 	message;          // One buffer filled by various routines
struct sCAN 	rxmessage;        // One buffer filled by various routines
unsigned long 	timestamp=0;	  // receiving messages get this

#define FLAG_TXB0_INT 0x06
#define FLAG_TXB1_INT 0x08
#define FLAG_TXB2_INT 0x0A
#define FLAG_RXB0_INT 0x0C
#define FLAG_RXB1_INT 0x0E

BOOL (*rx_system_call_back)(struct sCAN*) = NULL;	// Call back function for system msg processing 
BOOL (*rx_model_call_back )(struct sCAN*) = NULL;	// Call back function based on adrenaline board type

/*****************************************************************************
Name :  can_add_id_to_filter()  
			Slightly different from can_set_id() - It only adds the 16bit portion
			of the id to the filter, AND it also sets the mask for this!
			Use when setting up the MOB for specific receive Msgs.
			
Input:  mMOb_Number	- MOb to add the filter to
		mID_mask	- Mask of which bits in the ID portion of the tID are important
		mID_tag		- Tag (value) of the ID bits which must be matched.
* This is done independent of the instance filtering.  
*****************************************************************************/
void can_add_id_to_filter( byte mMOb_Number, word mID_mask, word mID_tag )
{
	tID id   = create_CAN_eid( mID_tag,  0 );
	tID mask = create_CAN_eid( mID_mask, 0 );

//  Ignore Block & Instance
//	byte idt1 = (CANIDT1 & 0xF8) | ((id.group.id & 0xE000)>>13);
//	byte idt2 = ((id.group.id & 0x1FE0)>>5);
//	byte idt3 = ((id.group.id & 0x001F)<<3) | (CANIDT3 & 0x07);
	// Ignore Block & Instance
//	byte idm1 = (CANIDM1 & 0xF8) | ((mask.group.id & 0xE000)>>13);
//	byte idm2 = ((mask.group.id & 0x1FE0)>>5);
//	byte idm3 = ((mask.group.id & 0x001F)<<3) | (CANIDM3 & 0x07);
}

void can_remove_instance_from_filter( byte mMOb_Number )
{
}

void can_setup_receive_mob( byte mMobNumber, short mMaskLow, short mMaskHigh, byte mDLC )
{
	int8_t savecanpage;
}
void setMask( uint8_t mByte, long int mMask )
{
	if (mByte)
	{
		// SET MASKS TO ZEROS :
		write_register(RXM0SIDH, 0x00);
		write_register(RXM0SIDL, 0x00);
		write_register(RXM0EID8, 0x00);
		write_register(RXM0EID0, 0x00);
	} else {
		// SET MASKS TO ZEROS :
		write_register(RXM1SIDH, 0x00);
		write_register(RXM1SIDL, 0x00);
		write_register(RXM1EID8, 0x00);
		write_register(RXM1EID0, 0x00);
	}
}

/**********************************************************************//**
*  Purpose	  :  Initialize CAN - Pin directions, SPI init, Chip Reset & Init,
				 Verify speed, setup 2515 pins & filters.

*  @param speed - Speed of the CAN network (bits/second) [CANSPEED_95,CANSPEED_125,CANSPEED_250,CANSPEED_500]
*  @return	  true = success
*************************************************************************/
BOOL CAN_init(int speed, uint8_t mLoopbackMode)
{
    // SPI PINS:
    int spi_speed = 1000000;
    if( wiringPiSPISetup(channel, spi_speed)==-1)
    {
		printf("Could not initialise SPI\n");
		printf("Make sure you run this as root: sudo ./aconf\n");
		return FALSE;
    }
    else printf("SPI Port opened at %d bps\n", spi_speed );

	Reset();
	uint8_t result = set_baud_rate( speed );
	if (result == 0)
	{
		printf("CAN_INIT error:  Cannot set registers!\n" );
		exit(1);
	}

	// deactivate the RXnBF Pins (High Impedance State)
	write_register(BFPCTRL, 0);

	// set TXnRTS as Ready-to-Send control:
	write_register( TXRTSCTRL,  0x07 );		// was 0x07

	// turn off filters => receive any message
	/* Note BUKT is very important.  It lets us use RXB1 interrupts!
		if it's not there, we never get a RXB1 */
	write_register(RXB0CTRL, (1<<RXM1)|(1<<RXM0)|(1<<BUKT) );
	write_register(RXB1CTRL, (1<<RXM1)|(1<<RXM0)		   );

	setMask( 0, 0x00000000 );			
	setMask( 1, 0x00000000 );			

	// reset device to normal mode
	if (mLoopbackMode)
		write_register(CANCTRL, 0x40); 	// Loopback mode
	else
		write_register(CANCTRL, 0x00); 	// reset device to normal mode

	write_register( TXB0CTRL, 0x01 );	// set priorities
	write_register( TXB1CTRL, 0x02 );	
	write_register( TXB2CTRL, 0x03 );	

	enable_tx_ints();					
	enable_rx_ints();					
	return TRUE;						
}


/*****************************************************************************
Name		: set_system_rx_callback()
INPUT 		: mCallback - a function ptr to receive incoming CAN msgs
OUTPUT		: none
RETURN		: none
DESCRIPTION:  CAN ISR will call the function sent here on a Receive interrupt
			  which has a system message ID
******************************************************************************/
void set_system_rx_callback( BOOL (*mCallback)(struct sCAN*) )
{
	printf("set_system_rx_callback(%d)\n", mCallback);
	rx_system_call_back = mCallback;
}

/*****************************************************************************
Name		: set_model_rx_callback()
INPUT 		: mCallback - a function ptr to receive incoming CAN msgs
OUTPUT		: none
RETURN		: none
DESCRIPTION:  CAN ISR will call the function sent here on a Receive interrupt
			  which is not handled by the can_board_msg.c system message handler
******************************************************************************/
void set_model_rx_callback( BOOL (*mCallback)(struct sCAN*) )
{
	printf("set_model_rx_callback(%d)\n", mCallback);
	rx_model_call_back = mCallback;
}

/*****************************************************************************
Name		: set_model_rx_callback()
INPUT 		: mCallback - a function ptr to receive incoming CAN msgs
OUTPUT		: none
RETURN		: TRUE - message was handled.
DESCRIPTION:  CAN ISR will call the function sent here on a Receive interrupt
			  disperses to one of several callback CAN handlers.
******************************************************************************/
BOOL msg_callbacks( struct sCAN* mMsg, byte mCANBufferNumber )
{
	BOOL retval = FALSE;
	// pass mCANBufferNumber on to the caller if needed.
	// not done yet because no masks or filters are used.
	if (isSystemMsg( mMsg->id ))  {			// System Message Handler
		if (rx_system_call_back != NULL)
			retval = rx_system_call_back(mMsg);
	}
	if (rx_model_call_back != NULL)		// Board based Handler
		retval = rx_model_call_back(mMsg);

	return retval;
}


void print_interrupt_type(byte status)
{
	switch (status&0x07)
	{
	case 0 : 	printf("no interrupt");
				break;
	case 1 : 	printf("error interrupt");
				break;
	case 2 : 	printf("wakeup interrupt");
				break;
	case 3 : 	printf("txb0 interrupt");
				break;
	case 4 : 	printf("txb1 interrupt");
				break;
	case 5 : 	printf("txb2 interrupt");
				break;
	case 6 : 	printf("rxb0 interrupt");
				break;
	case 7 : 	printf("rxb1 interrupt");
				break;
	default:	break;				
	}
}


/* Because this is a shared interrupt.  TX & RX have to be handled 
   in the same function call.  If we clear one of them (ie RX) 
   the INT line will not go high again until all interrupts are cleared.
   And if we take too long and another Rx interrupt occurs, we will not
   get a new falling edge to trigger another interrupt.  The pin will stay low.
   
*/
void CAN_isr()
{
	//printf("<<---CAN Received Interrupt--- \n");
	byte status = read_register( CANSTAT );
	byte intf   = read_register( CANINTF );
	byte inte   = read_register( CANINTE );
	byte rxbuff = 0;
	byte rxFlagMask = 0x01;
	byte Rx = FALSE;
	//printf("ISR %x:%x:%x\n", status, intf, inte);
  do {
	if ((intf & 0x01) & (inte&0x01))
	{ rxbuff = 0;  rxFlagMask = 0x01; Rx=TRUE; }
	if ((intf & 0x02) & (inte&0x02))
	{ rxbuff = 1;  rxFlagMask = 0x02; Rx=TRUE; }

	if (Rx)
	{
		// clear interrupt flag - Done Automatically by using SPI_READ_RX cmd 
		/* Page 64 of 2515 says that only the RXnIF flag is reset.  so it seems
		if a tx flag were set it would come back here. */
		//printf("rx%d:intf=%x;e=%x\n", rxbuff, intf,inte);
		get_message ( &rxmessage, rxbuff 		);
		bit_modify  ( CANINTF, rxFlagMask, 0x00 );
		AddToRxList ( &rxmessage );
		if (msg_callbacks( &rxmessage, rxbuff) == FALSE)
		{
			// printf(" Rx0:%x:%x\n", status, intf);
		}
		// This is a serial transfer of the text.  Slow! May cause the overruns.
		//print_message( &rxmessage );
		
#ifdef WIFI_SOCKET
			if (isTransporting_CAN())
				Cmd_client_send_CAN( &rxmessage );
#endif
	}
	if ((intf & 0x04) & (inte&0x04))
	{
			bit_modify( CANINTF, 0x04, 0x00 );
			digitalWrite( TX0RTS, 1 );
			
			//printf("TXB0 INTF\n");
			ReadyToSendAnother     = TRUE;		// Set a flag and send next in the main loop.
			TransmissionInProgress = FALSE;
	}
	if ((intf & 0x08) & (inte&0x08))
	{
			bit_modify( CANINTF, 0x08, 0x00 );
			digitalWrite( TX1RTS, 1 );
			
			//printf("Cleared TXB1 INTF\n");
			ReadyToSendAnother = TRUE;
			TransmissionInProgress = FALSE;
	}
	if ((intf & 0x10) & (inte&0x10))
	{
			bit_modify( CANINTF, 0x10, 0x00 );	
			digitalWrite( TX2RTS, 1 );
			
			//printf("Cleared TXB2 INTF\n");
			ReadyToSendAnother = TRUE;
			TransmissionInProgress = FALSE;
	}
	
	// Clear any error:  Need to handled better though.
	if ( intf & 0xE0 )
	{
		byte eflg  = read_register( EFLG );
		byte txctrl= read_register( TXB0CTRL );
		byte tec= read_register( TEC );
		byte rec= read_register( REC );
		printf("EF=%x; intf=%x; rec=%x; tec=%x\n", eflg, intf, rec,tec);
		bit_modify( CANINTF, 0xE0, 0x00 );
		write_register( REC, 0 );
		write_register( TEC, 0 );
	}
	intf = read_register( CANINTF );
	//printf("intf=%x\n", intf);
} while( inte & intf);
	
	return ;
	//printf("<<--eoi--\n");
} 

void enable_interrupt( byte mMask )
{
	byte regval = read_register(CANINTE);
	regval |= mMask;
	write_register( CANINTE, regval );	
}
void disable_interrupt( byte mMask )
{
	byte regval = read_register(CANINTE);
	regval &= ~(mMask);
	write_register( CANINTE, regval );	
}
void enable_tx_ints()
{	enable_interrupt( 0x1C);	}
void enable_rx_ints()
{	enable_interrupt(0x03);	}

void disable_tx_ints()
{	disable_interrupt(0x1C);	}
void disable_rx_ints()
{	disable_interrupt(0x03);	}

void enable_err_ints()
{	enable_interrupt(0x20);	}
void enable_wakeup_ints()
{	enable_interrupt(0x40);	}
void disable_err_ints()
{	disable_interrupt(0x20);	}
void disable_wakeup_ints()
{	disable_interrupt(0x40);	}	


void print_register_name( uint8_t mAddress )
{
	switch(mAddress)
	{
	case 0x0C : printf ("BFPCTRL");   break;
	case 0x0D : printf ("TXRTSCTRL"); break;
	case 0x0E : printf ("CANSTAT");   break;
	case 0x0F : printf ("CANCTRL");   break;
	case 0x1C : printf ("TEC");  	  break;
	case 0x1D : printf ("REC");  	  break;
	case 0x28 : printf ("CNF3");  	  break;
	case 0x29 : printf ("CNF2");  	  break;
	case 0x2A : printf ("CNF1");  	  break;
	case 0x2B : printf ("CANINTE");   break;
	case 0x2C : printf ("CANINTF");   break;
	case 0x2D : printf ("EFLG");  	  break;
	case 0x30 : printf ("TXB0CTRL");  break;
	case 0x40 : printf ("TXB1CTRL");  break;
	case 0x50 : printf ("TXB2CTRL");  break;
	case 0x60 : printf ("RXB0CTRL");  break;
	case 0x70 : printf ("RXB1CTRL");  break;
	Default   : printf("Unknown"); 	  break;
	}
}
float print_baud_prescaler( int mCNF1 )
{
	//printf("Clock Speed = 16Mhz\n");
	long int Prescaler = ((mCNF1 & 0x3F)+1)*2;
	float    TQ = 16000. / ((float)Prescaler);
	//printf("Prescaler= %d;\nTQ baudrate=%6.1f kbps\n", Prescaler, TQ );
	return TQ;
}

void print_bit_timing( byte mCNF2, byte mCNF3, float tq )
{
	byte PropagationSegs = ((mCNF2 & 0x07)+1);
	byte Phase1Segs      = ((mCNF2 & 0x38)>>3)+1;
	byte Phase2Segs      = ((mCNF3 & 0x07)+1);
	byte total_segs 	 = PropagationSegs+Phase1Segs+Phase2Segs+1;
	printf("PropogationSegs=%d;\nPhase1Segs=%d\nPhase2Segs=%d\n", PropagationSegs, Phase1Segs, Phase2Segs );
	printf("Total TQ Segs=%d\n", total_segs );
	printf("Final BaudRate=%6.1f kbps\n", tq/((float)total_segs) );
}

void print_speed( int mSpeed )
{
	printf("Baud rate = ");
	switch( mSpeed )
	{
	case CANSPEED_125 : printf("125 kbps\n");	break;
	case CANSPEED_250 : printf("250 kbps\n");	break;
	case CANSPEED_500 : printf("500 kbps\n");	break;
	case CANSPEED_95  : printf("95  kbps\n");	break;
	case CANSPEED_33  : printf("33.33kbps\n");	break;	
	default: break;
	}
}

/**********************************************************************//**
*  Purpose	  :  Reset() - Reset the 2515 chip
*  @param speed - none
*  @return	  true = success
*************************************************************************/
void Reset()
{
   // RESET 2515 CHIP:
   uint8_t    buff[1];
   buff[0]    = SPI_RESET;
   int result = wiringPiSPIDataRW (channel, buff, 1);
   delay(10);
}

/**********************************************************************//**
*  Purpose	  :  Read CAN controller register
*  @param[in]   address - which register to read
*  @param[in]   data - value to put into the register
*  @return	register value
*************************************************************************/
byte transfer( uint8_t data )
{
	uint8_t buff[1];
	buff[0] = data;
	int result = wiringPiSPIDataRW (channel, buff, 1);
	return result;
}
/**********************************************************************//**
*  Purpose	  :  Write CAN controller register
*  @param[in]   address - which register to read
*  @param[in]   data - value to put into the register
*  @return	register value
*************************************************************************/
void write_register( uint8_t address, uint8_t data )
{
	uint8_t buff[3];
	buff[0] = SPI_WRITE;
	buff[1] = address;
	buff[2] = data;
	int result = wiringPiSPIDataRW (channel, buff, 3);
	//printf("Set address %x = %x \n", address, buff[2] );
}
/**********************************************************************//**
*  Purpose	  :  Read CAN controller register
*  @param[in]   address - which register to read
*  @return	register value
*************************************************************************/
uint8_t read_register( uint8_t address )
{
	uint8_t buff[3];
	buff[0] = SPI_READ;
	buff[1] = address;
	buff[2] = 0xFF;
	int result = wiringPiSPIDataRW (channel, buff, 3);
	//printf("Read"); print_register_name( address );
	//printf("\t%x = %x \n", address, buff[2] );
	return buff[2];
}

/**********************************************************************//**
*  Purpose	  :  MultiRegisterWrite - write to consequetive registers (starting at mAddress1)
*  @param mAddress1 - First Register address
*  @param mValueArray  - All register values.  Sequentially from mAddress1 on..
*  @param mArrayLength - mArrayLength number of consequetive registers to write to
*  @return	  true = success
*   	mValueArray :  Contains the response bytes.
*************************************************************************/
void MultiRegisterWrite(uint8_t mAddress1, uint8_t* mValueArray, int mArrayLength )
{
#ifdef WIFI_SOCKET
   uint8_t* buff = new uint8_t[mArrayLength+2];
#else
   uint8_t* buff = (uint8_t*) malloc(mArrayLength+2);
#endif
   buff[0] = SPI_WRITE;
   buff[1] = mAddress1;
   memcpy( (buff+2), mValueArray, mArrayLength );   
   int result = wiringPiSPIDataRW (channel, buff, 2+mArrayLength);
}
/****************************************************************************//**
*  Purpose	  :  MultiRegisterRead - write to consequetive registers (starting at mAddress1)
*  @param mAddress1 - First Register address
*  @param mValueArray - Dummy data.  Input data is not important.  Used to store register values instead.
*  @param mArrayLength - mArrayLength number of consequtive registers to write to
*  @return	  true = success
*   	mValueArray :  Contains the response bytes.
********************************************************************************/
void MultiRegisterRead( uint8_t mAddress1, uint8_t* mValueArray, int mArrayLength )
{
#ifdef WIFI_SOCKET
   uint8_t* buff = new uint8_t[mArrayLength+2];
#else
   uint8_t* buff = (uint8_t*) malloc(mArrayLength+2);
#endif   

   buff[0] = SPI_READ;
   buff[1] = mAddress1;
   memcpy ( (buff+2), mValueArray, mArrayLength );   
   int result = wiringPiSPIDataRW( channel, buff, mArrayLength+2 );
}

/****************************************************************************//**
INPUT:
	byte*	mRegValues	array of 3 (CNF3,CNF2,CNF1)
RETURN:
	1 = Baud Rate found.
********************************************************************************/
byte get_CNF_reg_values( byte* mRegValues, int speed)
{
	/* Base Tq Freq = 2*(BRP+1) / Fosc = 2*(4) / 16Mhz  = 1/2Mhz = 0.500 usec
	3 * .500us = 1.5us => 666kbaud */
	if (speed == CANSPEED_500)
	{
		mRegValues[0] = BT1_500Kbps;            // CNF3 => 2 Phase2 segs
		mRegValues[1] = BT2_500Kbps;            // CNF2  
		mRegValues[2] = BT3_500Kbps;            // CNF1  TQ=16mhz/2*2=4mhz =>0.250us;  SJW=00 or 01;    
	}
	else if (speed == CANSPEED_250)
    {
		/* For 250kbaud use :  Tq = 8;  BRP=3;  SJW=2; SAM; */
		mRegValues[0] = BT1_250Kbps;            // CNF3 => 2 Phase2 segs
		mRegValues[1] = BT2_250Kbps;            // CNF2  
		mRegValues[2] = BT3_250Kbps;            // CNF1  BRP=4; TQ=16mhz/4*2=>0.500us;  SJW=00 or 01;
		/* Match Button Board as closely as possible.
				8 = 3 + 2 + 2 + 1       */
	}
	else if (speed == CANSPEED_125)
	{
		mRegValues[0] = BT1_125Kbps;            // CNF3 => 2 Phase2 segs
		mRegValues[1] = BT2_125Kbps;            // CNF2  
		mRegValues[2] = BT3_125Kbps;            // CNF1  BRP=4; TQ=16mhz/4*2=>0.500us;  SJW=00 or 01;   
	}	
	else if(speed == CANSPEED_95)
	{
		mRegValues[0] = BT1_95K2bps;            // CNF3 => 2 Phase2 segs
		mRegValues[1] = BT2_95K2bps;            // CNF2  
		mRegValues[2] = BT3_95K2bps;            // CNF1  BRP=4; TQ=16mhz/4*2=>0.500us;  SJW=00 or 01;   
	}
	else if(speed == CANSPEED_33)
	{
		mRegValues[0] = BT1_33K3bps;            // CNF3 => 2 Phase2 segs
		mRegValues[1] = BT2_33K3bps;            // CNF2  
		mRegValues[2] = BT3_33K3bps;            // CNF1  BRP=4; TQ=16mhz/4*2=>0.500us;  SJW=00 or 01;   
	}
	else 
		return 0;
	return 1;
}

/**********************************************************************//**
*  Purpose	  :  Setup CAN Bus speed
*  @param speed - Speed of the CAN network (bits/second) [CANSPEED_95,CANSPEED_125,CANSPEED_250,CANSPEED_500]
*  @return	  true = success
*************************************************************************/
//void set_baud( byte mSpeed, byte mPropSegs, byte Phase1Segs, byte Phase2Segs  )
byte set_baud_rate( int speed )
{
	uint8_t speed_value = ((1<<SJW1)|speed);
	uint8_t RegValues [4];
	get_CNF_reg_values( RegValues, speed );
	MultiRegisterWrite( CNF3, RegValues, 3 );
}

BOOL verify_baud_rate( int speed )
{
	uint8_t speed_value = ((1<<SJW1)|speed);
	uint8_t RegValues[4];
	uint8_t RegRead  [4];	
	get_CNF_reg_values( RegValues, speed );

	// Verify Operation by reading back what we just wrote:	
	RegRead[2] = read_register(CNF1);
	RegRead[1] = read_register(CNF2);
	RegRead[0] = read_register(CNF3);

	if ((RegRead[0] == RegValues[0]) && 
		(RegRead[1] == RegValues[1]) && 
		(RegRead[2] == RegValues[2]) )
	{		
		return 1;
	}
	//else printf("\nReturned    : %x %x %x \n", RegRead[0], RegRead[1], RegRead[2] );
	return 0;
}


/**********************************************************************//**
*  Purpose	  :  check if there are any new messages waiting
*  @param[in]   none
*  @return	0=No message;  1=Message available
*************************************************************************/
uint8_t check_message(void)
{
	uint8_t flags = read_rx_status();
	//printf("RX Status= %x\n", flags);
	
	if (flags & (0x40)) { 
		printf("Message RECEIVED on Rx0\n");  
		return 1; 
	}
	if (flags & (0x80)) { 
		printf("Message RECEIVED on Rx1\n");  
		return 2; 
	}
	return 0;
//	old way return (!digitalRead(MCP2515_INT));
}

/**********************************************************************//**
*  Purpose	  : 
*  @param[in]   address		- Register address	
*  @param[in]   mask		- Mask showing bits to update
*  @param[in]	data		- The update values
*  @return	none
*************************************************************************/
void bit_modify(uint8_t address, uint8_t mask, uint8_t data)
{
	uint8_t buff[4];
	buff[0] = SPI_BIT_MODIFY;
	buff[1] = address;
	buff[2] = mask;
	buff[3] = data;
	int result = wiringPiSPIDataRW(channel, buff, 4);
}

/**********************************************************************//**
*  Purpose	  :  read status of the CAN controller
*  See Figure 12-8 in the 2515 manual.
*  This status byte includes:  2 RX and 3 TX interrupt Flags and 3 TXREQ bits.
*	AND  - 	TXB0CNTRL.TXREQ
*			TXB1CNTRL.TXREQ
*			TXB2CNTRL.TXREQ
*  @param[in]   type - 		TEST THIS!
*  @return	status 
*************************************************************************/
uint8_t read_status()
{
	uint8_t buff[3];
	buff[0] = SPI_READ_STATUS;
	buff[1] = 0xFF;
	int result = wiringPiSPIDataRW(channel, buff, 2);
	return buff[1];
}
/**********************************************************************//**
*  Purpose	  :  read status of the CAN controller
*  @param[in]   type - 		TEST THIS!
*  @return	status 
*************************************************************************/
uint8_t read_rx_status()
{
	uint8_t buff[3];
	buff[0] = SPI_RX_STATUS;
	buff[1] = 0xFF;
	int result = wiringPiSPIDataRW(channel, buff, 2);
	return buff[1];
}

/**********************************************************************//**
*  Purpose	  :  Read CAN controller register
*  			check if there is a free buffer to send messages
*  @param[in]   address - which register to read
*  @param[in]   data - value to put into the register
*  @return	register value
*************************************************************************/
uint8_t check_free_buffer(void)
{
	uint8_t status = read_status();

	if ((status & 0x54) == 0x54) {
		// all buffers used
		return false;
	}
	return true;
}

/**********************************************************************//**
*  Purpose	  :  Retrieve the complete CAN message
*  @param[in]   message structure - which will hold the message 
*  @param[in]   timestamp - time in (milliseconds) when the message arrived 
*  @return	[0,status] 0=No Msg available.  
*************************************************************************/
uint8_t get_message(struct sCAN *message, byte mBuffNumber )
{
	uint8_t addr;
	uint8_t t;

	if ( mBuffNumber==0 )	
		addr = 0x00;
	else if (mBuffNumber==1)
		addr = 0x04;
	else
		return 0;	// Error: no message available

	message->time_stamp = micros();

	uint8_t buff[15];
	buff[0] = SPI_READ_RX | addr;
	buff[1] = 0xFF;  // SIDH
	buff[2] = 0xFF;	 // SIDL
	buff[3] = 0xFF;	 // ID8
	buff[4] = 0xFF;	 // ID0
	buff[5] = 0xFF;	 // DLC
	buff[6] = 0xFF;	 // RXB0D0
	buff[7] = 0xFF;	 // RXB0D1
	buff[8] = 0xFF;	 // RXB0D2
	buff[9] = 0xFF;	 // RXB0D3
	buff[10] = 0xFF; // RXB0D4
	buff[11] = 0xFF; // RXB0D5
	buff[12] = 0xFF; // RXB0D6
	buff[13] = 0xFF; // RXB0D7
	buff[14] = 0xFF; // CANSTAT
	int result = wiringPiSPIDataRW(channel, buff, 15);	// buff is updated with new data

	// read id
	byte ide = (buff[2] & 0x08)>0;		// SIDL & 0x08
	if (ide)
		parse_CAN_eid_b( &(message->id), &(buff[1]) );
	else
		parse_CAN_id( &(message->id), &(buff[1]) );

	// Read DLC:
	uint8_t length = buff[5] & 0x0f;
	message->header.DLC = length;
	message->header.rtr = 0;	    //(bit_is_set(status, 3)) ? 1 : 0;

	for (t=0; t<length; t++) {
		message->data[t] = buff[6+t];
	}
	return 1;
}

/**********************************************************************//**
*  Purpose	:  Conver TX buffer bit to address used by SPI_WRITE_TX.
*  @param[in]   message - See sCAN message structure.  Must be filled in.
*  @return	[0,1,2,4] 0 all transmit buffers are busy.  
			Address of the TX buffer which was used.
			(note address 0 is changed to 1)  
*************************************************************************/
uint8_t Determine_tx_address(byte tx_buffer)
{
	/* Statusbyte:
	 *
	 * Bit	Function
	 *  2	TXB0CNTRL.TXREQ
	 *  4	TXB1CNTRL.TXREQ
	 *  6	TXB2CNTRL.TXREQ
	 */
	 /* For normal writes, need address:  0x30, 0x40, 0x50 */
	uint8_t address;
	switch (tx_buffer)
	{
	case 1: address = 0x00;	break;	// Values are for LOAD_TX_BUFF instruction
	case 2: address = 0x02;	break;	
	case 4: address = 0x04;	break;
	default: // all buffer used => could not send message
		address= -1;	break;
	}
	//printf("Tx Address Determined = %x\n", address);
	return address; 	
}

uint8_t Determine_tx_buffer()
{
	uint8_t status = read_status();
	//printf("Determine_tx_buffer()  :  Status = %x;  ", status);
	if (bit_is_clear(status, 2)) { /*printf("using TX buffer 0 \n");*/ return 1; }
	if (bit_is_clear(status, 4)) { /*printf("using TX buffer 1 \n");*/ return 2; }
	if (bit_is_clear(status, 6)) { /*printf("using TX buffer 2 \n");*/ return 4; }
	return 0;
}

/**********************************************************************//**
*  Purpose :  Request_To_Send
*  Note:	Rev 4 is to have gpio pins connected to the RTS on the 2515 chip
*		for faster sending.
*  @param[in]   mTxBufferBit - bit indicating which TX buffer to set 
*		Must be: [1,2,4]
*  @return	[0,1,2,4] 0 all transmit buffers are busy.  
*			Address of the TX buffer which was used.
*			(note address 0 is changed to 1)  
*************************************************************************/
uint8_t Request_To_Send( uint8_t mTxBufferBit )
{
	//printf("RequestToSend Command %d=Low \n",mTxBufferBit);
	// The signal was previously put high after initiation.
	// Active on Falling Edge.
	switch (mTxBufferBit)
	{
		case 1:	digitalWrite( TX0RTS, 0 );		break;
		case 2:	digitalWrite( TX1RTS, 0 );		break;
		case 4:	digitalWrite( TX2RTS, 0 );		break;
		default: 		break;
	}
	//delay (10);	
/*	switch (mTxBufferBit)
	{
		case 1:	digitalWrite( TX0RTS, 1 );		break;
		case 2:	digitalWrite( TX1RTS, 1 );		break;
		case 4:	digitalWrite( TX2RTS, 1 );		break;
		default: 		break;
	} */

/*Software method:
	uint8_t value = SPI_RTS | mTxBufferBit;
	if (mTxBufferBit)  
		transfer( value );*/
}

/**********************************************************************//**
*  Purpose:  	Send a CAN message
* 		Adrenaline is using 11 bit IDs (not extended)
*
*  @param[in]   mabc_address (Txbuff 0,1,2) See datasheet Figure 12-5 Page 66
*  @param[in]   message - See sCAN message structure.  Must be filled in.
*  @return	[0,1,2,4] 0 all transmit buffers are busy.  
			Address of the TX buffer which was used.
			(note address 0 is changed to 1)  
*************************************************************************/
void transfer_message(uint8_t mabc_address, struct sCAN *message)
{
	int t;
	byte buff[14];
	buff[0] = SPI_WRITE_TX | mabc_address;		// occupies lowest 11 bits
	buff[1] = message->id.id_array[0];			// TXBnSIDH	(Standard ID)
	buff[2] = message->id.id_array[1] | 0x08;	// TXBnSIDL	(Standard ID)
	buff[3] = message->id.id_array[2];			// TXBnEID8	
	buff[4] = message->id.id_array[3];			// TXBnEID0

	uint8_t length = message->header.DLC & 0x0f;
	if (message->header.rtr)  {
		// a rtr-frame has a length, but contains no data
		buff[5] = ((1<<RTR) | length);	//printf("RTR!\n");
	} else {
		buff[5] = length;  				// TXBnDLC
		for (t=0;t<length;t++) 
			buff[6+t] = message->data[t];		
	}
	/*printf("RAW SPI BUFFER: ");
	for (int i=0; i<(6+length); i++)
		printf(" %2x ", buff[i]);  */
	//printf("transfer_message result = %x\n", result);
	int result = wiringPiSPIDataRW(channel, buff, 6+length);  // buff is updated with new data 
	delayMicroseconds(1); 
}

uint8_t send_message(struct sCAN* message)
{
	uint8_t txbuff_num = Determine_tx_buffer();
	uint8_t addr       = Determine_tx_address( txbuff_num );
 	transfer_message( addr, message );
	//Request_To_Send( txbuff_num );		// x1 x2 or x4 only values!
	return txbuff_num;
}

uint8_t can_send_msg_no_wait(byte unused, struct sCAN* message)
{
	return send_message(message);
}

uint8_t print_message(struct sCAN* msg)
{	
	printf(" ID=%2X:%4X:%d; ", msg->id.group.block, 
			 msg->id.group.id, msg->id.group.instance );
	printf("  %s\t", getID_Text( msg->id.group.id ) );
	printf("DLC=%d; ", msg->header.DLC );
	printf("D[]=");
	int i;
	for (i=0; i<8; i++)
	{
		if (i >= msg->header.DLC)
			printf("   ");
		else
			printf("%2x ", msg->data[i]);
	}
	if (msg->header.rtr==1)	
		printf("\tRTR\n");
	else
		printf("\tDF\n");
}

void tx_register_dump(byte mBuffer)
{
        byte bank = (mBuffer<<4);
        read_register( TXB0CTRL+bank );
        read_register( TXB0SIDH+bank );
        read_register( TXB0SIDL+bank ); 
        read_register( TXB0EID8+bank ); 
        read_register( TXB0EID0+bank ); 
        read_register( TXB2DLC+bank  );
        read_register( TXB2D0+bank   );
        for (int i=0; i<8; i++)
        read_register( TXB2D0+i+bank );
}

void register_dump()
{
        // READ CAN:
        uint8_t adr=0;
        uint8_t val=0;
        val=read_register( CANINTE );	print_register_name(CANINTE);    printf("\t=%x\n",val);
        val=read_register( CANINTF );	print_register_name(CANINTF);    printf("\t=%x\n",val);
        val=read_register( CANSTAT );	print_register_name(CANSTAT);    printf("\t=%x\n",val);
        val=read_register( CANCTRL );	print_register_name(CANCTRL);    printf("\t=%x\n",val);
        val=read_register( EFLG );		print_register_name(EFLG   );    printf("\t=%x\n",val);
        val=read_register( TEC );		print_register_name(TEC    );    printf("\t=%x\n",val);
        val=read_register( REC );		print_register_name(REC    );    printf("\t=%x\n",val);
        val=read_register( TXRTSCTRL );	print_register_name(TXRTSCTRL);  printf("\t=%x\n",val);
        val=read_register( RXB0CTRL );	print_register_name(RXB0CTRL );  printf("\t=%x\n",val);
        val=read_register( RXB1CTRL );	print_register_name(RXB1CTRL );  printf("\t=%x\n",val);
        val=read_register( TXB0CTRL );	print_register_name(TXB0CTRL );  printf("\t=%x\n",val);
        val=read_register( TXB1CTRL );	print_register_name(TXB1CTRL );  printf("\t=%x\n",val);
        val=read_register( TXB2CTRL );	print_register_name(TXB2CTRL );  printf("\t=%x\n",val);
        if (0)
        {
             val=read_register( CNF1 );	print_register_name(CNF1 );  printf("=%x\n",val);
             val=read_register( CNF2 );	print_register_name(CNF2 );  printf("=%x\n",val);
             val=read_register( CNF3 );	print_register_name(CNF3 );  printf("=%x\n",val);
        }
}


