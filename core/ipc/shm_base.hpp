#ifndef _SHMBASE_HPP_
#define _SHMBASE_HPP_
#include "global.h"
#include "machine_defs.h"	// base path for segment_id filename.


/* This class will be the base for any shared memory 

*/
class SHMBase
{
public:
	SHMBase ( uint16_t mKey, size_t mSizeInBytes, char* mFilename );
	~SHMBase( );
	
	int  	connect_shared_memory( char mAllocate );
	bool 	is_IPC_memory_available();	
	bool 	is_poiner_valid		();		
	int  	allocate_memory		();
	void 	deallocate_memory	();

	void 	save_segment_id		();
	int  	read_segment_id		();
	
	int  	attach_memory		();
	void 	reattach_memory		();
	void 	detach_memory		();

	int  	get_segment_size	();
	void 	fill_memory			();
	void 	hex_dump			();
	
private:
	char* 		m_segment_filename;
	uint16_t	m_key;
	char* 		m_shared_memory;
	int 		m_segment_id;
	size_t		m_size;		// in bytes;
};


#endif
