/* This class will organize all shared memory segments
	Allocating/ Attaching at the same time.
	Keeping tabs on which are available.
	and shuting them down when done.
	
*/


#ifndef _SHM_MANAGER_HPP_
#define _SHM_MANAGER_HPP_

#include "global.h"
#include "machine_defs.h"	// base path for segment_id filename.


/* This class will be the base for any shared memory */
class SHMManager
{
public:
	SHMManager ( );
	~SHMManager( );
	
	int  	connect_shared_memory( );
	bool 	is_IPC_memory_available(int index);	
	bool 	is_poiner_valid		(int index);		
	
	void 	save_segment_ids	();
	int  	read_segment_ids	();
	int  	print_status		();
	int  	print_details		();
		
	int  	allocate_memory		();
	void 	deallocate_memory	();	
	int  	attach_memory		();
	void 	reattach_memory		();
	void 	detach_memory		();

	SHMBase*	get_segment		( int index );
	
private:
	std::vector<SHMBase*> 	m_segments;	
};


#endif
