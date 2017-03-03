#include <stdio.h> 
#include <stdlib.h>
#include <sys/stat.h>
#include <string>
#include <errno.h>

// #if (PLATFORM==RPI)
#include <string.h>
// #endif
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>

#include "shm_base.hpp"

#define Debug 0


SHMBase::SHMBase(uint16_t mKey, size_t mSizeInBytes, char* mFilename)
:m_key(mKey)
{
	m_size = mSizeInBytes;
	m_segment_filename = strdup( mFilename );
	read_segment_id();
}

SHMBase::~SHMBase()
{
}

void SHMBase::hex_dump()
{
	int row = 0;
	for (size_t i=0; i<m_size; i++)
	{
		if ((i%32)==0)
		{	
			row++;	printf("\n"); 	
			printf("%d,%5d:%2x ", row, row*32, m_shared_memory[i] );
		}		
		printf("%2x ", m_shared_memory[i] );
	}
}

bool SHMBase::is_IPC_memory_available()
{
	struct shmid_ds buf;			// shm data descriptor.
	int retval = shmctl(m_segment_id, IPC_STAT, &buf);
	if (retval==-1) {
		perror("is_IPC_memory_available : " );
		return false;
	}
	Dprintf( "Found segment, size=%d and %ld attachments.\n", buf.shm_segsz, buf.shm_nattch );
	
	if ((buf.shm_segsz > 0)			// segment size > 0
	    && (buf.shm_nattch >= 1))	// number of attachments.
		return true;
	return false;
}

bool SHMBase::is_poiner_valid()
{
	if ((m_shared_memory != (char*)-1) && (m_shared_memory != NULL))
		return true;
	return false;	
}

int  SHMBase::connect_shared_memory( char mAllocate )
{
	bool available = is_IPC_memory_available();
 
	if ((!available) && (mAllocate))
	{
		int result = allocate_memory( );
		if (result == -1)	{
			Dprintf("Cannot allocate shared memory!\n");
		}
		attach_memory( );
		fill_memory  ( );				
		
		Dprintf("Saving segment id: ");
		save_segment_id( );		
		return 1;
	}
	 else  
	{
		attach_memory();	
		if (is_poiner_valid()==true)
			return 1;			
	}
	return 0;
}
	
void SHMBase::save_segment_id	()
{
	char fn[1024];
	strcpy( fn, shared_mem_ids_base_path);
	strcat( fn, m_segment_filename );
	FILE* fd = fopen(fn, "w");
	if (fd==NULL) {
		printf("Cannot open file for writing %s. %s \n", fn, strerror(errno) );		
		return;
	}
	Dprintf("Segment_id=%d\n", m_segment_id );
	fprintf( fd, "%d", m_segment_id );
	fclose( fd );
}

int SHMBase::read_segment_id()
{
	char fn[1024];
	strcpy( fn, shared_mem_ids_base_path);
	strcat( fn, m_segment_filename );	
	FILE* fd = fopen( fn, "r" );
	if (fd==NULL)  {
		printf("read_segment_id - ERROR: %s \n", strerror(errno) );	
		return -1;
	}
	fscanf( fd, "%d", &m_segment_id );
	Dprintf("Segment_id=%d\n", m_segment_id );
	fclose( fd );
	return m_segment_id;
}
	
int  SHMBase::allocate_memory	()
{
	Dprintf ("SHMBase:: key=%x; shm seg size=%d\n", m_key, m_size );

	/* Allocate a shared memory segment. */
	m_segment_id = shmget( m_key, m_size, IPC_CREAT | 0666 );
	if (m_segment_id==-1)
		perror("SHMBase::allocate_memory() shmget " );
	else
		printf ("SHMBase:: segment_id=%d\n", m_segment_id );
	return m_segment_id;
}

void SHMBase::deallocate_memory()
{
	/* Deallocate the shared memory segment. */ 
	int result = shmctl (m_segment_id, IPC_RMID, 0);
	if (result==-1)
		perror("SHMBase::deallocate_memory() ");	
}

int SHMBase::attach_memory		()
{
	/* Attach the shared memory segment. */
	m_shared_memory = (char*) shmat (m_segment_id, 0, 0);
	if (m_shared_memory==(char*)-1) {
		Dprintf("cli_attach_memory - Error: %s \n", strerror(errno) );
		return 1;
	} else 
		Dprintf ("Client shm attached at address %p\n", m_shared_memory); 	
	return 0;
}

void SHMBase::reattach_memory	()
{
	/* Reattach the shared memory segment, at a different address. */ 
	m_shared_memory = (char*) shmat (m_segment_id, (void*) 0x5000000, 0); 
	if (m_shared_memory==(char*)-1) {
		Dprintf("SHMBase::reattach_memory - ERROR: %s \n", strerror(errno) );
		return ;
	} 
	printf ("shm reattached at address %p\n", m_shared_memory); 
}

void SHMBase::detach_memory		()
{
	/* Detach the shared memory segment. */
	shmdt (m_shared_memory);
}

/* Should be identical to m_size! */
int  SHMBase::get_segment_size	()
{
	/* Determine the segment’s size. */
	struct 		shmid_ds	shmbuffer;
	shmctl (m_segment_id, IPC_STAT, &shmbuffer);
	int segment_size = shmbuffer.shm_segsz;
	printf ("segment size: %d\n", segment_size);
	return segment_size;
}

void SHMBase::fill_memory		()
{
	Dprintf("SHMBase::fill_memory() - ");
	memset(m_shared_memory, 0, m_size);
}

