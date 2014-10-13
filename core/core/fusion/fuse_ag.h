
void accel_stationary_angles(struct fXYZ* mRaw );
void gyro_angles		(struct fXYZ* mRaw 	);
void magnet_angles		( struct fXYZ* mRaw );

void process_accel		( BOOL ShowData 	);
void process_gyro		( BOOL ShowData 	);
void process_magnet		( BOOL ShowData 	);

void compute_accelGyro_fusion();

