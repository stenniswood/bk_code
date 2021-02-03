#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>

#include <fcntl.h>
#include <linux/input.h>
#include <unistd.h>

#include "crc_32.h"
#include "ps4_hid_outputs.hpp"


unsigned char oBuffer[BUFFER_SIZE];

static  void append_1b( int& mAdr, unsigned char mData )
{
	oBuffer[mAdr++] = (mData & 0x00FF);
}
static void append_2b( int& mAdr, unsigned short mData )
{
	oBuffer[mAdr++] = (mData & 0x00FF);			// PS4 uses Little endian
	oBuffer[mAdr++] = (mData & 0xFF00) >> 8;
}

void dump_obuffer()
{
	printf( "OUTPUT BUFFER : " );
	for (int i=0; i<75+4; i++)
	{
		if ((i%16)==0) printf("\n%4d :  ", i);
		printf("%2x ", oBuffer[i] );		
	}
	printf("\n");
}

void init_obuffer()
{
	for (int i=0; i<BUFFER_SIZE; i++)
		oBuffer[i] = 0;		
}

/*	HID Report header & footer  */
int form_HID_A2_portion(int adr )
{
	append_1b( adr, 0xA2   );
	append_1b( adr, 0x11   );
	append_1b( adr, 0xC0   );
	append_1b( adr, 0x20   );	// 4
	return adr;
}

int form_LED_color( int& StartAdr, bool mFlash, byte mRed, byte mGreen, byte mBlue )
{
	byte d = (0xF2);
	if (mFlash)     d |= 0x04;
	append_1b( StartAdr,  d  );

	StartAdr = +4+4+1;
	append_1b( StartAdr, mRed    );	// red
	append_1b( StartAdr, mGreen  );	// green
	append_1b( StartAdr, mBlue   );	// blue	

	byte mBrightFlash = 0;
	byte mDarkFlash   = 0;
	append_1b( StartAdr, mBrightFlash   );	// Flash LED bright
	append_1b( StartAdr, mDarkFlash     );	// Flash LED dark
	return StartAdr;
}

int form_Rumble( byte mStrong, byte mWeak )
{
	int adr = 4;
	byte d  = (0xf0 | 0x01);
	append_1b( adr,  d );
	
	adr = 7;
	append_1b( adr, mWeak   );
	append_1b( adr, mStrong );
	return adr;
}

void append_checksum( unsigned long int mChecksum )
{
	int adr = 75;
	append_1b( adr,  mChecksum & 0xff  );		mChecksum >>= 8;
	append_1b( adr,  mChecksum & 0xff  );		mChecksum >>= 8;
	append_1b( adr,  mChecksum & 0xff  );		mChecksum >>= 8;
	append_1b( adr,  mChecksum & 0xff  );		mChecksum >>= 8;
}


int send_Rumble( int fd, byte mStrong, byte mWeak )
{
	int adr = 0;
	init_obuffer();
	adr     = form_HID_A2_portion( adr );
	
	form_Rumble( mStrong, mWeak );

	int crc_start  = 0;
	int send_start = 1;
	uint32_t crc = crc_32( &(oBuffer[crc_start]), 75 );
	append_checksum( crc );
	dump_obuffer();
	
	size_t bytes_written = write(fd, &(oBuffer[send_start]), 75+4 -1 );
	printf("send_Rumble:: result = %d\n", bytes_written );
	
	return bytes_written;
}


int set_LED_color( int fd, byte mRed, byte mGreen, byte mBlue )
{
	init_obuffer();

	int adr = 0;
	adr     = form_HID_A2_portion( adr );
	
	form_LED_color( adr, false,  mRed,  mGreen,  mBlue );

	int crc_start = 0;
	int send_start = 1;
	uint32_t crc = crc_32( &(oBuffer[crc_start]), 75 );
	append_checksum( crc );
	
	size_t bytes_written = write(fd, &(oBuffer[send_start]), 75+4 -1 );
	//printf("set_LED_color:: result = %d\n", bytes_written );
	
	return bytes_written;
}

/*
int form_HID_portion(  )
{
	int adr = 9;
	append_1b( adr,  0xA1  );
	append_1b( adr,  0x11  );
	append_1b( adr,  0xC0  );

	// Now Pack 72 bytes of the data here!
	return adr;
}

	HID Report header & footer  
int form_header()
{
	int adr = 0;
	append_1b( adr,  0x02  );
	append_2b( adr, 0x2015 );
	append_2b( adr, 0x0053 );
	append_2b( adr, 0x004F );
	append_2b( adr, 0x0042 );	// 9 
	return adr;
}*/