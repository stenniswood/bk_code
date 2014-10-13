
#ifdef  __cplusplus
extern "C" {
#endif


// File Receive States:
#define BEGIN_OF_NEW_FILE 1
#define GET_FULL_PATHNAME BEGIN_OF_NEW_FILE
#define GET_FILE_NAME  2
#define GET_FILE_SIZE  3
#define GET_FILE_DATA  4
#define OPEN_FILE	   5
#define GET_FILE_BODY  6

// File Send States :
#define SEND_IDLE 		10
#define SEND_NEW_FILE  	11
#define SEND_ONE_CHUNK 	12
#define SEND_QUE_NEXT	13
#define SEND_ALL_DONE	14


#define UINT unsigned int
#define WORD unsigned short
#define BYTE unsigned char

extern BOOL	 hmi_terminate_requested;

BOOL  handle_hmi_data( );
void* hmi_server_thread(void*);



#ifdef  __cplusplus
}
#endif

