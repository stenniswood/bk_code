
#include "shm_manager.hpp"


SHMManager::SHMManager ( )
{
}
SHMManager::~SHMManager( )
{
}

int  	SHMManager::connect_shared_memory( )
{
}
bool 	SHMManager::is_IPC_memory_available(int index)
{
}	
bool 	SHMManager::is_poiner_valid		(int index)
{
}		

void 	SHMManager::save_segment_ids	()
{
}
int  	SHMManager::read_segment_ids	()
{
}
int  	SHMManager::print_status		()
{
}
int  	SHMManager::print_details		()
{
}

int  	SHMManager::allocate_memory		()
{
}
void 	SHMManager::deallocate_memory	()
{
}	
int  	SHMManager::attach_memory		()
{
}
void 	SHMManager::reattach_memory		()
{
}
void 	SHMManager::detach_memory		()
{
}

SHMBase*	get_segment		( int index )
{
}
