#ifndef _APPENDAGE_HPP_
#define _APPENDAGE_HPP_

#include <vector>
using namespace std;


class Appendage
{
public:
	Appendage( );
	Appendage( int mActuators );

	void	Initialize				   ( );
	void 	print_current_positions	   ( );
	void	print_current_angles	   ( );
	
	int		find_actuator_instance	   ( byte mInstance 	);
	void	update_submitted_timestamps( struct timeval mts );

	BOOL 	is_destination_reached     ( );
	bool	vector_fully_read		   ( );		// at least 1 update on all enabled motors.
	void	set_new_destinations	   ( struct sVectorSet& mVectors, int mVectorIndex );

	void 	update_position			   ( );
	void 	set_current_position_as_destination( );	// should stop all motors.
	void 	compute_speeds			   ( );
	void 	send_speed_messages		   ( );
	void	send_moveto_angle_messages ( );
	void 	configure_motor_reports	   ( byte mRate, byte mReports );

	// CHANGE FREQUENTLY (ie realtime) :
	word 	Reads;				// number of CAN positioning reads done since duty was last updated
	word 	ReadsAllowed;		// number of CAN positioning reads before duty will be updated
	byte  	ElementsFilled;		// Bitmap. Use this each time a CAN ID_MOTOR_ANGLE msg comes in to indicate we have a new reading of the element.  Up to 8 elements

	vector<Motor> actuators;

	// Semi Constant Variables (change very infrequently)
	bool 	Enable;	// If FALSE, does not participate in the sequencing
	char	Name[40];
	
private:

};


#endif
