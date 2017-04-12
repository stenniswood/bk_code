#ifndef _DRAGGER_GESTURE_HPP_
#define _DRAGGER_GESTURE_HPP_

//#include <vector>
#include "mouse_oop.hpp"
// class Mouse;


const int THREE_UP			= 0x001;
const int THREE_DOWN 		= 0x002;
const int TWO_ROTATE  		= 0x004;
const int TWO_ENLARGE 		= 0x008;
const int THREE_ENLARGE 	= 0x010;
const int THREE_NEXT_SCREEN = 0x020;		
const int THREE_TWO_MOVE 	= 0x040;		
const int THREE_THREE_MOVE 	= 0x080;
const int TOUCH_GESTURE_TAP	= 0x100;		
const int FLINGER_ONE_DRAG	= 0x201;
const int FLINGER_ONE_FLING	= 0x202;
const int DIRECTION_UP    	= 0x01;
const int DIRECTION_DOWN  	= 0x02;
const int DIRECTION_LEFT  	= 0x03;
const int DIRECTION_RIGHT 	= 0x04;

const float MAX_MOVE_THRESHOLD = 10.0;


class DraggerGesture : public Mouse
{
public: 
	DraggerGesture();

	virtual void	Initialize(); 	
	virtual int		time_slice();
	virtual void	handle_event		();			
	
	bool	recognize_tap	 	 ( );			// 1 finger distance between
	bool	recognize_enlarge 	 ( );			// 2 fingers distance between
	bool	recognize_z_enlarge  ( );			// 3 fingers distance between thumb & fore
	bool	recognize_rotation	 ( );			// 2 fingers change angles
	bool	recognize_three_up_down( );			// 2 finger left or right
	bool	recognize_next_screen( );		// 2 finger left or right
	bool	flinger_one_finger	 ( );
	void	handle_recognize     ( );
	
	void	recognize_gesture	 ( );

	bool	recognize_two_move_together  ( );
	bool	recognize_three_move_together( );
	
	void	add_event			 ( );

	// Enlarge/Rotate : 
	float	m_enlarge_ratio;	
	float	m_rotate_angle_relative;		// relative

	// "flinger_one_finger" : 
	int		m_direction;		// Algo selects one of 4.  Most dominant DIRECTION_UP, etc.
	float	m_drag_amount_dx;
	float	m_drag_amount_dy;	
	float	m_fling_speed;		// pixels / second

	int		m_last_gesture_recognized;	
	int		m_test_results_bitfield;	
};
//	std::vector<touch_event>	m_events;

#endif
