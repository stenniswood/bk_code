#ifdef  __cplusplus
extern "C" {
#endif



// COMMANDS:
#define UPDATE_PAN_TILT 		0x01
#define UPDATE_LOWSIDE_DRIVER 	0x02
#define UPDATE_LOWSIDE_DRIVERS 	0x03
#define READ_ANALOG				0x04
#define READ_ANALOGS			0x05

extern float Tilt;
extern float Pan ;
extern byte	LED_status;				// LED_Status
extern byte	LS_Values;				// Lowside Drivers
extern byte Buttons;				// 3 front edge buttons
#define NUM_ANALOG_CHANNELS 8
extern word Analog_Values[NUM_ANALOG_CHANNELS];		// Really is defined in atmel_spi.h !


void update_pan_tilt_angles	( float mPan, float mTilt  );
void update_low_side_driver	( byte mBit,   byte mValue );
void update_low_side_drivers( byte mValues	);
word read_analog			( byte mChannel	);
void read_all_analogs		(				);

void test_command(byte mCmd);


#ifdef  __cplusplus
}
#endif
