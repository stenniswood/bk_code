#ifdef  __cplusplus
extern "C" {
#endif


#define IPC_KEY_VIS 1234

extern char* 	visual_shared_memory;
extern int 		visual_segment_id;
extern struct   avisual_ipc_memory_map* ipc_memory_avis;

#define MAX_CLIENT_ARRAY_SIZE 2048

/******************** AVISUAL MEMORY MAP *****************/
struct avisual_ipc_memory_map
{
	long int StatusCounter;
	char	 ConnectionStatus[64];

	long int SentenceCounter;
	char	 Sentence        [128];	
	int		 NumberClients;
	char	 ClientArray[MAX_CLIENT_ARRAY_SIZE];		// String array (dimension of NumberClients)
	//short	 ScreenNumber;			// Which screen is being displayed.  voice commands can change.  Simplistic for now a single number per page.
	
};
/******************** AVISUAL MEMORY MAP *****************/
/*********************************************************/

void dump_ipc				();
void vis_save_segment_id	(char* mFilename);
int  read_segment_id		(char* mFilename);

int  vis_allocate_memory	();
void vis_deallocate_memory	(int msegment_id);


int  vis_attach_memory		();
void vis_reattach_memory	();
void vis_detach_memory		();

int  vis_get_segment_size();
void vis_fill_memory		();

void ipc_write_connection_status( char* mStatus   );
void ipc_write_command_text 	( char* mSentence );


#ifdef  __cplusplus
}
#endif
