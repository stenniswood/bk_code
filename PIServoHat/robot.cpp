#include <pthread.h> 
#include "robot.hpp"


extern pthread_mutex_t lock; 


Robot::Robot( std::string mFilename  )
:device(0x68)
{
	active_vector_played = false;
	device.calc_yaw = true;

	read_setup_file(mFilename);

	read_servo_positions( "LastCounts.csv" );		
}

void 	Robot::read_vector_file( std::string mSequenceName )
{
	// LOAD all basic Robot Moves:
	VectorSequence* vecs = new VectorSequence(mSequenceName);
	vecs->open_file();
	vecs->read_file();
	m_moves.push_back( vecs );
	m_selected_move = 0;
	
	goto_first_sequence();
	printf("read_vector_file() done\n");
}

void Robot::setup_start_times( )
{
pthread_mutex_lock(&lock); 
	size_t len = m_limbs.size();
	for (int l=0; l<len; l++)
	{
		m_limbs[l].setup_start_times();
	}	
pthread_mutex_unlock(&lock); 	
}

Robot::~Robot()
{

}

void  Robot::print_robot_setup()
{
	size_t len = m_limbs.size();
	
	printf("\n======= Robot Configuration : %d limbs ========\n", len );
	for (int l=0; l<len; l++)
	{
		m_limbs[l].print_parameters();
	}
	printf("==================================================\n");
}

void	Robot::stand_still()
{

}

int Robot::play_active_vector( )
{	
	bool cmd = m_moves[m_selected_move]->get_is_command();
	if (cmd)
	{
		// Skip.  Commands are 1 liners : 
		printf("Command found executing \n");
		m_moves[m_selected_move]->execute_command();
		return 0;		
	} else {
		printf("Vector found actuating \n");
		struct stFloatVector newVec = m_moves[m_selected_move]->get_vector_package( );
		newVec.print();
			struct stCountVector vc;
			convert_to_counts( newVec, vc );
			vc.print();
		
		actuate_vector( newVec );	
		return 1;
	}		
	active_vector_played = true;	
}

void Robot::play_group(  )
{
	m_moves[m_selected_move]->next_sequence( );	
	int gs = m_moves[m_selected_move]->get_group_size();
	printf("group size=%d\n", gs );
	
	play_n( gs );
}

void Robot::play_n( int mNumVecsToPlay )
{
	int num_vectors_actuated = 0;
	do
	{	
		int va = Robot::play_active_vector( );		
		num_vectors_actuated++;
		printf("PROCESSED LINES = %d / %d\n", num_vectors_actuated, mNumVecsToPlay);

		if ((num_vectors_actuated < mNumVecsToPlay))
			m_moves[m_selected_move]->next_sequence( );	
	
	} while (num_vectors_actuated < mNumVecsToPlay);	
}

void Robot::convert_to_counts ( struct stFloatVector& mDegVector, struct stCountVector& mCountVector )
{
	mCountVector.limb_num = mDegVector.limb_num;
	m_limbs[mDegVector.limb_num].convert_to_counts( mDegVector.m_angles, mCountVector.m_counts );
}
void Robot::convert_to_degrees( struct stCountVector& mCountVector,  struct stFloatVector& mDegVector )
{
	mCountVector.limb_num = mDegVector.limb_num;
	m_limbs[mCountVector.limb_num].convert_to_degrees( mCountVector.m_counts, mDegVector.m_angles );
}

void Robot::actuate_vector( struct stFloatVector mVec )
{
	//printf("Robot::actuate_vector(angles).  Limb:%d\n", mVec.limb_num );
	m_limbs[mVec.limb_num].actuate_vector( mVec.m_angles );
}
void Robot::actuate_vector( struct stCountVector mVec )
{
	printf("Robot::actuate_vector(counts).  Limb:%d\n", mVec.limb_num );	
	m_limbs[mVec.limb_num].actuate_vector( mVec.m_counts );
}
	
void  Robot::goto_first_sequence()
{
	m_moves[m_selected_move]->goto_first_sequence();
	m_moves[m_selected_move]->process_one_vector( );
	active_vector_played = false;
}

