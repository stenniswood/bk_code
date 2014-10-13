#ifndef _PWM_H_
#define _PWM_H_

extern word TOP;

// For atmega16m1 micro, the 16 bit output compares will always have the same DDR/PORT
// Configurations.  So we don't need to define these in pin_definitions.

#define PWM_1A_DDR DDRD
#define PWM_1B_DDR DDRC

#define PWM_1A_PORT PORTD
#define PWM_1B_PORT PORTC

#define PWM_1A_PIN 2		// PD2
#define PWM_1B_PIN 1		// PC1


void OCR1A_Write  ( unsigned int i );
void OCR1B_Write  ( unsigned int i );

byte get_tccr1b_prescaler( word Prescaler );

word get_top_value( float mHerz    );
void set_top	  ( word mTop 	   );
void set_base_PWM_freq( float mHerz );

void set_dutyA	  ( float mDuty    );
void set_dutyB	  ( float mDuty    );

void set_polarityA( BOOL mHigh );
void set_polarityB( BOOL mHigh );

void pwm_init	  ( float mBaseFrequencyHerz, BOOL mA_Enable=TRUE, BOOL mB_Enable=TRUE  );



#endif
