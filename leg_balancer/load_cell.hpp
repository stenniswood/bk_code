#ifndef _LOADCELL_HPP_
#define _LOADCELL_HPP_

#include "sserial.hpp"

extern SSerial loadcell_left;
extern SSerial loadcell_right;


struct stOneFoot 
{
	float front_left;
	float front_right;
	float back_left;
	float back_right;
};

float 	calc_excess_weight		( float mLarger, float mSmaller );
float 	calc_forward_pressure	( float mFront, float mBack );
float 	calc_backward_pressure	( float mFront, float mBack );

bool 	is_balanced			( float mRatio);
void 	setup_load_cell		( const char* mDeviceNameLeft, const char* mDeviceNameRight);

int 	read_data_left		( );
int 	read_data_right		( );

void 	load_cell_stop		(  );

/*
*/
#endif

