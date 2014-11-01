******************** TILT SENSOR ***************************
This app is intended to process a single Tilt sensor inputs.

It reads the accel, gyro, & magnet, converts to floating point types.
	normalizes to gravity, and has an algorithm to compute the world 
	orientation.  
	
Additional linear acceleration, speed, position is computed after.

This will also catagorize position such as :  "on back", "face down", "standing", "upside down"
"reclined", "leaning forward", etc

