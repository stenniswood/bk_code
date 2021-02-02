
int init_obuffer()
{
	for (int i=0; i<BUFFER_Size; i++)
	{
		oBuffer[i] = 0;		
	}	
}

/*	HID Report header & footer  */
int form_A2_header()
{
	int adr = 0;
	append_1b( adr, 0xA2   );
	append_1b( adr, 0x11   );
	append_1b( adr, 0x80   );
	append_1b( adr, 0x00   );
	return adr;
}

int form_LED_color( int mFlash, byte mRed, byte mGreen, byte mBlue )
{
	int adr = 4;
	byte d = ( mFlash & 0x01 ) | (0x20);	
	append_1b( adr, 0xf0 | d  );
	
	adr = 9;
	append_1b( adr, mRed    );	// red
	append_1b( adr, mGreen  );	// green
	append_1b( adr, mBlue   );	// blue	

	append_1b( adr, mBrightFlash   );	// Flash LED bright
	append_1b( adr, mDarkFlash     );	// Flash LED dark

}

int form_Rumble( int mFlash, byte mRed, byte mGreen, byte mBlue )
{
	int adr = 4;
	byte d = ( mFlash & 0x01 ) | (0x20);	
	append_1b( adr, 0xf0 | d  );

}

int form_HID_portion( unsigned char* mData )
{
	append_1b( adr,  0xA1  );
	append_1b( adr,  0x11  );
	append_1b( adr,  0xC0  );

	// Now Pack 72 bytes of the data here!
	return adr;
}

int set_LED_color( byte mRed, byte mGreen, byte mBlue )
{
	init_obuffer();
	int adr = form_A2_header();
	form_LED_color( int mFlash, byte mRed, byte mGreen, byte mBlue );

	form_HID_portion( unsigned char* mData );
	write();
			
}

