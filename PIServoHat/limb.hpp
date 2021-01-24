#ifndef _LIMB_HPP_
#define _LIMB_HPP_

#include <vector>
#include <string>
#include "servo.hpp"
#include "rcServo.hpp"
#include "dfServo.hpp"

class Limb
{
public:
	Limb();
	~Limb();

	void	setup_start_times();	
	void	load_limb_structure   ( std::ifstream& mFile );
	void	save_limb_calibrations( FILE* fd );
	void	save_limb_positions   ( FILE* fd );
	void	load_limb_positions   ( std::ifstream& fd );		
	
	int		get_num_servos()   				  { return m_servos.size(); 		};
	Servo*	get_servo_handle(int mServoIndex) { return m_servos[mServoIndex]; 	};
	
	void	set_name( std::string mName );	

	void	actuate_vector( std::vector<float> mDegVector );
	void	actuate_vector( std::vector<unsigned short> mCountVector );	
	void	actuate_centers(  );	
	
	void	print_parameters();

	
protected:
	std::vector<Servo*> m_servos;
	
private:
	std::string	m_limb_name;
};


class Leg : public Limb
{
public:
	Leg();
	~Leg();

	
	float	compute_knee_height();	
	float	compute_ankle_height();

	float	compute_heel_forward();	
	float 	compute_knee_forward();
	float	compute_ankle_forward();
	
	float	compute_toe_height();		// not implemented yet.
	float	make_foot_horizontal();	
	
	
private:


};

class Arm2 : public Limb
{
public:

private:


};

#endif

