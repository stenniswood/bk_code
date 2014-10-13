#ifndef _SWAYCONFIG_HPP_
#define _SWAYCONFIG_HPP_

#include "preferences.hpp"



class SwayConfig : public Preferences
{
public:
	SwayConfig(char* mFileName = (char*)"segway.ini");

	void	load_file(char* mFileName = (char*)"segway.ini");
	byte  	use_default_config	  ();
	byte	get_LeftMotorInstance ();
	byte	get_RightMotorInstance();
	float	get_LeftAlpha		  (byte mBinNumber);
	float	get_RightAlpha		  (byte mBinNumber);
	void 	set_LeftAlpha		(byte mBinNumber, float mNewValue);
	void	set_RightAlpha		(byte mBinNumber, float mNewValue);

	byte	get_bin_number 		  (float angle );
	char* 	get_bin_name		  (float angle, char mLeftOrRight );

	char	get_Axis			  ();
	
	void	print_all_params();
	
private:
	// all keys held in key_table
	bool	loaded;
};

extern SwayConfig sc;

#endif
