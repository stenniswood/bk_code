#ifdef  __cplusplus
extern "C" {
#endif



extern struct frXYZ  GyroAdjusted;		// dps (after zero offset)
extern struct frXYZ  GyroAngles;		// Integration


extern struct fXYZ AR_weight_gyro; 		// Angular Rate Weights 
extern struct fXYZ GyroOffset  ;		// Holds the average offset as measured during trial stationary data collection.
extern struct fXYZ GyroVariance;		// Holds the variance as measured during trial stationary data collection.
extern float  		prev_gyro_time ;
extern float  		gyro_time_delta;
extern struct fXYZ SumDeltaGyroVariances;
extern struct fXYZ SumGyroDeviations;


void fuse_gyro_init();
void accumulate(struct fXYZ* mRaw, struct fXYZ* mAccumd, int *mNumberOfTimes );
void process_gyro( BOOL ShowData );

void print_gyro_result();
void print_gyro_results();


#ifdef  __cplusplus
}
#endif
