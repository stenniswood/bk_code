#include <string>
#include <string.h>
#include "shm_manager.hpp"


void ensure_shm_ids_directory()
{
	char path[200];
	strcpy(path, shared_mem_ids_base_path );
	
	DIR* dir = opendir(path);
	if (dir) {
		printf("Making already exists: %s\n", path);
		closedir(dir);		
	} else if (ENOENT == errno) {
		printf("Making directory: %s\n", path);
		int retval = mkdir (path, S_IRWXU );
		if (retval==-1)
			perror("Cannot create folder for shared memory!\n");
	}
}


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
