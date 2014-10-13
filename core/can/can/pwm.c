#include <stdlib.h>
#include <math.h>
#include "bk_system_defs.h"
#include "inttypes.h"
#include "interrupt.h"
#include "pin_definitions.h"
//#include "leds.h"
#include "pwm.h"

#define PRESCALER 1			// Fclk
word TOP = 0xFFFF;

/* This is proper way to write to the 16 bit register! */
void OCR1A_Write( unsigned int i ) 
{
	unsigned char sreg;	
	sreg = SREG;			/* Save global interrupt flag */ 
	cli();					/* Disable interrupts */
	OCR1A = i;				/* Set TCNTn to i */
	SREG = sreg;			/* Restore global interrupt flag */
}
/* This is proper way to write to the 16 bit register! */
void OCR1B_Write( unsigned int i ) 
{
	unsigned char sreg;
	sreg = SREG;	/* Save global interrupt flag */ 
	cli();			/* Disable interrupts */
	OCR1B = i;		/* Set TCNTn to i */
	SREG = sreg;	/* Restore global interrupt flag */ 
}

word get_top_value( float mHerz )
{
	word Top = (F_CPU/PRESCALER) / mHerz;	
	return Top;
}
void set_top( word mTop )
{
	TOP = mTop;
	ICR1 = TOP;
}
void set_base_PWM_freq( float mHerz )
{
	TOP = get_top_value( mHerz );	
	set_top(TOP);
}
byte get_tccr1b_prescaler( word Prescaler )
{
	switch ((Prescaler&0x07))
	{
	case 1	 :	return 0x01;		break;
	case 8	 :	return 0x02;		break;
	case 64	 :	return 0x03;		break;
	case 256  :	return 0x04;		break;
	case 1024 :	return 0x05;		break;
	default	 :  break;
	}
}
void pwm_init( float mBaseFrequencyHerz, BOOL mA_Enable, BOOL mB_Enable )
{
	if (mA_Enable)		PWM_1A_DDR |= (1<<PWM_1A_PIN);
	if (mB_Enable)		PWM_1B_DDR |= (1<<PWM_1B_PIN);

	// No Power Reduction (needed to enable the Timer1)
	PRR &= ~((1<<PRTIM1) | (1<<PRTIM0));

	// 	16 Bit Registers:
	OCR1A_Write( 0 ); 	// set pwm duty
	OCR1B_Write( 0 ); 	// set pwm duty

	// TOP counter value is defined in the ICR1
	// 16Mhz / 65535 = 	244 Hz  (slowest possible at this prescaler)
	set_top( get_top_value( mBaseFrequencyHerz ) );

	// 8 Bit control Registers:
	byte tccr1a = 0x02;
	if (mA_Enable)	tccr1a |= 0x80;
	if (mB_Enable)	tccr1a |= 0x20;
	TCCR1A 	= tccr1a;
	TCCR1B  = 0x19;

	//byte tccr1b = 0x18;
	//TCCR1B = tccr1b | get_tccr1b_prescaler(PRESCALER);
	// Mode 14 Fast PWM ICR1 holds TOP
	// We will be using the Waveform Generation Mode (WGM =0000; Normal Mode)
	// Correction!  We will be using FastPWM ICRn is TOP 
	//	(Mode 14)	
}

/*******************************************
set_polarityA(BOOL mHigh)

HIGH (0.33 duty)
  _________
_|		   |_____________________
LOW (0.33 duty)
_		    _____________________
 |_________|
******************************************/
void set_polarityA( BOOL mHigh )
{
	byte tmp = TCCR1A;
	tmp |= 0x80;
	if (mHigh)
		tmp &= ~(0x40);		// Clear PWM pin on compare match
	else
		tmp |= 0x40;		// Set PWM pin on compare match
	TCCR1A = tmp;		
}
void set_polarityB( BOOL mHigh )
{
	byte tmp = TCCR1A;
	tmp |= 0x20;
	if (mHigh)
		tmp &= ~0x10;
	else
		tmp |= 0x10;
	TCCR1A = tmp;
}
void set_dutyA( float mDuty )
{
	word counts = ceil( (float)TOP * mDuty );
	OCR1A_Write( counts );
}
void set_dutyB( float mDuty )
{
	word counts = ceil( (float)TOP * mDuty );
	OCR1B_Write( counts );
}



