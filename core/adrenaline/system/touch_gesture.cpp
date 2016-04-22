#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <math.h>
#include <shapes.h>
#include <fontinfo.h>
#include <vector>

#include "VG/openvg.h"
#include "VG/vgu.h"
#include "bk_system_defs.h"
#include "Graphbase.hpp"
#include "control.hpp"
#include "button.hpp"
#include "display.h"
#include "text_view.hpp"
#include "touch_gesture.hpp"


#define Debug 0


DraggerGesture::DraggerGesture()
{  
	if (Debug) printf("DraggerGesture::DraggerGesture()\n");
	//Initialize();
}

/* this is a virtual function.  So when the base ctor gets called,
	it calls the base Initialize();  ie Mouse() -> Mouse::Initialize()
		Here the object type is Mouse (b/c its the mouse ctor)
	
	Then the DraggerGesture ctor runs.  And it calls Initialize()
	to get these vars set.
	
	So do not call the base class Intialize() here!  
	Especially for this class because the base class Init starts another thread!
*/
void DraggerGesture::Initialize()
{
	Mouse::Initialize();
	if (Debug) printf("DraggerGesture::Initialize() ...\n"); 
	m_enlarge_ratio 		  = 1.0;
	m_rotate_angle_relative	  = 0.;
	m_last_gesture_recognized = 0;
	m_test_results_bitfield	  = 0;
}

void DraggerGesture::handle_event( )
{
	Mouse::handle_event();
	recognize_gesture();
}

float distance( struct finger_info f1, struct finger_info f2 )
{
	float	distance = fabs(f1.x - f2.x) + fabs(f1.y - f2.y);
	return  distance;
}


// We only want to trigger this 1 time on the release!
bool DraggerGesture::recognize_tap ( )			// 1 finger distance between
{
	//static int init_x=0;
	//static int init_y=0;
	static int state = 1;
	switch(state)
	{
	// Start with no fingers down : 
	case 1: // Must be 1 finger down
			if ((mouse_ev.code == 57) && (mouse_ev.value != -1))
			{ 
				state = 2;
			} else state = 1;
			break;
	case 2:	// if mouse move abs
			// if release
			if ((mouse_ev.code == 57) && (mouse_ev.value == -1))
			{
				//printf("DraggerGesture::recognize_tap() released, going to state 1\n");
				state = 1;
				int size = m_finger_history[0].size();
				if (size > 1)
				{
					float delta_x = m_finger_history[0][0].x - m_finger_history[0][size-1].x;
					float delta_y = m_finger_history[0][0].y - m_finger_history[0][size-1].y;
					float sum     = fabs(delta_x) + fabs(delta_y);
					if (sum > MAX_MOVE_THRESHOLD)
					{
						return false;		// start over!
					}
					else {
						if (Debug) printf("TAP EVENT RECEIVED! %d \n", m_last_gesture_recognized);
						m_last_gesture_recognized = TOUCH_GESTURE_TAP;
						return true;
					}
				} else {
						if (Debug) printf("TAP EVENT RECEIVED!\n");
						m_last_gesture_recognized = TOUCH_GESTURE_TAP;
						return true;				
				}
			}
			break;
	default: if (Debug) printf("DraggerGesture::recognize_tap() default\n");
			break;
	}
	return false;
}


bool DraggerGesture::recognize_enlarge ( )
{ 
	static int state  =1;
	static float m_initial_distance = 0.0;
	static float distance2;
	
	switch(state)
	{
	case 0:	// Must be no fingers down.
			//if (m_num_fingers_down==0)
			//	state = 1;
			// else wait here.
			// break;
	case 1: // Must be 2 finger down.
			if (m_num_fingers_down==2)
			{
				if ((m_finger_history[0].size()>1) && (m_finger_history[1].size()>1))
				{
					m_initial_distance = distance( m_finger_history[0][0], m_finger_history[1][0] );
					state = 2;
				}
				// we dont know yet if it's going to be 2 finger slide or an enlarge.
			} 
			break;
	case 2:	// still 2 fingers down - moves change ratio:
			if (m_num_fingers_down==2)
			{
				int last  = m_finger_history[0].size()-1;
				distance2 = distance( m_finger_history[0][last], m_finger_history[1][last] );
				m_enlarge_ratio = (distance2 / m_initial_distance);
				
				if (m_last_gesture_recognized & TWO_ENLARGE)
				{
					//printf("recognize_enlarge:  init=%6.2f; final=%6.2f   Enlarge= %6.2f\n",
					//		 m_initial_distance, distance2, m_enlarge_ratio );
					return true;
				}
				else if ((distance2 < m_initial_distance*0.8) || 
					     (distance2 > m_initial_distance*1.2))
				{
					m_last_gesture_recognized |= TWO_ENLARGE;
					if (Debug) printf("ENLARGE EVENT RECEIVED:  Ratio=%6.2f\n", m_enlarge_ratio);
					return true;
				}
			} 
			else {
				if (Debug) printf("recognize_enlarge:  init=%6.2f; final=%6.2f   Enlarge= %6.2f\n",
						 m_initial_distance, distance2, m_enlarge_ratio );
				m_last_gesture_recognized &= ~TWO_ENLARGE;
				state = 1;			
			}
			break;
	case 3:	// Must be no fingers down.
			break;
	default:
			break;
	}
	return false;
}		

