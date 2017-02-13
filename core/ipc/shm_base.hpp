#ifndef _SHMBASE_H_
#define _SHMBASE_H_

/* This class will be the base for any shared memory 

*/

class SHMBase
{
public:
	SHMBase( uint16_t mKey );
	~SHMBase();

	int  connect_shared_memory( char mAllocate );
	BOOL is_IPC_memory_available();	
	BOOL is_poiner_valid();		
	int  allocate_memory	();
	void deallocate_memory	(int msegment_id);

	void save_segment_id	(char* mFilename);
	int  read_segment_id	(char* mFilename);
	
	void attach_memory		();
	void reattach_memory	();
	void detach_memory		();

	int  get_segment_size	();
	void fill_memory		();

private:
	uint16_t	m_key;
	char* 	m_shared_memory;
	int 	m_segment_id;
	struct  avisual_ipc_memory_map* ipc_memory=NULL;
};


#endif
