



SHMBase::SHMBase(uint16_t mKey)
:m_key(mKey)
{
}

SHMBase::~SHMBase()
{
}

BOOL SHMBase::is_IPC_memory_available()
{

}
BOOL SHMBase::is_poiner_valid()
{
	if ((ipc_memory_client != (struct client_ipc_memory_map*)-1) && (ipc_memory_client != NULL))
		return TRUE;
	return FALSE;	
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
		save_segment_id( m_segment_id_filename );		

			return 1;
	}
	 else  
	{
		cli_attach_memory();	
		if ((ipc_memory_client!=(struct client_ipc_memory_map*)-1) && (ipc_memory_client != NULL))
			return 1;			
	}
	return 0;
}
	
void SHMBase::save_segment_id	(char* mFilename)
{
	FILE* fd = fopen(mFilename, "w");
	Dprintf("Segment_id=%d\n", m_segment_id );
	fprintf( fd, "%d", m_segment_id );
	fclose( fd );
}

int SHMBase::read_segment_id	(char* mFilename)
{
	FILE* fd = fopen( mFilename, "r" );
	fscanf( fd, "%d", &m_segment_id );
	fclose( fd );
	return m_segment_id;
}
	
int  SHMBase::allocate_memory	()
{
	const int 	shared_segment_size = sizeof(struct avisual_ipc_memory_map);
	Dprintf ("Visual shm seg size=%d\n", shared_segment_size );
	
	/* Allocate a shared memory segment. */
	visual_segment_id = shmget( IPC_KEY_VIS, shared_segment_size, IPC_CREAT | 0666 );
	int errsv = errno;
	if (visual_segment_id==-1)
		printf("vis_allocate_memory() - shmget ERROR: %s \n", strerror(errsv) );
	else 
		printf ("Visual shm segment_id=%d\n", m_segment_id );
	return visual_segment_id;	
}

void SHMBase::deallocate_memory	(int msegment_id)
{
}

	
void SHMBase::attach_memory		()
{
	/* Attach the shared memory segment. */
	m_shared_memory = (char*) shmat (m_segment_id, 0, 0);
	ipc_memory			 = (struct avisual_ipc_memory_map*)m_shared_memory;
	printf ("Visual shm attached at address %p\n", m_shared_memory); 	

}

void SHMBase::reattach_memory	()
{
	/* Reattach the shared memory segment, at a different address. */ 
	m_shared_memory = (char*) shmat (m_segment_id, (void*) 0x5000000, 0); 
	ipc_memory			 = (struct avisual_ipc_memory_map*)m_shared_memory;	
	printf ("Visual shm reattached at address %p\n", m_shared_memory); 
}

void SHMBase::detach_memory		()
{
	/* Detach the shared memory segment. */
	shmdt (m_shared_memory);
}


int  SHMBase::get_segment_size	()
{
	struct 		shmid_ds	shmbuffer;
	/* Determine the segment’s size. */
	shmctl (visual_segment_id, IPC_STAT, &shmbuffer);
	int segment_size = shmbuffer.shm_segsz;
	printf ("Visual segment size: %d\n", segment_size);
	return segment_size;
}

void SHMBase::fill_memory		()
{
	memset(m_shared_memory, 0, vis_get_segment_size());
}

