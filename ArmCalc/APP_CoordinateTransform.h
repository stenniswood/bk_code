#include <math.h>
#include <stdint.h>

#include "APP_Servos.h"


/* Use by calling CT_CalculateForwardEquations() each time the ServoN 
   motors have been updated.  When displaying coordinates only, just call before
   drawing the display. 
   
   This utility references the Servo1 variables from the DEV_OCpwm.h device driver.
*/

/********************************************************************
********************	DEFINITIONS   *******************************
*********************************************************************/

// Count values: 
extern uint16_t Servo1,Servo2,Servo3,Servo4,Servo5,Servo6;

#define BASE_SERVO 			Servo1
#define SHOULDER_SERVO		Servo2
#define ELBOW_SERVO			Servo3
#define WRIST_SERVO			Servo4
#define WRIST_ROTATE_SERVO	Servo5
#define GRIPPER_SERVO		Servo6


#define BASE_SERVO_NUM 	   		1
#define SHOULDER_SERVO_NUM 		2
#define ELBOW_SERVO_NUM    		3
#define WRIST_SERVO_NUM    		4
#define WRIST_ROTATE_SERVO_NUM 	5
#define GRIPPER_SERVO_NUM 		6




// THE FOLLOWING ARE REAL MEASUREMENTS! (expressed in inches)
/*#define LENGTH_SHOULDER 	4.75
#define LENGTH_ELBOW		4.75
#define LENGTH_WRIST		6.0
#define BASE_HEIGHT			2.5 */
// THE ABOVE ARE REAL MEASUREMENTS! 	(expressed in inches)



/********************************************************************
********************  INTERMEDIATE DATA   ***************************
*********************************************************************/

extern float X,Y,Z;			// THE ARM COORDINATES.

// UTILITY FUNCTIONS:
float 	CT_Convert_Radians_to_Degrees(float mRadians);
float 	CT_Convert_Degrees_to_Radians(float mRadians);
float 	CT_Convert_FlipDirection	 (float mRadians);

void 	CT_CalculateShoulder();		// Stage 1
void 	CT_CalculateElbow();		// Stage 2
void	CT_CalculateWrist();		// Stage 3
void	CT_Calculate_XYZ();			// Stage 4


// Do all functions here:
void CT_CalculateForwardEquations( struct stServoAnglesSet* sas, float* X, float* Y, float* Z );




