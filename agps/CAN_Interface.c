/*
  Test.h - Test library for Wiring - implementation
  Copyright (c) 2006 John Doe.  All right reserved.
*/

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
#include "global.h"
#include "CAN_Interface.h"
#include <wiringPiSPI.h>
#include <wiringPi.h>
#include "pican_defines.h"

// Declare 1 instance
CANInterface CAN;

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
	printf("Clock Speed = 16Mhz\n");
	long int Prescaler = ((mCNF1 & 0x3F)+1)*2;
	float    TQ = 16000. / ((float)Prescaler);
	printf("Prescaler= %d;\nTQ baudrate=%6.1f kbps\n", Prescaler, TQ );
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
	default: break;
	}
}

/**********************************************************************//**
*  Purpose	  :  Construct CAN
*  @param speed - Speed of the CAN network (bits/second) [CANSPEED_95,CANSPEED_125,CANSPEED_250,CANSPEED_500]
*  @return	  true = success
*************************************************************************/
CANInterface::CANInterface(void)
: channel(0) 	// Wired to CE0 not CE1!  See Pican Schematics.
{
    int speed = 500000;

    // SPI PINS:
    if( wiringPiSPISetup(channel, speed)==-1)
    {
	printf("Could not initialise SPIn");
	printf("Make sure you run this as root: sudo ./picans");
	return;
    }
    printf("SPI Port opened at %d bps\n", speed );
    printf("Constructed CANInterface ");

    //Init(CANSPEED_500)
}

