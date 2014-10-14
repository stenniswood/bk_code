#ifdef  __cplusplus
extern "C" {
#endif

#define LCD_CMD_PAUSE 750


int proc_dev( int argc, char *argv[] );
int proc_led( int argc, char *argv[], byte first_param );
int proc_instance( int argc, char *argv[], byte first_param );
int proc_lcd( int argc, char *argv[], byte first_param );


int proc_mot( int argc, char *argv[], byte first_param );
int proc_vec( int argc, char *argv[], byte first_param );

#ifdef  __cplusplus
}
#endif
