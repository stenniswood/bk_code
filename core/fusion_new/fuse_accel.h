#ifdef  __cplusplus
extern "C" {
#endif

extern BOOL  LinearAccelerationDetected;

extern struct fXYZ AccelScaled;
extern struct frXYZ AccelAngularPosition;	// Calc'd from aSin when stationary
extern struct frXYZ AccelAngularVelocity;	// Hold the change in angles (compare to gyro)
extern struct fXYZ AccelRawVariance;		// Holds the variance as measured during trial stationary data collection.
extern struct fXYZ AccelAngleVariance;		// Holds the variance as measured during trial stationary data collection.
extern struct fXYZ AccelDeltaVariance;		// Holds the variance as measured during trial stationary data collection.
extern BOOL   NoLinearAcceleration;

void fuse_accel_init		( 					);
void calc_stationary_angles ( struct fXYZ* mRaw );

char compare_to_1g			( struct fXYZ* mVector, float mMagnitudeThreshold);
//char compare_to_1g			( struct fXYZ* mVector, float mThreshold, BOOL mShowData);
void process_accel			( BOOL ShowData 						);


void print_accel_angles();
void print_accel_velocity();


#ifdef  __cplusplus
}
#endif