bool DraggerGesture::recognize_rotation( )
{

	static float m_initial_angle = 0.0;
	float dx = 0;
	float dy = 0;
	static float angle2 = 0;
	static int state = 1;
	switch(state)
	{
		case 1: // Must be 2 finger down.
			if (m_num_fingers_down==2)
			{
				if ((m_finger_history[0].size()>1) && (m_finger_history[1].size()>1))
				{
					dx = (m_finger_history[0][0].x - m_finger_history[1][0].x);
					dy = (m_finger_history[0][0].y - m_finger_history[1][0].y);
					m_initial_angle = degrees( atan2 ( dx , dy ) );
					state = 2;
				}
				// we dont know yet if it's going to be 2 finger slide or an enlarge.
			} 
			break;
	case 2:	// still 2 fingers down - moves change ratio:
			if (m_num_fingers_down==2)
			{
				int last  = m_finger_history[0].size()-1;
				dx = (m_finger_history[0][last].x - m_finger_history[1][last].x);
				dy = (m_finger_history[0][last].y - m_finger_history[1][last].y);
				angle2 = degrees( atan2 ( dx , dy ));
				m_rotate_angle_relative = (angle2 - m_initial_angle);
				if (m_last_gesture_recognized & TWO_ROTATE)
				{
					//printf("recognize_rotation:  initial=%6.2f;  final=%6.2f   Change=%6.2f\n", m_initial_angle, angle2, m_rotate_angle_relative );
				}   
				else if ((angle2 < m_initial_angle*0.8) || 
					     (angle2 > m_initial_angle*1.2))
				{
					m_last_gesture_recognized |= TWO_ROTATE;
					if (Debug) printf("ROTATE EVENT RECEIVED:  Angle=%6.2f\n", m_rotate_angle_relative);
					return true;
				}
			} 
			else {
				if (Debug) printf("recognize_rotation:  initial=%6.2f;  final=%6.2f   Change=%6.2f", 
								m_initial_angle, angle2, m_rotate_angle_relative );
				m_last_gesture_recognized &= ~TWO_ROTATE;
				state = 1;
			}
			break;
	default:
				break;
	}

	return false;
}

bool DraggerGesture::recognize_z_enlarge( )			// 3 fingers distance between thumb & fore
{
	static float init_dist_0_1 = 0.;
	static float init_dist_1_2 = 0.;
	static float init_dist_2_0 = 0.;
	//static int stationary_index_1 = 0;	// this is const dist to 
	//static int stationary_index_2 = 0;	// this one.
	//static int moving_index = 0;	// this one.
	
	if (m_num_fingers_down != 3) return false;
	// Calc Distance between all fingers:
	float dist_0_1 = distance( m_fingers[0], m_fingers[1] );
	float dist_1_2 = distance( m_fingers[1], m_fingers[2] );
	float dist_2_0 = distance( m_fingers[2], m_fingers[0] );
	// one distance should remain fairly constant.
	// the other will grow or shrink.
	//if (m_initial_measurement)
	{
		init_dist_0_1 = dist_0_1;
		init_dist_1_2 = dist_1_2;
		init_dist_2_0 = dist_2_0;	
	}
	//m_last_gesture_recognized = THREE_ENLARGE;
	return true;	
}

const int VERTICAL_MOVE_THRESHOLD = 25;


