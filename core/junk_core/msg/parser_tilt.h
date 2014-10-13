
#ifdef  __cplusplus
extern "C" {
#endif

#define SENSITIVITY_2g_mgPerDigit  1
#define SENSITIVITY_4g_mgPerDigit  2
#define SENSITIVITY_8g_mgPerDigit  4
#define SENSITIVITY_16g_mgPerDigit 12

void extract_xyz( struct sCAN* mMsg, struct tXYZ* mXYZ );
void print_raw	( struct sCAN* mMsg, struct fXYZ* mRaw, BOOL mShowCANmsg );

void parse_accel_msg	( struct sCAN* mMsg );
void parse_gyro_msg		( struct sCAN* mMsg );
void parse_magnet_msg	( struct sCAN* mMsg );


extern struct fXYZ RawxyzAccel ;
extern struct fXYZ RawxyzGyro  ;
extern struct fXYZ RawxyzMagnet;

#ifdef  __cplusplus
}
#endif
