#ifndef _PS4_AUDIO_OUTPUTS_HPP_
#define _PS4_AUDIO_OUTPUTS_HPP_


#include "joystick_raw.hpp"


/*#define BUFFER_SIZE 1024
extern unsigned char aoBuffer[BUFFER_SIZE];

void init_aobuffer	( );
int  form_header();

int  form_A2_header	( );
int  form_LED_color	( int& StartAdr, bool mFlash, byte mRed, byte mGreen, byte mBlue );
int  form_Rumble		( int mFlash, byte mRed, byte mGreen, byte mBlue );
int  form_HID_portion( unsigned char* mData );
void append_checksum( unsigned long int mChecksum );
*/


int  send_audio  ( int fd, byte* mData, int mLength );


void send_audio_buffer( int fd );


#endif


