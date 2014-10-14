
#ifdef  __cplusplus
extern "C" {
#endif


// See visual_memory.h for structure definition.  
//   the file is shared between client and server!



char* get_connection_status			();
BOOL  is_new_connection_status		();

char* get_sentence					();
BOOL  is_new_sentence				();
int   read_sentence_counter			();

int   connect_shared_abkInstant_memory();


#ifdef  __cplusplus
}
#endif

