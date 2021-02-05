#ifndef _PS4_HID_OUTPUTS_HPP_
#define _PS4_HID_OUTPUTS_HPP_


#include "joystick_raw.hpp"


#define BUFFER_SIZE 1024
extern unsigned char oBuffer[BUFFER_SIZE];


void init_obuffer	( );
int  form_header();

int  form_A2_header	( );
int  form_LED_color	( int& StartAdr, bool mFlash, byte mRed, byte mGreen, byte mBlue );
int  form_Rumble		( int mFlash, byte mRed, byte mGreen, byte mBlue );
int  form_HID_portion( unsigned char* mData );
void append_checksum( unsigned long int mChecksum );


int  set_Volume   ( int fd, byte mLeft, byte mRight, byte mMicrophone );
int  set_LED_color( int fd, byte mRed, byte mGreen, byte mBlue );
int  send_Rumble  ( int fd, byte mStrong, byte mWeak );


#endif
