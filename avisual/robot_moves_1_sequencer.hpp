


extern bool algorithm_running;
extern bool  start_bow ;		// Set to true to begin the sequence
extern float bow_angle ;		// Set to the desired accelerometer position.

extern bool  start_squat ;		// Set to true to begin the sequence
extern float squat_angle ;		// Set to the desired accelerometer position.

const int Hip_instanceL = 31;	const int Hip_instanceR = 21; 
const int Knee_instanceL = 32;	const int Knee_instanceR = 22; 
const int Ankle_instanceL = 33;	const int Ankle_instanceR = 23; 

void* bow_now_thread( void* param );

