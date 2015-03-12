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
	
	int		find_actuator_by_instance  ( byte mInstance, int* Aindex, int* actuator_index );
	void	update_submitted_timestamps( struct timeval mts );

	BOOL 	is_destination_reached     ( );
	void	set_new_destinations	   ( sRobotVector& mVectors );

	void 	set_current_position_as_destination( );	// should stop all motors.
	void 	compute_speeds			   ( );
	void 	send_speed_messages		   ( );
	void	send_moveto_angle_messages ( );
	void 	configure_motor_reports	   ( byte mRate, byte mReports );

	// CHANGE FREQUENTLY (ie realtime) :
	word 	Reads;				// number of CAN positioning reads done since duty was last updated
	word 	ReadsAllowed;		// number of CAN positioning reads before duty will be updated

	vector<Appendage> limbs;

	// Semi Constant Variables (change very infrequently)
	bool 	Enable;		// If FALSE, does not participate in the sequencing
	char	Name[40];
	
private:

};


#endif
