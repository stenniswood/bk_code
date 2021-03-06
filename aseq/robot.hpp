#ifndef _ROBOT_HPP_
#define _ROBOT_HPP_

#include <vector>
using namespace std;




class Robot
{
public:
	Robot( );

	void	Initialize				   ( );
	void 	print_current_positions	   ( );
	void 	print_averages			   ( );	
	void	clear_reads				   ( int mNumExpected );
	bool	are_reads_completed		   ( );	
	void	start_measurement_averaging( int mNumSamples );
	bool	done_averaging			   ( );
	void	load_config				   ( char* mFilename );

	void	deactivate_outputs			( );	
	void	activate_outputs			( );	
	void	activate_enabled_outputs	( );	

	int  	handle_CAN_message	    	( struct sCAN* mMsg 	);	// handles incoming msg	
	int		find_actuator_by_instance  ( byte mInstance, int* Aindex, int* actuator_index );
	void	update_submitted_timestamps( struct timeval mts );

	BOOL 	is_destination_reached     ( );
	void	set_new_destinations	   ( sRobotVector* mSeq, float mTimeDelta );
	void	set_duty				   ( sRobotVector* mSeq, int mIndex, float mTimeDelta );

	void 	set_current_position_as_destination( );	// should stop all motors.
	void 	compute_speeds			   ( );
	void 	send_speed_messages		   ( );
	void	send_moveto_angle_messages ( );
	void 	configure_motor_reports	   ( byte mRate, byte mReports );

	// CHANGE FREQUENTLY (ie realtime) :
	word 	Reads;				// number of CAN positioning reads done since duty was last updated
	word 	ReadsAllowed;		// number of CAN positioning reads before duty will be updated

	vector<Appendage*> limbs;

	void	print_vector		( int mIndex, bool mAngles );
	void 	set_vectors_limbs	( );
	void	next_vector			( );
	sRobotVector	  seq;		// one sequence of moves 

	// Semi Constant Variables (change very infrequently)
	bool 	Enable;		// If FALSE, does not participate in the sequencing
	char	Name[40];
	
private:

};

extern Robot robot;


#endif