/**********************************************************************//**
*  Purpose	  :  Reset() - Reset the 2515 chip
*  @param speed - none
*  @return	  true = success
*************************************************************************/
void CANInterface::Reset()
{
   // RESET 2515 CHIP:
   uint8_t buff[1];
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
byte CANInterface::transfer( uint8_t data )
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
void CANInterface::write_register( uint8_t address, uint8_t data )
{
	uint8_t buff[3];
	buff[0] = SPI_WRITE;
	buff[1] = address;
	buff[2] = data;
	int result = wiringPiSPIDataRW (channel, buff, 3);
	printf("Set address %x = %x \n", address, buff[2] );
}
/**********************************************************************//**
*  Purpose	  :  Read CAN controller register
*  @param[in]   address - which register to read
*  @return	register value
*************************************************************************/
uint8_t CANInterface::read_register( uint8_t address )
{
	uint8_t buff[3];
	buff[0] = SPI_READ;
	buff[1] = address;
	buff[2] = 0xFF;
	int result = wiringPiSPIDataRW (channel, buff, 3);
	print_register_name( address );
	printf("Read \t%x = %x \n", address, buff[2] );
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
void CANInterface::MultiRegisterWrite(uint8_t mAddress1, uint8_t* mValueArray, int mArrayLength )
{
   uint8_t* buff = (uint8_t*) malloc(mArrayLength+2);
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
void CANInterface::MultiRegisterRead( uint8_t mAddress1, uint8_t* mValueArray, int mArrayLength )
{
   uint8_t* buff = (uint8_t*) malloc(mArrayLength+2);
   buff[0] = SPI_READ;
   buff[1] = mAddress1;
   memcpy ( (buff+2), mValueArray, mArrayLength );   
   int result = wiringPiSPIDataRW (channel, buff, mArrayLength+2 );
}

/**********************************************************************//**
*  Purpose	  :  Setup CAN Bus speed
*  @param speed - Speed of the CAN network (bits/second) [CANSPEED_95,CANSPEED_125,CANSPEED_250,CANSPEED_500]
*  @return	  true = success
*************************************************************************/
byte CANInterface::SetChipSpeed( int speed )
{
	uint8_t speed_value = ((1<<SJW1)|speed);
	uint8_t mRegValues[4];
	uint8_t RegValues [4];
	uint8_t RegRead   [3];
	print_speed( speed );
	float tq = print_baud_prescaler( speed );

	/* Base Tq Freq = 2*(BRP+1) / Fosc = 2*(4) / 16Mhz  = 1/2Mhz = 0.500 usec
	3 * .500us = 1.5us => 666kbaud */
	 
	if (speed == CANSPEED_250)
	{
		if (0) {
			/* For 250kbaud use :  Tq = 16;  BRP=1;  SJW=2; SAM; 
			 CNF1 = 0x41;
			 CNF2 = 0xEA;	// add BTL?
			 CNF3 = 0x05	*/
			mRegValues[0] = 0x05;		// CNF3
			mRegValues[1] = 0xEA;		// CNF2
			mRegValues[2] = 0x01;		// CNF1  BRP=1=>.250us;  SyncJumpWidth=2xlength;  
		} else {
			/* For 250kbaud use :  Tq = 8;  BRP=1;  SJW=2; SAM; 
			 Alternate:
			 CNF1 = 0x03
			 CNF2 = 0xD1
			 CNF3 = 0x01	 */
			mRegValues[0] = 0x01;		// CNF3 => 2 Phase2 segs
			// or 0x01;  but where is the final sjw bit as in mega16m1 ( oh in by default)
			mRegValues[1] = 0xCA;		// CNF2	 
			mRegValues[2] = 0x03;		// CNF1  BRP=4; TQ=16mhz/4*2=>0.500us;  SJW=00 or 01;
			/* Match Button Board as closely as possible.
				8 = 3 + 2 + 2 + 1	*/
		}
	}
	else if(speed == CANSPEED_95)
	{
		mRegValues[0] = (1<<PHSEG21);  // Bitrate 250kbps at 16 MHz.  	write to CNF3
		mRegValues[1] = (1<<BTLMODE)|(1<<PHSEG11)|(1<<PHSEG2); //	write to CNF2 fix to go back to 500 kbps
	}
	else
	{
		mRegValues[0] = (1<<PHSEG21);  //Bitrate 250kbps at 16 MHz.  	write to CNF3
		mRegValues[1] = (1<<BTLMODE)|(1<<PHSEG11); 			//	write to CNF2 fix to go back to 500 kbps
	}
	//mRegValues[2] = speed_value;							 //	write to CNF1
	mRegValues[3] = (1<<RX1IE)|(1<<RX0IE);	// CANINTE - activate interrupts
	print_bit_timing( mRegValues[1], mRegValues[0], tq );
	printf("\nSetChipSpeed: %x %x %x %x \n", mRegValues[0], mRegValues[1], mRegValues[2], mRegValues[3] );
	memcpy ( RegValues, mRegValues, 3);
	MultiRegisterWrite( CNF3, mRegValues, 4 );

	// Verify Operation by reading back what we just wrote:	
	RegRead[2] = read_register(CNF1);
	RegRead[1] = read_register(CNF2);
	RegRead[0] = read_register(CNF3);

	if ((RegRead[0] == RegValues[0]) && 
		(RegRead[1] == RegValues[1]) && 
		(RegRead[2] == RegValues[2]) )
		{
		printf("\nReturned    : %x %x %x \n", RegRead[0], RegRead[1], RegRead[2] );
		return 1;
		}
	return 0;
}

void CANInterface::setMask( uint8_t mByte, long int mMask )
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
bool CANInterface::Init(int speed, uint8_t mLoopbackMode)
{	
	Reset();
	uint8_t result = SetChipSpeed( speed );
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
	write_register(RXB0CTRL, (1<<RXM1)|(1<<RXM0));
	write_register(RXB1CTRL, (1<<RXM1)|(1<<RXM0));

	setMask( 0, 0x00000000 );
	setMask( 1, 0x00000000 );

	// reset device to normal mode
	if (mLoopbackMode)
		write_register(CANCTRL, 0x40); // Loopback mode
	else
		write_register(CANCTRL, 0x00); // reset device to normal mode

	CAN.write_register( TXB0CTRL, 0x01 );	// set priorities
	CAN.write_register( TXB1CTRL, 0x02 );
	CAN.write_register( TXB2CTRL, 0x03 );	
		
	return true;
}

/**********************************************************************//**
*  Purpose	  :  check if there are any new messages waiting
*  @param[in]   none
*  @return	0=No message;  1=Message available
*************************************************************************/
uint8_t CANInterface::check_message(void)
{
	uint8_t flags = read_rx_status();
	printf("RX Status= %x\n", flags);
	
	if (flags & (0x40)) { printf("Message RECEIVED on Rx0\n");  return 1; }
	if (flags & (0x80)) { printf("Message RECEIVED on Rx1\n");  return 2; }
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
void CANInterface::bit_modify(uint8_t address, uint8_t mask, uint8_t data)
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
*  @param[in]   type - 		TEST THIS!
*  @return	status 
*************************************************************************/
uint8_t CANInterface::read_status()
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
uint8_t CANInterface::read_rx_status()
{
	uint8_t buff[3];
	buff[0] = SPI_RX_STATUS;
	buff[1] = 0xFF;
	int result = wiringPiSPIDataRW(channel, buff, 2);
	return buff[1];
}

/* ---------------------------------------------------------------------------- */

// check if there is a free buffer to send messages
/**********************************************************************//**
*  Purpose	  :  Read CAN controller register
*  @param[in]   address - which register to read
*  @param[in]   data - value to put into the register
*  @return	register value
*************************************************************************/
uint8_t CANInterface::check_free_buffer(void)
{
	uint8_t status = read_status();

	if ((status & 0x54) == 0x54) {
		// all buffers used
		return false;
	}
	return true;
}

/* ---------------------------------------------------------------------------- */

/**********************************************************************//**
*  Purpose	  :  Retrieve the complete CAN message
*  @param[in]   message structure - which will hold the message 
*  @param[in]   timestamp - time in (milliseconds) when the message arrived 
*  @return	[0,status] 0=No Msg available.  
*************************************************************************/
uint8_t CANInterface::get_message(sCAN *message, byte mBuffNumber )
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
	buff[1] = 0xFF; // SIDH
	buff[2] = 0xFF;	// SIDL
	buff[3] = 0xFF;	// ID8
	buff[4] = 0xFF;	// ID0
	buff[5] = 0xFF;	// DLC
	buff[6] = 0xFF;	// RXB0D0
	buff[7] = 0xFF;	// RXB0D1
	buff[8] = 0xFF;	// RXB0D2
	buff[9] = 0xFF;	// RXB0D3
	buff[10] = 0xFF; // RXB0D4
	buff[11] = 0xFF; // RXB0D5
	buff[12] = 0xFF; // RXB0D6
	buff[13] = 0xFF; // RXB0D7
	buff[14] = 0xFF; // CANSTAT
	int result = wiringPiSPIDataRW(channel, buff, 15);	// buff is updated with new data

	// read id
	message->id.id_array[0] = buff[1];
	message->id.id_array[1] = buff[2];
	message->id.id_array[2] = buff[3];
	message->id.id_array[3] = buff[4];

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
uint8_t CANInterface::Determine_tx_address(byte tx_buffer)
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
	printf("Tx Address Determined = %x\n", address);
	return address; 	
}

uint8_t CANInterface::Determine_tx_buffer()
{
	uint8_t status = read_status();
	printf("Determine_tx_buffer()  :  Status = %x;  ", status);
	if (bit_is_clear(status, 2)) { printf("using TX buffer 0 \n"); return 1; }
	if (bit_is_clear(status, 4)) { printf("using TX buffer 1 \n"); return 2; }
	if (bit_is_clear(status, 6)) { printf("using TX buffer 2 \n"); return 4; }
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
uint8_t CANInterface::Request_To_Send( uint8_t mTxBufferBit )
{
	printf("RequestToSend Command %d=Low \n",mTxBufferBit);
	switch (mTxBufferBit)
	{
		case 1:	digitalWrite( TX0RTS, 0 );		break;
		case 2:	digitalWrite( TX1RTS, 0 );		break;
		case 4:	digitalWrite( TX2RTS, 0 );		break;
		default: 		break;
	}
	delay (100);	
	switch (mTxBufferBit)
	{
		case 1:	digitalWrite( TX0RTS, 1 );		break;
		case 2:	digitalWrite( TX1RTS, 1 );		break;
		case 4:	digitalWrite( TX2RTS, 1 );		break;
		default: 		break;
	}
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
void CANInterface::transfer_message(uint8_t mabc_address, sCAN *message)
{
	byte buff[14];
	buff[0] = SPI_WRITE_TX | mabc_address;	// occupies lowest 11 bits
	buff[1] = message->id.id_array[0];		// TXBnSIDH	(Standard ID)
	buff[2] = message->id.id_array[1];		// TXBnSIDL	(Standard ID)
	buff[3] = message->id.id_array[2];		// TXBnEID8	
	buff[4] = message->id.id_array[3];		// TXBnEID0

	uint8_t length = message->header.DLC & 0x0f;
	if (message->header.rtr) {
		// a rtr-frame has a length, but contains no data
		printf("RTR!\n");
		buff[5] = ((1<<RTR) | length);
	} else { 
		buff[5] = length;  	// TXBnDLC
		for (int t=0;t<length;t++) {
			buff[6+t] = message->data[t];
		}
	}
	int result = wiringPiSPIDataRW(channel, buff, 6+length);  // buff is updated with new data
	delayMicroseconds(1);
	printf("transfer_message result = %x\n", result);
}

uint8_t CANInterface::send_message(struct sCAN* message)
{
	uint8_t txbuff_num = Determine_tx_buffer();
	uint8_t addr       = Determine_tx_address( txbuff_num );
 	transfer_message( addr, message );
	//Request_To_Send( txbuff_num );		// x1 x2 or x4 only values!
	printf("Message sent\n" );
	return txbuff_num;
}

uint8_t CANInterface::print_message(struct sCAN* msg)
{
	printf("Block=%2x; ID=%2x; Instance=%2x; ", msg->id.group.block, msg->id.group.id, msg->id.group.instance );
	printf("DLC=%d; ", msg->header.DLC );
	printf("Data[]= ");
	for (int i=0; i<msg->header.DLC; i++)
		printf("%2x ", msg->data[i]);

	if (msg->header.rtr==1)	
		printf("\tRTR\n");
	else
		printf("\tDF\n");
	//printf("\n=== Msg Complete ===\n\n");
}
