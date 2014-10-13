#ifdef  __cplusplus
extern "C" {
#endif

struct tXYZ
{
	short x;
	short y;
	short z;
};

struct fXYZ
{
	float x;
	float y;
	float z;
};

struct frXYZ
{
	float rx;		// Rotation about X axis
	float ry;		// Rotation about Y axis
	float rz;		// Rotation about Z axis
};


union uXYZ 
{
	uint8_t  array[6];	// byte access
	struct tXYZ	 coords;	// structure
	uint16_t value[3];	// xyz as an array
};


void print_vector		( struct fXYZ* mXYZ 				);
void print_ivector		( struct tXYZ* mXYZ 				);
void print_avector		( struct frXYZ* mXYZ 				);

float rad_to_deg( float radians );
float deg_to_rad( float radians );


void  convert_to_real	( struct tXYZ* mIn, struct fXYZ* fOut 	 );
float angle_between	 	( struct fXYZ* mA, struct fXYZ* mB		 );
void  degrees_to_radians( struct frXYZ* mDeg, struct frXYZ* mRad );
void  radians_to_degrees( struct frXYZ* mRad, struct frXYZ* mDeg );

void  subtract			( struct fXYZ* mFirst, struct fXYZ* mSecond, struct fXYZ* mResult );
void  add	 			( struct fXYZ* mFirst, struct fXYZ* mSecond, struct fXYZ* mResult );
float dot_product		( struct fXYZ* mA, struct fXYZ* mB	);
float magnitude			( struct fXYZ* mReadings 			);
void  v_product			( struct fXYZ* mA, struct fXYZ* mB, struct fXYZ* mResult );
void  v_divide			( struct fXYZ* mA, struct fXYZ* mB, struct fXYZ* mResult );
void  v_reciprocal  	( struct fXYZ* mInput, struct fXYZ* mResult 			 );
void  scale				( struct fXYZ* mA, float mScaler, struct fXYZ* mResult   );

struct fXYZ average			( struct fXYZ mRead1, struct fXYZ mRead2 );
struct fXYZ weighted_average( struct fXYZ mRead1, struct fXYZ mRead2, float w1, float w2 );
void weighted_average_comp  ( struct fXYZ *mRead1, struct fXYZ *mRead2, 
					 		  struct fXYZ *w1,     struct fXYZ *w2, struct fXYZ *sumW12,
					 		  struct fXYZ *mResult );


#ifdef  __cplusplus
}
#endif
