
#include <unistd.h> // required for I2C device access and usleep()
#include <fcntl.h>  // required for I2C device configuration
#include <sys/ioctl.h> // required for I2C device usage
#include <linux/i2c-dev.h> // required for constant definitions
#include <stdio.h>  // required for printf statements

#include <iostream>     // std::cout
#include <fstream>      // std::ifstream
#include <math.h>

#include "servo.hpp"
#include "rcServo.hpp"
#include "dfServo.hpp"
#include "limb.hpp"


Limb::Limb()
{

}

Limb::~Limb()
{}
	
static std::string delimiter = ",";

void	Limb::setup_start_times()
{
	int num_servs = m_servos.size();
	printf( "%s has %d servos\n", m_limb_name.c_str(),  num_servs );
	for (int s=0; s<num_servs; s++)
	{
		m_servos[s]->setup_hawrdware();
	}
}

void	Limb::load_limb_structure( std::ifstream& mFile )
{	
	std::string   oneLine;

	// EXTRACT # Servos for this limb.
	std::getline( mFile, oneLine );				
	std::string num_servos_str = oneLine.substr( 0, 1 );
	int num_servos = atoi( num_servos_str.c_str() );	
	//printf("Limb::load_limb_structure has %d servos\n", num_servos);
	
	// LOAD CALIBRATION INFO FOR EACH SERVO:	
	for (int i=0; i<num_servos; i++)
	{
		// EXTRACT MOTOR TYPE (ie. rc or df):
		std::getline( mFile, oneLine );		// Get Servo parameter description line.
		std::string servo_type = oneLine.substr( 0,2 );		
		int pos = 0;
		
		if (servo_type.compare("rc")==0 )
		{
			int rc_num = 0;
			pos = oneLine.find(',');			    
			if (pos != std::string::npos) 
			{			
				// EXTRACT Servo #
				std::string rc_num_str = oneLine.substr( 2, pos );
				rc_num = atoi( rc_num_str.c_str() );				
			}		
			Servo* tmp = new rcServo( rc_num );
			
			//printf("Creating Servo id=%d\n", rc_num);
			
			// PROCESS REMAINDER OF LINE :
			std::string subLine = oneLine.substr( pos+1, -1 );
			//printf("pos=%d; subLine=%s;  rc_num=%d\n", pos, subLine.c_str(), rc_num );
			tmp->parse_cal_line( subLine.c_str() );
			m_servos.push_back( tmp );	
			// DEBUG:
			//(m_servos.back())->print_parameters();			
		}
		else
		{	
			printf("New DF motor.\n");  
			/*Servo* tmp = new dfServo('w');
			oneLine = oneLine.substr( pos, -1 );
			tmp->parse_cal_line( oneLine.c_str() );
			m_servos.push_back( tmp );			*/
		}
	}
}

void	Limb::print_parameters()
{
	int num_servs = m_servos.size();
	printf("%s has %d servos\n", m_limb_name.c_str(),  num_servs);
	
	for (int s=0; s<num_servs; s++)
	{
		m_servos[s]->print_parameters();
	}
}

void	Limb::load_limb_positions   ( std::ifstream& fd )
{
	for (int s=0; s<m_servos.size(); s++)
	{
		m_servos[s]->load_current_position( fd );		
	}	
}

void	Limb::save_limb_positions   ( FILE* fd )
{
	for (int s=0; s<m_servos.size(); s++)
	{
		m_servos[s]->save_current_position( fd );
	}	
}

void	Limb::save_limb_calibrations( FILE* fd )
{
	char tmp[256];
	sprintf(tmp, "%d servos\n", m_servos.size() );
	size_t len = strlen( tmp );
	fwrite(tmp, 1, len, fd );
	
	for (int s=0; s<m_servos.size(); s++)
	{
		m_servos[s]->save_cal_file_line( fd );
	}
}

void	Limb::actuate_vector( std::vector<float> mDegVector )
{
	size_t len = m_servos.size();
	
	for (int s=0; s < len; s++)
	{
		//printf("actuating Limb servo[%d]=%6.3f \n", s, mDegVector[s] );		
		m_servos[s]->angle_to( mDegVector[s] );
	}
}

