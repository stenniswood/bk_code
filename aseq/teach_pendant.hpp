#ifndef _TEACH_PENDANT_HPP_
#define _TEACH_PENDANT_HPP_

#include "preferences.hpp"

// Standard RadioShack Pots give 300 degree range.  
// BigMotorBoard Firmware gives 1024 counts.
#ifndef CountsPerDegree
//const float CountsPerDegree = 1024. / 300.;
//const float DegreesPerCount = 300. / 1024.;
#endif

float radians(float mDegrees);


struct DialInfo 
{
	int		index;				// first byte of the ID_ANALOG_MEASUREMENTS msg.
	int		CurrCount;
	int		PrevCount;
	int		ZeroOffset;
	long	SumCurrCounts;		// for averaging.
	int		Missing_Msg;	
	char	Name[40];
};


class TeachPendant
{
public:
	TeachPendant();
	void	Initialize				( 								);
	void 	print					( );

	int		matches_any_dial		( int mIndex 					);
	void	add_dial				( int mIndex, char* mName		);
	bool	read_config_data		( Preferences& mprefs, int mIndex );	// reads indices, names, 
	int  	handle_CAN_message		( struct sCAN* mMsg 			);		// handles incoming msg

	/*** VARIABLES ***/
	byte 	m_Instance;					// Analog board instance.	     
	int		m_CAN_msg_id;			// ID_ANALOG_MEASUREMENT or ID_MOTOR_VALUE
	vector<struct DialInfo>	 m_dials;
	bool	m_is_connected;
	
};


#endif