bool DraggerGesture::recognize_three_up_down( )
{ 
	float fractional_threshold = 0.2;
	bool  vertical = true;
	float average_vertical = 0;
	struct mouse_t mouse2;
	
	static int state = 1;
	switch(state)
	{
		case 1: // Must be 2 finger down.
				break;
		case 2: // Must be 2 finger down.
				break;
		default:
		
				break;		
	}
	// Find the vectors (first touch point to last touch point)
	float dx = (m_fingers[0].x - m_fingers[1].x);
	float dy = (m_fingers[0].y - m_fingers[1].y);	
	if (dx > (fractional_threshold*dy))
		vertical = false;
	average_vertical += dy;
	
	dx = (m_fingers[0].x - m_fingers[1].x);
	dy = (m_fingers[0].y - m_fingers[1].y);
	if (dx > (fractional_threshold*dy))
		vertical = false;
	average_vertical += dy;

	dx = (m_fingers[0].x - m_fingers[1].x);
	dy = (m_fingers[0].y - m_fingers[1].y);
	if (dx > (fractional_threshold*dy))
		vertical = false;
	average_vertical += dy;

	average_vertical /= 3.;
	
	int num_fingers = 3;
	if ((vertical) && (average_vertical > VERTICAL_MOVE_THRESHOLD) && (num_fingers==3)) 
	{
		m_last_gesture_recognized = average_vertical;
		if (average_vertical > 0)
			m_last_gesture_recognized = THREE_UP;		
		else
			m_last_gesture_recognized = THREE_DOWN;

		return m_last_gesture_recognized;
	}
	return false;
}

bool DraggerGesture::recognize_two_move_together( )
{ 
	if (m_num_fingers_down != 2) return false;
	
	m_last_gesture_recognized = THREE_TWO_MOVE;
	return false;
}	

bool DraggerGesture::recognize_three_move_together( )
{ 
	if (m_num_fingers_down != 3) return false;
	
	m_last_gesture_recognized = THREE_THREE_MOVE;
	return false;
}	

int pick_direction( float dx, float dy )
{
	int Direction = 0;
	if ((dx>dy) && (dx>0))	Direction = DIRECTION_RIGHT;
	if ((dx>dy) && (dx<0))	Direction = DIRECTION_LEFT;
	if ((dy>dx) && (dy>0))	Direction = DIRECTION_DOWN;
	if ((dy>dx) && (dy<0))	Direction = DIRECTION_UP;
	return Direction;
}
const int FLING_GAP_THRESHOLD = 20;	 //	pixels

float subtract_time_stamps( struct timeval time1, struct timeval time2)
{
	return 0.1;
}

/* Subtract the ‘struct timeval’ values X and Y,
   storing the result in RESULT.
   Return 1 if the difference is negative, otherwise 0. */
   
int  timeval_subtract( struct timeval *result, struct timeval *x, struct timeval *y )
{
  /* Perform the carry for the later subtraction by updating y. */
  if (x->tv_usec < y->tv_usec) {
    int nsec = (y->tv_usec - x->tv_usec) / 1000000 + 1;
    y->tv_usec -= 1000000 * nsec;
    y->tv_sec += nsec;
  }
  if (x->tv_usec - y->tv_usec > 1000000) {
    int nsec = (x->tv_usec - y->tv_usec) / 1000000;
    y->tv_usec += 1000000 * nsec;
    y->tv_sec -= nsec;
  }

  /* Compute the time remaining to wait.
     tv_usec is certainly positive. */
  result->tv_sec = x->tv_sec - y->tv_sec;
  result->tv_usec = x->tv_usec - y->tv_usec;

  /* Return 1 if result is negative. */
  return x->tv_sec < y->tv_sec;
}

