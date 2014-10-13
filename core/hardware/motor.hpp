#ifndef _MOTOR_
#define _MOTOR_

#include "control.hpp"
#include "CAN_Interface.hpp"
#include "ini_file.hpp"


#define MaxMotorTorque 6.0		// newtons

/**********************************************************
  This class is an interface to the CAN protocol for a BigMotorBoard.
  An instance can belong to a Motor Group.
  The group serves the walking sequencer, calibrator,
  configurator, graphical displays, etc.	
 **********************************************************/
class Motor 
{
public:
	Motor( );
	Motor( byte Instance );

	// Accessors : 
	void	 setInstance	( byte mInstance ) 	{ Instance = mInstance; };
	float    getAngle		(   )				{ return Angle; 		};
	void     setAngle		( float mAngle  )	{ Angle = mAngle; 		};
	word     getValue		(	)	{ return Value; 	};
	byte     getInstance	(	)	{ return Instance;	};
	float    getSpeed		(	)	{ return Speed; 	};
	float    getAmperage	(	)	{ return Current;	};
 	float    getDuty		(float mTorque	)	{ return mTorque / MaxMotorTorque;	};
 	
	// Evaluate with Calibration Points : 
	float   getAngleAt		( word mValue 				);
	word    getValueAt		( float mAngle 				);
	word 	getZeroOffset   ( word mValueAtZeroDegrees 	);
	void	adjustZeroOffset( word mValueAtZeroDegrees 	);

	// Stops : 
	void 	MarkStop    	( byte StopNum, float mAngle, word mValue );
	void	setStopAngle	( byte StopNum, float mAngle );
	void	setStopValue	( byte StopNum, word  mValue );
	float	getStopAngle	( byte StopNum 				 );
	word	getStopValue	( byte StopNum 				 );
	byte	inStopRegion	(  );		// 0=>Valid Range; 1=>Stop1; 2=>Stop2;
	BOOL	ValidRange  	(  );		// within control limits.
	BOOL	verifyStops  	(  );
	
	// INI FILE:
	int		SavePreferences	( Preferences* pref			 );	// put into a .ini file
	int		LoadPreferences	( Preferences* pref			 );	
	
	// CAN MESSAGING FUNCTIONS:
	void move_to_angle	 	  (  float mAngleDeg, 	  word mSpeedTenthPercent 		);
	void move_speed	 	  	  (  float mSpeedPercent 								);
	void move_dual_speed	  (  float mSpeedPercent, float mSpeedPercent2 			);
	void mark_motor_stop	  (  byte mStopNumber, 	  float mAngleDeg, word mValue 	);
	void set_max_acceleration (  float mMaxAcceleration 							);
	void read_motor_stop	  (  byte mStopNumber 									);
	void send_message		  (	 );
	void send_stops	  	 	  (  );

	// CONFIGURE:
	void set_config_byte		  ( byte mindex, byte value );
	void send_all_configs		  ( );	 // don't want set/send.  so this will go away.
	
	void read_configure			  ( 										 );	
	void configure_flag			  ( byte mFlag, BOOL mOn 					 );
	void configure_set_update_rate( byte mRate, byte mReports 				 );
	void set_base_frequency		  ( byte mRate  ); 	// BASE_100HZ 
 	byte get_base_frequency		  (				);
	byte get_report_rate		  (				);

	// RECEIVE CAN MESSAGE : 
	int  process_CAN_msg	  	( struct sCAN* mMsg );
	byte can_parse_motor_angle	( struct sCAN* mMsg );
	byte can_parse_motor_value	( struct sCAN* mMsg );
	byte can_parse_motor_speed	( struct sCAN* mMsg );
	byte can_parse_motor_stop	( struct sCAN* mMsg );

	byte can_compare_motor_stop ( struct sCAN* mMsg );

protected:
	// CAN MESSAGING FUNCTIONS:
	void pack_move_to_angle	 	  (  float mAngleDeg, 	  word mSpeedTenthPercent 		);
	void pack_move_speed	 	  (  float mSpeedPercent 								);
	void pack_move_dual_speed	  (  float mSpeedPercent, float mSpeedPercent2 			);
	void pack_mark_motor_stop	  (  byte mStopNumber );
	void pack_set_max_acceleration(  float mMaxAcceleration 							);
	void pack_read_motor_stop	  (  byte mStopNumber 									);
	void pack_configure			  (  byte config_index, byte Mask, byte Value 			);
	
	void 	init();	// helper used by constructors.

private:
	byte Instance;

	// Variable Info:
	word  Value;
	float Angle;		// unit is degrees
	float Current;
	float Speed;

	// Calibration Stops:
	word Stop1_Value;
	word Stop2_Value;		
	float Stop1_Angle;
	float Stop2_Angle;

	byte config_bytes[4];
	// CAN msg used for Motor Related transmissions.
	struct sCAN MMsg;
};


/************* CONFIGURATION *****************/
/**** CONFIG BYTE 1 - BIT DEFS ****/
#define MODE_USE_ENCODER 	0x01		// else use potentiometer.  default is pot (0x00)
#define MODE_2_MOTORS	 	0x02		// 0=> 1 bidirectional;  1=> 2 unidirectional motors
#define MODE_RESERVED_1 	0x04		// else defaults (80%)
#define MODE_FREE_SPIN	 	0x08		// ignore stops.  default (0) is to stop
#define MODE_TILT_RESPONDER 0x10		// 1=> respond to tilt Y axis readings
#define MODE_PID_CONTROL	0x20		// PID or constant speed.
#define MODE_BASE_FREQ_1	0x40		// 00->100Hz;  01--> 300Hz
#define MODE_BASE_FREQ_2	0x80		// 10->1000hz; 11--> 5000Hz

#define BASE_100HZ 0X00
#define BASE_300HZ 0X40
#define BASE_1000HZ 0X80
#define BASE_5000HZ 0XC0
/**** END OF CONFIG BYTE 1 DEFS   ****/
/*************************************/

/*************************************/
/**** CONFIG BYTE 2 - BIT DEFS    ****/
/* Upper nibble of config_byte_2 defines the update rate.  
   BigMotor will send motor angle and current readings
every:
	0	- No report
	1	- Report every 10ms
	2	- Report every 20ms
	3	- Report every 50ms
	4	- Report every 100ms	*/
#define MODE_SEND_UPDATES_NONE	0x00	// 
#define MODE_SEND_UPDATES_10ms	0x10	// 
#define MODE_SEND_UPDATES_20ms	0x20	// 
#define MODE_SEND_UPDATES_50ms	0x40	// 
#define MODE_SEND_UPDATES_100ms	0x80	// 
// Lower nibble indicates which messages will be reported:
#define MODE_SEND_POSITION_RAW	0x01 	// Measured Pot/Encoder, CurrentLeft, CurrentRight, Speed
#define MODE_SEND_POSITION_CALC 0x02 	// Calculated quantities: Angle (deg*100), Current (Amps*100)
#define MODE_SEND_STATUS		0x04 	// 
/**** END OF CONFIG BYTE 2 DEFS   ****/
/*************************************/
 
#endif


