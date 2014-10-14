#ifndef _MOTOR_VECTOR_H_
#define _MOTOR_VECTOR_H_


/* Info for 1 Motor board. */
struct stMotorStateInfo
{
	float Angle;
	word  PotValue;
	word  CurrentTimesTen;		// Current Tenth Amps (fixed point)
	word  SpeedTimesTen;		// Tenth Degree Per Second
}; 

// An array of 2 Vectors will be predefined.
#define LEFT_LEG_INDEX  0
#define RIGHT_LEG_INDEX 1

/* 1 Motor Vector */
struct stVector {
	char* Name;
	byte  Dimension;
	byte  ElementsFilled;		// Bitmap. Use this each time a CAN ID_MOTOR_ANGLE msg comes in to indicate we have a new reading of the element.  Up to 8 elements
	byte* Instances;
	float* CurrAngle;		// in Degrees * 10
	float* NextAngle;		// in Degrees * 10
	word* SpeedTimesTen;
	word* CurrentTimesTen;	
	
	struct stVector* Next;
	struct stVector* Prev;	
};


/********** FUNCTIONS ******************************************/
void set_rx_vector_refreshed_callback	( BOOL (*mCallback)(byte VectorIndex) 	);	// When info from CAN has completely refreshed the Vector's angles, call this function.
BOOL can_motor_position_responder	 	( struct sCAN* mMsg 				);
BOOL can_vector_read_complete_responder	( byte VectorIndex 					);
byte vector_fully_read		( byte mVindex 									);
void init_appendages		(												);	// Init various code.
void Init_Appendage			( byte mVectIndex, char* mName, byte mDimension );
//void InitVector				( byte mVectIndex, char* mName, byte mDimension );	// Init 1 vector.

int scan_for_instance		( byte mVectorIndex, byte mInstance 				);
void CopyInstances			( byte mVectorIndex, byte* mInstances 				);	// Copy supplied instances into the vector.
void set_Speeds				( byte mVectorIndex, word* mSpeeds					);
void set_NextAngles			( byte mVectorIndex, float* mAngles 				);
void set_CurrAngle			( byte mVectorIndex, byte index, float mAngle 		);
void set_Current			( byte mVectorIndex, byte index, float mAmpsTimesTen);
void send_speed_vector		( byte mVectorIndex 								);
void send_angle_vector		( byte mVectorIndex 								);
void configure_motor_reports( byte mVectorIndex, byte mRate, byte mReports		);


//struct stVector* NewVector( char* mName, byte mDimension 								);
/*struct stMotorStateInfo
{
	float Angle;
	word  PotValue;
	word  CurrentTimesTen;		// Current Tenth Amps (fixed point)
	word  SpeedTimesTen;		// Tenth Degree Per Second
}; */


#endif
