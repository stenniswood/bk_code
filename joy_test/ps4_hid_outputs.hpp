#ifndef _PS4_HID_OUTPUTS_HPP_
#define _PS4_HID_OUTPUTS_HPP_




int init_obuffer	( );
int form_A2_header	( );
int form_LED_color	( int mFlash, byte mRed, byte mGreen, byte mBlue );
int form_Rumble		( int mFlash, byte mRed, byte mGreen, byte mBlue );
int form_HID_portion( unsigned char* mData );

int set_LED_color( byte mRed, byte mGreen, byte mBlue );


#endif
