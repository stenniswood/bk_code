// though .hpp it is not OOP!

extern int  fd;

extern BOOL ShowAccelerometerData;
extern BOOL ShowGyroData 		;
extern BOOL ShowMagnetData	 	;
extern BOOL ShowCANData 		;
extern BOOL Classify 			;

extern int count_accel  ;
extern int count_gyro   ;
extern int count_magnet ;
extern int count_samples;
extern int UpdateDisplaySemaphore;

BOOL callback_board_presence( struct sCAN* mMsg );

BOOL callback_main			( struct sCAN* mMsg );
BOOL button_board_callback	( struct sCAN* mMsg );
BOOL motor_report_callback	( struct sCAN* mMsg );
BOOL callback_tilt_reading	( struct sCAN* mMsg );
BOOL callback_instance_catcher( struct sCAN* mMsg );