void	Limb::actuate_vector( std::vector<unsigned short> mCountVector )
{
	size_t len = m_servos.size();
//    printf("Limb::actuate_vector(counts).  servs:%d %d \n",len, mCountVector.size() );
   	
	for (int s=0; s < len; s++)
	{		
		m_servos[s]->count_to( mCountVector[s] );
//		printf("get_counts() : %d \n", m_servos[s]->get_counts() );
	}
}


void Limb::actuate_centers(  )
{
	size_t len = m_servos.size();	
	for (int s=0; s < len; s++)
	{
		m_servos[s]->to_center( );
	}
}

void Limb::set_name( std::string mName )
{
	m_limb_name = mName;
}

/* 
   First line should start with the servo Number.

   Note:  - Servo number corresponds directly to the PI Servo Hat numbers!
   		  - Servo numbers are therefore 0 indexed!
*/
/*int read_cal_file( std::ifstream mfile ) 
{ 
	servs_used = 0;
	
    ///Already opened in Robot.cpp    
    std::string str; 
	int i=0;
	unsigned char servo_num = -1;
    while (std::getline(mfile, str))
    {
		size_t pos = 0;
		std::string token;
		
		// Extract Servo Num:
		if ((pos = str.find(delimiter)) != std::string::npos) 
		{
				token = str.substr(0, pos);
				str.erase(0, pos + delimiter.length());
				servo_num = std::stoi( token );
				//calfile_lines[servo_num] = str.c_str();
		}	
    }
    //print_calfile_info();
    return 0;
}*/
/*********************************************************************/




/*********************************************************************/
/* Note:  First servo is the hip.
		  Second servo is the knee.
*****************************************/
const float UpperLegLength = 9.0;	// inches
const float LowerLegLength = 9.0;	// inches

Leg::Leg()
{

}
Leg::~Leg()
{
}
/*** These are for fixed Hip height ***/
const float HipHeight = 21.0;

float	Leg::compute_knee_height()
{
	// assuming waist is horizontal.
	
	float hip_deg = m_servos[0]->get_angle();	
	float hip_rad = hip_deg * M_PI/180.0;
	
	float vert_projection = UpperLegLength * cos( hip_rad );
	float retval = (HipHeight - vert_projection);
	printf("UpperLeg vertical Projection = %6.2f\n", vert_projection);
	return retval;
}

float	Leg::compute_ankle_height()
{
	float knee_height = compute_knee_height();
	
	float hip_angle = m_servos[0]->get_angle();	
	
	float knee_deg_wrt_gravity = hip_angle + m_servos[1]->get_angle();
	float knee_rad_wrt_gravity = knee_deg_wrt_gravity * M_PI/180.0;

	float vert_projection = LowerLegLength * cos( knee_rad_wrt_gravity );
	float retval = (knee_height - vert_projection);
	printf("LowerLeg vertical Projection = %6.2f\n", vert_projection);
	return retval;
	
}
float	Leg::compute_toe_height()
{
	return 0;
}


float	Leg::compute_knee_forward()
{
	float hip_deg = m_servos[0]->get_angle();	
	float hip_rad = hip_deg * M_PI/180.0;
	
	float forward_projection = UpperLegLength * sin( hip_rad );
	float retval = (forward_projection);
	printf("UpperLeg forward Projection = %6.2f\n", forward_projection);
	return retval;

}
float	Leg::compute_ankle_forward()
{
	float knee_forward = compute_knee_forward();
	
	float hip_angle = m_servos[0]->get_angle();		
	float knee_deg_wrt_gravity = hip_angle + m_servos[1]->get_angle();	
	float knee_rad_wrt_gravity = knee_deg_wrt_gravity * M_PI/180.0;
	
	float forward_projection = LowerLegLength * sin( knee_rad_wrt_gravity );
	float retval = (knee_forward + forward_projection);
	printf("LowerLeg forward Projection = %6.2f\n", retval);
	return retval;
}

float	Leg::compute_heel_forward()
{
   return 0.0;
}

/* returns the angle needed to make foot level with floor */
float	Leg::make_foot_horizontal()
{
	float hip_angle = m_servos[0]->get_angle();		
	float knee_deg_wrt_gravity = hip_angle + m_servos[1]->get_angle();	
	float knee_rad_wrt_gravity = knee_deg_wrt_gravity * M_PI/180.0;

	float foot_ankle_angle_rad = -knee_rad_wrt_gravity;
	return foot_ankle_angle_rad;
}

