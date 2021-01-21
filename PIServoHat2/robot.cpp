#include "robot.hpp"




Robot::Robot( std::string mFilename )
:file( mFilename ), device(0x68)
{
	m_setup_file_name = mFilename;
	read_setup_file();
	active_vector_played = false;
	device.calc_yaw = true;

	read_servo_positions( "LastCounts.csv" );
	
	
	// LOAD all basic Robot Moves:
	Vector* vecs = new Vector("walk.txt");
	vecs->open_file();
	vecs->read_file();
	m_moves.push_back( vecs );
	m_selected_move = 0;

	
	goto_first_sequence();
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

/*struct stOneCounts	Robot::get_limb_zero_vector_package( int mLimbNum )
{
	int NumServos   = m_limbs[mLimbNum].get_num_servos();
	return m_moves[0]->get_zeros_vector_package( mLimbNum, NumServos );
	
}*/

void	Robot::stand_still()
{

}

void Robot::play_active_vector( )
{
	if (active_vector_played)	return;
	
	bool activate = m_moves[m_selected_move]->process_one_vector( );
	if (activate)
	{
		struct stOneVector newVec = m_moves[m_selected_move]->get_vector_package( );	
		actuate_vector( newVec );	
	}
	active_vector_played = true;	
}

void Robot::actuate_vector( struct stOneVector mVec )
{
//	printf("Robot::actuate_vector(angles).  Limb:%d\n", mVec.limb_num );	
	m_limbs[mVec.limb_num].actuate_vector( mVec.m_angles );
}
void Robot::actuate_vector( struct stOneCounts mVec )
{
//	printf("Robot::actuate_vector(counts).  Limb:%d\n", mVec.limb_num );	
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

void Robot::read_setup_file()
{
	Limb tmp;
	m_limbs.clear();
	std::string oneLine;

	// EXTRACT # LIMBS:
	std::getline(file, oneLine);
	std::string num_limb_str = oneLine.substr( 0,1 );
	int num_limb = atoi( num_limb_str.c_str() );	
	//printf("Robot has %d limbs.\n", num_limb );
	
	// EXTRACT NAME of LIMBS (and add to the queue)
	for(int i=0; i<num_limb; i++)
	{
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
	
FILE* Robot::save_calibrations  ( std::string mFilename )
{
	printf("SAVE_CALIBRATINS....%s\n", mFilename.c_str() );
	
	FILE* fd = fopen( mFilename.c_str(), "wr+" );
	if (fd==0) perror("Error cannot open cal file for writing.\n");
	
	for (int l=0; l<m_limbs.size(); l++)
	{
		m_limbs[l].save_limb_calibrations( fd );
	}
	fclose( fd );
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

