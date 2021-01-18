#ifndef _CAL_MOTOR_HPP_
#define _CAL_MOTOR_HPP_

#include <vector>
#include <string>
#include "drive_five.h"


class CalMotor 
{
public:
	CalMotor();
	~CalMotor();
	
	void set_min_max_travel		( long min,   long max 	  );
	void set_cal_pt1     		( long count, float angle );
	void set_cal_pt2     		( long count, float angle );
	void set_counts_per_rev		( long counts 			  );
	void set_counts_per_degs	( long counts, float angle );
	void set_zero_offset		( long counts 			  );
	
	void disable_motor			();
	void set_map_info     		( struct stMap *mInfo       );
	void print_cal_data			( );

	long  convert_angle_count	( float angle );
	float convert_count_angle	( long count  );
			
	void send_pid_request		( float mAngle    );
	void send_pid_request		( long int mCount );	
	
	float	get_max_angle		();
	float	get_min_angle		();
	bool 	valid_range			( long int mCount );
	

	DriveFive*  df;
	
	// Acsessors:
	char*    get_joint();
	char*    get_alias();
	
private:
	bool	 disabled;
	bool	 reversed;		// software correction for swapped power leads.
	
	char     drive_letter;
	char     alias[3];
	char	 joint[40];
	char     board_name[20];	// "Left_arm"
	char     device_name[40];	// "/dev/ttyUSB0"
		
	long int min_travel;
	long int max_travel;
	long int counts_per_rev;	
	long int zero_offset;
	
	long int cal_pt1;
	float    cal_pt1A;
	long int cal_pt2;
	float    cal_pt2A;

};

extern std::vector<CalMotor*>  mots;

#endif
