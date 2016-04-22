// though .hpp it is not OOP!

extern int  fd;


extern int count_samples;
extern int UpdateDisplaySemaphore;

void tilt_sensor_update_gui();

BOOL callback_board_presence( struct sCAN* mMsg );

BOOL callback_main			( struct sCAN* mMsg );
BOOL button_board_callback	( struct sCAN* mMsg );
BOOL motor_report_callback	( struct sCAN* mMsg );
//BOOL callback_tilt_reading	( struct sCAN* mMsg );
BOOL callback_instance_catcher( struct sCAN* mMsg );

