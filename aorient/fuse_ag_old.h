/******************* CALIBRATION DATA *********************************
 The following data was collected by recording the Gyro 
  measurements with the tilt sensor sitting on a table with a weight 
  on it.  (Hence no rotation).  Two data collections were made giving
  similar results.  First 200 samples.  Second 656 samples.

  1 bit on the Gyro is 0.375 degree per second.			*/
#define Gyro_offset_x (  33.6 * 0.375 )
#define Gyro_offset_y ( 156.1 * 0.375 )
#define Gyro_offset_z (-221.5 * 0.375 )

#define Gyro_variance_x (17*0.375)
#define Gyro_variance_y (15*0.375)
#define Gyro_variance_z (16*0.375)

#define Gravity_variance 0.007

// Variance in the raw Accelerometer readings:
#define Accel_variance_raw_x 0.007
#define Accel_variance_raw_y 0.007
#define Accel_variance_raw_z 0.007

// After converting to an angle:
#define Accel_variance_angle_x 0.4
#define Accel_variance_angle_y 0.4
#define Accel_variance_angle_z 0.4

// After converting to an angle (quadrature of above):
#define Accel_variance_angle_delta_x 0.5
#define Accel_variance_angle_delta_y 0.5
#define Accel_variance_angle_delta_z 0.5
/*********************************************************/

/******************* FUNCTIONS ***************************/
void fuse_init				(					);
void accel_stationary_angles( struct fXYZ* mRaw );
void accumulate_gyro_angles	( struct fXYZ* mRaw );
void magnet_angles			( struct fXYZ* mRaw );

void process_accel			( BOOL ShowData 	);
void process_gyro			( BOOL ShowData 	);
void process_magnet			( BOOL ShowData 	);
void compute_accelGyro_fusion(BOOL mShowDebug	);
/*********************************************************/

#define cast_f (struct fXYZ *)

/***********************************************************/
extern struct fXYZ BestAnglesPrev;		// Holds the best combined estimate of the orientation.
/*********************  FINAL RESULTS  *********************/
extern struct fXYZ BestAngularPosition;	// Holds the best combined estimate of the orientation.
extern struct fXYZ BestAngularRate;		// Holds the best estimate of rate of turn.
extern struct fXYZ BestAngularAccel;	// Holds the best estimate of angular acceleration
extern struct fXYZ BestLinearPosition;	// Holds the best estimate of rate of turn.
extern struct fXYZ BestLinearVelocity;	// Holds the best estimate of rate of turn.
extern struct fXYZ BestLinearAcceleration;	// Holds the best estimate of rate of turn.
/***********************************************************/



extern struct fXYZ AR_weight_accel;				// 1=> Gyro; 2=> AccelDelta;