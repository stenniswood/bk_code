#ifndef _MOTOR_VECTOR_H_
#define _MOTOR_VECTOR_H_


#ifdef  __cplusplus
extern "C" {
#endif

extern int Current_Vindex;

/* Info for 1 Motor board. */
struct stMotorStateInfo
{
	float Angle;
	word  PotValue;				// Current reading
	word  CurrentTimesTen;		// Current Tenth Amps (fixed point)
	word  SpeedTimesTen;		// Tenth Degree Per Second
}; 

// An array of 2 Vectors will be predefined.
#define LEFT_LEG_INDEX  0
#define RIGHT_LEG_INDEX 1


//extern int iterations_requested;		// -1 means infinite.
//extern int iterations_completed;


/********** FUNCTIONS ******************************************/
//void set_rx_vector_refreshed_callback	( BOOL (*mCallback)(byte VectorIndex) 	);	// When info from CAN has completely refreshed the Vector's angles, call this function.

//BOOL get_appendage_actuator ( int* Aindex, int* actuator_index, byte mInstance );
//float get_destination		( int mAppendageIndex, int mVectorIndex, int mActuator );


#ifdef  __cplusplus
}
#endif

#endif