void  Robot::prev_sequence()
{	
	m_moves[m_selected_move]->prev_sequence();
	active_vector_played = false;
}

void  Robot::next_sequence()
{
	m_moves[m_selected_move]->next_sequence();	
	active_vector_played = false;
}


void Robot::select_move( int mMoveNum )
{
	m_selected_move = mMoveNum;
}

void Robot::do_move(  )
{
}

void Robot::read_setup_file( std::string mSetup_file_name )
{
	printf("Reading Robot configuration file: %s\n", mSetup_file_name.c_str() );
	std::ifstream  file( mSetup_file_name );

	Limb tmp;
	m_limbs.clear();
	std::string oneLine;

	// EXTRACT # LIMBS:
	std::getline(file, oneLine);
	std::string num_limb_str = oneLine.substr( 0,1 );
	int num_limb = atoi( num_limb_str.c_str() );	
	//printf("Robot has %d limbs.\n", num_limb );

	for(int i=0; i<num_limb; i++)
	{
		// EXTRACT NAME of LIMBS (and add to the queue)
		std::getline(file, oneLine);
		//printf("\tLimb #%d = %s\n", i, oneLine.c_str() );
		tmp.set_name( oneLine );
		m_limbs.push_back( tmp );
	}
	
	// LOAD SERVOS FOR EACH LIMB:
	for(int i=0; i<num_limb; i++)
	{
		m_limbs[i].load_limb_structure( file );
	}	
}

void  Robot::read_servo_positions( std::string mCountsFileName )
{
	printf("LOAD LAST POSITIONS....%s\n", mCountsFileName.c_str() );
	
	std::ifstream  		file( mCountsFileName );
	//if (fd==0) perror("Error cannot open counts file.\n");
	
	for (int l=0; l<m_limbs.size(); l++)
	{
		m_limbs[l].load_limb_positions( file );
	}		
}
	
void Robot::save_servo_positions( std::string mCountsFileName )
{
	printf("SAVE_ POSITIONS....%s\n", mCountsFileName.c_str() );
	
	FILE* fd = fopen( mCountsFileName.c_str(), "wr+" );
	if (fd==0) perror("Error cannot open cal file for writing.\n");
	
	for (int l=0; l<m_limbs.size(); l++)
	{
		m_limbs[l].save_limb_positions( fd );
	}	
	fclose(fd);
}

void Robot::show_calibrations  ( )
{
	printf("\nC  : MIN MAX CENTER  D: MIN MAX CNTR \n" );
	
	for (int l=0; l<m_limbs.size(); l++)
	{
		m_limbs[l].print_parameters( );
	}
	printf("-------------------------------------------\n" );
}	

FILE* Robot::save_calibrations  ( std::string mFilename )
{
	printf("SAVE_CALIBRATIONS....%s\n", mFilename.c_str() );
	
	FILE* fd = fopen( mFilename.c_str(), "wr+" );
	if (fd==0) perror("Error cannot open cal file for writing.\n");
	
	for (int l=0; l<m_limbs.size(); l++)
	{
		m_limbs[l].save_limb_calibrations( fd );
	}
	fclose( fd );
	printf("\n");
	
	return NULL;
}

void Robot::actuate_centers()
{
	for (int l=0; l < m_limbs.size(); l++)
	{
		m_limbs[l].actuate_centers();
	}
}

Servo*	Robot::get_servo_handle( int mLimbIndex, int mServoIndex )
{
	return m_limbs[mLimbIndex].get_servo_handle( mServoIndex );
}

/* Just 1 sequence step */
void  Robot::play_one_interpolator_sequence(  )
{
	int num_limbs = m_limbs.size();
	
	struct stFloatVector vec;
	for (int l=0; l<num_limbs; l++)
	{
		interp.get_play_vector( l, vec );
		actuate_vector        ( vec    );
		interp.bump_sequence  ( );		
	}	
}

void Robot::play_full_interpolator_sequence( )
{
	do {
		play_one_interpolator_sequence();
		usleep( 10000 );
	} while (interp.has_completed() == false );
	interp.restart();
}