bool DraggerGesture::flinger_one_finger( )
{ 
	float dx = 0;
	float dy = 0;
	float sum = 0;
	static int DirectionCount = 0;	// number of times in current direction Latches in that dir.
	static int Direction=0;
	static int state = 1;
	int last  = m_finger_history[0].size()-1;
	if (last>=1) {
		dx = m_finger_history[0][last].x - m_finger_history[0][last-1].x;
		dy = m_finger_history[0][last].y - m_finger_history[0][last-1].y;
	}
	Direction = pick_direction (dx, dy);
	float max_change = max( fabs(dx),fabs(dy) );
	
	switch(state)
	{
		case 1: 
			if (m_num_fingers_down==1)
			{
				if (last >= 1) 	//= b/c last is an index (not size)
				{
					m_direction = Direction;
					DirectionCount = 1;
					if ((dx > MAX_MOVE_THRESHOLD) || (dy > MAX_MOVE_THRESHOLD))
						state = 2;
				}
				// we dont know yet if it's going to be 2 finger slide or an enlarge.
			} else {
				m_last_gesture_recognized &= ~FLINGER_ONE_DRAG;
				m_last_gesture_recognized &= ~FLINGER_ONE_FLING;
			}
			//stay here.
			break;
		case 2:	// still 1 finger down - "DRAG MODE" slow/small changes.
			// Determine direction (north, south, east, west)
			//					   (up, down, left, right)
			if (m_num_fingers_down==1)
			{	
				m_drag_amount_dy = dy;
				m_drag_amount_dx = dx;
				if (max_change > FLING_GAP_THRESHOLD)
					state = 3;
				
				if ((mouse_ev.code == 57) && (mouse_ev.value == -1))
				{
					state = 1;
				}
				if (Direction == m_direction)	
					DirectionCount++;
				else
					DirectionCount--;
				if (DirectionCount < 0)
				{
					m_direction = Direction;
					DirectionCount = 0;
					//state = 1;
				}	
				if (m_last_gesture_recognized & FLINGER_ONE_DRAG)
				{
				}
				else
				{
					m_last_gesture_recognized |= FLINGER_ONE_DRAG;
					if (Debug) printf("SCROLL DRAG EVENT RECEIVED:  d<x,y>=<%6.2f,%6.2f>\n", 
								m_drag_amount_dx, m_drag_amount_dy );
					return true;
				}
			}
			else {
				//state = 1;
			}
			break;
		case 3:	// still 1 finger down - "FLING MODE" fast/large changes.
			if (m_num_fingers_down==1)
			{
				m_drag_amount_dy = dy;
				m_drag_amount_dx = dx;
				if (Debug) printf("SCROLL DRAG EVENT RECEIVED:  d<x,y>=<%6.2f,%6.2f>\n", 
								m_drag_amount_dx, m_drag_amount_dy );

				// The speed will be taken from the very last two coordinates received!
				// Released?				
				if (m_last_gesture_recognized & FLINGER_ONE_FLING)
				{ }
				else 
				{
					//m_last_gesture_recognized |= FLINGER_ONE_FLING;
					return true;
				}
			} 
			if ((mouse_ev.code == 57) && (mouse_ev.value == -1))
			{
				state = 1;		
				struct timeval time_delta;
				int reverse_time = timeval_subtract( &time_delta, &(mouse_ev.time), 
														&m_prev_ev_time );
				float time_delta_f = time_delta.tv_sec + (time_delta.tv_usec / 1000000.);
				m_fling_speed = (max_change / time_delta_f);		
									// pixels / second 
				m_last_gesture_recognized |= FLINGER_ONE_FLING;
				if (Debug) printf("SCROLL FLING RECEIVED: max=%6.3f deltaTime=%6.3f Speed=%6.2f\n", 
							max_change, time_delta_f, m_fling_speed);
			}
			break;
		default:
			break;
	}
	
	//m_last_gesture_recognized = THREE_THREE_MOVE;
	return false;
}	

void DraggerGesture::handle_recognize( )
{	
	if (m_last_gesture_recognized & TOUCH_GESTURE_TAP)
	{
		x    = m_finger_history[0][0].x;
		y    = max_y - m_finger_history[0][0].y;
		left = UNHANDLED_EVENT | (0x01);
		m_last_gesture_recognized &= ~TOUCH_GESTURE_TAP;
		if (Debug) printf("DraggerGesture::handle_recognize( )  left = %x\n", left );
	}  
//	else printf("DraggerGesture::handle_recognize()  %d %d \n", m_last_gesture_recognized,TOUCH_GESTURE_TAP );	
}

void DraggerGesture::recognize_gesture( )
{	
	// Run the gauntlet of tests:
	recognize_tap     ();
	recognize_enlarge ();
	recognize_rotation();
	flinger_one_finger();

	handle_recognize  ();

/*	recognize_z_enlarge();
	recognize_three_up_down();
	recognize_next_screen();
	recognize_gesture(); */
}

void DraggerGesture::add_event()
{ 

}

int	DraggerGesture::time_slice()
{
	m_test_results_bitfield = 0;
	recognize_gesture();

//	if (m_last_gesture_recognized & TOUCH_GESTURE_TAP)
//		m_last_gesture_recognized &= ~TOUCH_GESTURE_TAP;
	return Mouse::time_slice();	
}


/* Allowed directions are : Up, down, left, right.
	No diagonals.  
const int RIGHT_DIR = 1;
const int LEFT_DIR  = 1;
const int DOWN_DIR  = 1;
const int UP_DIR    = 1;
*/
