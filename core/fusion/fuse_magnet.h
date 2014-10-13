#ifdef  __cplusplus
extern "C" {
#endif


//extern struct frXYZ MagnetAngles;		// Sin/Cos when stationary
extern struct frXYZ MagnetAngularPosition;			// 
extern struct frXYZ MagnetAngularPositionPrev;		// 
extern struct frXYZ MagnetAngularVelocity;			// 
extern struct fXYZ  magnetReference;

void print_mag_angles();
void print_magrefence();
void fuse_magnet_init();

void magnet_angles ( struct fXYZ* mRaw );
void compute_velocity();
void get_magtime_update();
void process_magnet( BOOL ShowData 	   );

#ifdef  __cplusplus
}
#endif
