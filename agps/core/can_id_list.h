#ifndef _CAN_ID_LIST_h
#define _CAN_ID_LIST_h


char* getID_Text(word mID);


/* FOLLOWING IS A LIST OF THE IDENTIFIERS (16 bit):	
	(Block ID not specified; Instance ID determined at RunTime)  

	RX Messages - Instance goes with the intended receiver (no way to identify whos sending)
					if the instance is 0x00, then the intended receiver is all modules who accept that id!
					
	TX Messages - Instance goes with the transmitter (to identify who's sending)
*/
#define ID_SYSTEM_REQUEST_MASK			0xFC00		// INCOMING  ( instance may contain transmitting board, however can be zero as well)

#define ID_BOARD_PRESENCE_REQUEST		0xFC10		// INCOMING  ( instance may contain transmitting board, however can be zero as well)
#define ID_BOARD_PRESENCE_BROADCAST		0xFC11		// OUT (instance goes with the transmitting board)
	#define REQUEST_TYPE_PRESENCE 			0x00
	#define REQUEST_TYPE_REVISION			0x01
	#define REQUEST_TYPE_SERIALNUMBERS		0x02
	#define REQUEST_TYPE_DESCRIPTION		0x03

#define ID_BOARD_REVISION				0xFC12		// OUT "
#define ID_BOARD_SERIAL_NUMBER			0xFC13		// OUT "
#define ID_BOARD_DESCRIPTION			0xFC14		// OUT "

/* Instance Numbers will be unique per network.  ie the counting sequence goes 
	 for all boards not just per type.		*/
#define ID_INSTANCE_CLAIM				0xFC23	// Claim an Instance Number request  (instance goes with transmitter - random)
#define ID_INSTANCE_QUERY				0xFC24	// (MyInstance)
#define ID_INSTANCE_ASSIGN				0xFC25	// Sets Instance based on serial number ("master" sends) 
#define ID_INSTANCE_RESTART				0xFC26	// All nodes to clear their "Confirmed" Instance status.




/* Though the instance of this message is 0 (accepted by all modules), the Assigned Number 
   will be the first data byte inside.  The next 4 bytes are the serial number (binary form)
   Each module should check this serial number against it's own; if they match then from
   then on, claim that Instance number. */

/* Since each board has a unique 8 bit identifier (Instance number)
   The last 8 bits of the LED message identifiers below will 
   specify the board which board the LED is intended for.  Since this
   filtering will likely be in hardware, we don't need to check this
   in the CAN software callback function.
*/
#define ID_SYSTEM_BAUD_CHANGE			0xFC20		// OUT "
#define ID_SYSTEM_SHUT_DOWN				0xFC21		// Low power mode all modules
#define ID_SYSTEM_CONFIGURE				0xFC22		// Set a byte of configuration mode bits.

#define ID_SYSTEM_LED_REQUEST			0x0015	// the system is requesting an LED display pattern.
// Instance goes with the intended receiver

/******* BUTTON BOARD *******************************************************************/
#define ID_BUTTON_PRESSED				0x0035  // (Instance goes with transmitter)
#define ID_BUTTON_ROSTER				0x0036	// "

/******* BIGMOTOR ***********************************************************************/
// RECEIVING MESSAGE IDs (higher ids should prioritize higher):
#define ID_MARK_MOTOR_STOP				0x0040		// 1 or 2 in data[0]
#define ID_MOVE_TO_ANGLE				0x0041		// Instance goes with intended Receiver
#define ID_MOVE_SPEED					0x0042
#define ID_SET_MAX_MOTOR_ACCEL			0x0043

// TRANSMITTING MESSAGE IDs:
#define ID_CALIBRATED_STOP				0x0045		// Upon request FOR DEBUG  1 or 2 in data[0]
#define ID_MOTOR_ANGLE					0x0046		// Instance goes with Transmitter
#define ID_MOTOR_VALUE					0x0047
#define ID_MOTOR_SPEED					0x0048
#define ID_MOTOR_ACCEL					0x0049
#define ID_MOTOR_STATUS					0x004A


/******* LCD ****************************************************************************/
// LCD MODES "data[0]" constants
// RECEIVING MESSAGE IDs:
#define ID_LCD_COMMAND					0X0060		// Sketch, Graph, CAN monitor
#define ID_LCD_DRAW_TEXT				0X0061		// Draws text at current position & font (8 chars at a time)
#define ID_COLLECT_GRAPH_DATA			0X0062		// Specifies which CAN msg and data elements to collect (word only)

/* FORMAT FOR ID_LCD_COMMAND:
data[0] = 0=No action; 1=MoveTo; 2=ClearScreen; 3=Clear EOL; 4=Draw Graph; 5=Draw Polar; 6=Monitor CAN_ID_LIST_h
		  7=Draw Line; 8=DrawHorizontal; 9=DrawVertical; A=Draw Arc; B=DrawCircle; C= Draw Box;
		  D=Invert Region; 		  x10= Set Window; 		  x20= Beep;		  x30= BackLight;
!! See "can_msg.h" for definitions of allowed commands!!
*/

/******* TILT SENSOR ********************************************************************/
//  (RX)
#define ID_TILT_CHANGE_SENSITIVITY		0x0070

// TILT SENSOR (TX)
#define ID_ACCEL_XYZ					0x0071
#define ID_GYRO_XYZ						0x0072
#define ID_MAGNET_XYZ					0x0073

#define ID_POSITION_XYZ					0x0074
#define ID_POSITION_ABC					0x0075		// ABC are rotations about XYZ respectively
#define ID_VELOCITY_XYZ					0x0076
#define ID_VELOCITY_ABC					0x0077
#define ID_ACCELERATION_XYZ				0x0078
#define ID_ACCELERATION_ABC				0x0079

/******* PWM GENERATOR ******************************************************************/
//  (RX)
#define ID_PWM_CHANNEL_UPDATE			0x0080		// Adjusts the raw PWM for 4 channels 
#define ID_CORR_FACTOR_UPDATE			0x0081		// Adjusts the correction for 4 channels 
#define ID_THRUST_UPDATE				0x0082		// Adjusts the quad-copter thrust

// PWM GENERATOR (TX)

/******* ANALOG MODULE ******************************************************************/
//  (RX)
//  (TX)
#define ID_ANALOG_MEASUREMENT			0x00A0		// first byte holds [0..48] starting index
//#define ID_ANALOG_CHANNEL_0_TO_6		0x00A0
//#define ID_ANALOG_CHANNEL_7_TO_13		0x00A1
//#define ID_ANALOG_CHANNEL_14_TO_20		0x00A2
//#define ID_ANALOG_CHANNEL_21_TO_27		0x00A3
// etc

/************************* BOARD MODEL IDENTIFICATION NUMBERS **********************/
/* The following are to be sent as data[0] in the ID_BOARD_PRESENCE_BROADCAST message
 * They identify the model of the board.		 */
#define Adrenaline_Button		 0x01
#define Adrenaline_Tilt  		 0x02
#define Adrenaline_Analog  		 0x03
#define Adrenaline_BigMotor		 0x04
#define Adrenaline_QuadCopter	 0x05
#define Adrenaline_LCD			 0x06
#define Adrenaline_Power		 0x07
#define Adrenaline_BlueTooth	 0x08
#define Adrenaline_SmartBackplane  0x09
#define Adrenaline_Developer	 0x0A
#define Adrenaline_BigMotorEn	 0x0B

#define Adrenaline_Edge			 0xF0
/************************* END BOARD MODEL IDENTIFICATION NUMBERS ******************/

#endif

