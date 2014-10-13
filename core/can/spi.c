/*********************************************************************
Product of Beyond Kinetics, Inc
This core code handles SPI communications hardware

Notes on Initial Debug with PWM generator board:
		Several problems int the "core" spi.c file.
	A)	The ddr pins were oring with MOSI and not (1<<MOSI).
		This cause the SCLK (1<<SCLK) to not operate
	B)  Have to force SS pin to be an output to prevent it 
		from going into slave mode.  Believe this is why
		the SPIF flag was not getting set; module went into 
		slave mode and was expecting an external clock!
	C)  The alternate pins also was missing the 1<<  (1<<SPIPS)

	After this, the motor ran! 

DATE 	:  9/23/2013
AUTHOR	:  Stephen Tenniswood
********************************************************************/
#include <stdlib.h>
#include <avr/sfr_defs.h>
#include <avr/common.h>
#include "inttypes.h"
#include "interrupt.h"
#include "bk_system_defs.h"
#include "pin_definitions.h"
#include "spi.h"
#include "leds.h"


#define SPI_PORT PORTB
#define SPI_DDR  DDRB
#define MISO  0
#define MOSI  1
#define SCLK  7
#define SS    3
#define SS_DDR DDRD
#define SS_PORT PORTD

/******* SPI Transfer Complete ISR ***************/
ISR ( SPI_STC_vect )
{	
}

// Set LSBFIRST or MSBFIRST
void spi_setBitOrder( uint8_t bitOrder )
{
  if(bitOrder == LSBFIRST)
      SPCR |= (1<<DORD);
  else
     SPCR &= ~((1<<DORD));
}

/************************************************************
mode	:	[SPI_MODE0..SPI_MODE3] or
			you can or in (SPI_CLK_POLARITY | SPI_CLK_PHASE)
			
Modes [0..4] have to do with the same polarity and phase.
*************************************************************/
void spi_setDataMode( uint8_t mode )
{
  SPCR = (SPCR & ~SPI_MODE_MASK) | mode;
}

// Sets BaudRate:
void spi_setClockDivider( uint8_t rate )
{
  SPCR = (SPCR & ~SPI_CLOCK_MASK) | (rate & SPI_CLOCK_MASK);
  SPSR = (SPSR & ~SPI_2XCLOCK_MASK) | ((rate >> 2) & SPI_2XCLOCK_MASK);
}

// Set direction register for SCK and MOSI pin.
// MISO pin automatically overrides to INPUT.
// When the SS pin is set as OUTPUT, it can be used as
// a general purpose output port (it doesn't influence
// SPI operations)
void pin_ddr( BOOL mAlternatePins )
{
  if (mAlternatePins) return ;

  // PORTD bit 3 is the SS for normal pins.
  SS_DDR  |= (1<<SS);		// we have to make SS an output or it could go into slave mode!
  SS_PORT |= (1<<SS);		// 

  // DDR  CHIP SELECT PINS (OUTPUT)
  CS_DDR  |= CS_DDR_MASK;		
  CS_PORT |= CS_DDR_MASK;		// ALL HIGH for INIT

  // DDR  MISO,MOSI,SCLK pins
  SPI_DDR  |=  ((1<<MOSI) | (1<<SCLK));  	// output
  SPI_DDR  &=  ~(1<<MISO);					// input

  SPI_PORT &=  ~(1<<MOSI);					//
  SPI_PORT &=  ~(1<<SCLK);					//
}

//***** SPI initialization *****************************************************
/****************************************************************************
INPUT:
	mbit_order	:	[MSGFIRST,LSBFIRST]  
	mSPI_Mode	:	SPI mode [SPI_MODE0..SPI_MODE3]
****************************************************************************/
void spi_init( BOOL mAlternatePins, uint8_t mbit_order, uint8_t mSPI_Mode )
{
   pin_ddr(mAlternatePins);

  /************************ WARNING!! *****************************
   No need to modify MCUCR reigsters.  Default is '0' which means
   the SPI to use regular MISO/MOSI, not the alternate pins 
   NOTE: The Serial Progamming always uses the Alternate pins.
   ****************************************************************/
   if (mAlternatePins)
  	  MCUCR |= (1<<SPIPS); 				        // Clear for MOSI.  Set for MOSI_A
   else
  		MCUCR &= ~(1<<SPIPS); 				    // Clear for MOSI.  Set for MOSI_A

  spi_setBitOrder( mbit_order );		// LIS3d & CMR3000 (accel & gyro) both require msb first.
  spi_setDataMode( mSPI_Mode  );		// FOR LIS3D Accelerometer, SPI_MODE3 is required.
										// CMR3000 wants SPI_MODE0, but sample is done on rising edge as in Mode3
  spi_setClockDivider( SPI_CLOCK_DIV64 );

  // Warning: if the SS pin ever becomes a LOW INPUT then SPI 
  // automatically switches to Slave, so the data direction of 
  // the SS pin MUST be kept as OUTPUT.
  SPCR |= (1<<MSTR);		// Master SPI
  SPCR |= (1<<SPE);			// Enable SPI Module
}

byte spi_transfer(byte _data) 
{
  SPDR = _data; 
  while (! (SPSR & (1<<SPIF)) );
  return SPDR;
}

void spi_attachInterrupt() 
{
  SPCR |= (1<<SPIE);
}

void spi_detachInterrupt() 
{
  SPCR &= ~(1<<SPIE);
}




