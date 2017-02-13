#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>

#include "VG/openvg.h"
#include "VG/vgu.h"
#include <fontinfo.h>
#include <shapes.h>

#include <fcntl.h>
#include <pthread.h>
#include "mouse_oop.hpp"
#include "display_manager.hpp"


static int 		mouse_fd;
static int 		quitState  = 0;
#define    	CUR_SIZ  16				// cursor size, pixels beyond centre dot

#define Debug 1
pthread_t inputThread_id;

//extern Mouse* mouse;
//extern DraggerGesture mouse; 
extern Mouse mouse;

// eventThread reads from the mouse input file
static void* eventThread(void *arg) 
{
	Dprintf("eventThread(): \n");
	char counter=-1;
	char dev_name[40];
	
	//Mouse* M = (Mouse*) arg;	
	Mouse* M = (Mouse*) &mouse;
	
	Dprintf("Mouse M=%p\n", M );
	Dprintf("Mouse mode=%d\n", M->mouse_mode );
	
	// Open mouse driver
//	if ((mouse_fd = open("/dev/input/mouse0", O_RDONLY)) < 0) {	
	//for (counter=9; counter>0; counter--)
	{
		sprintf( dev_name, "/dev/input/event%d", counter );		
		sprintf( dev_name, "/dev/input/mice" );		
		printf("Trying %s\n", dev_name );

		//sprintf( dev_name, "/dev/input/mouse%d", counter );		
		mouse_fd = open( dev_name, O_RDONLY );
		if (mouse_fd >= 0) {
			printf( "Opened Mouse:  %s\n", dev_name );
			//M->show_mouse();			
			//break;
			
		} 
		
		if (counter==0) {
			fprintf(stderr, "Error opening Mouse!\n");
			quitState = 1;
			return &quitState;
		}
	}

	M->x = M->max_x / 2;			   // Reset mouse
	M->y = M->max_y / 2;
	
	while (1) {
		M->m_prev_ev_time = M->mouse_ev.time;
		read(mouse_fd, &(M->mouse_ev), sizeof(struct input_event));
		M->print_event_info();
		//M->hide_mouse();
		M->handle_event();

		Dprintf("\n");
	}
}

void Mouse::print_event_info()
{
	if (Debug==0) return;

	if (mouse_ev.type == EV_SYN)
		printf("EV_SYN:");
	if (mouse_ev.type == EV_REL)
		printf("EV_REL:");
	if (mouse_ev.type == EV_ABS)
		printf("EV_ABS:");	
	if (mouse_ev.type == EV_KEY)
		printf("EV_KEY:");
	Dprintf("t=%d, c=%d, v=%d\t", mouse_ev.type, mouse_ev.code, mouse_ev.value );
}

void Mouse::handle_event()
{	
	if (mouse_ev.type == EV_REL) {
		if (mouse_ev.code == REL_X) {
			x += (VGfloat) mouse_ev.value;
			if (x < 0)		x = 0;
			if (x > max_x) 	x = max_x;				
		}
		if (mouse_ev.code == REL_Y) {	   //This ones goes backwards hence the minus
			y -= (VGfloat) mouse_ev.value;
			if (y < 0)		y = 0;
			if (y > max_y) 	y = max_y;
		}
	} else if (mouse_ev.type == EV_ABS) {
		extract_finger_info( );
	} else if (mouse_ev.type == EV_KEY) {
		//printf("Time Stamp:%d - type %d, code %d, value %d\n",
		//      mouse_ev.time.tv_usec,mouse_ev.type,mouse_ev.code,mouse_ev.value);
		if (mouse_ev.code == BTN_LEFT) {
			left = UNHANDLED_EVENT | (mouse_ev.value & 0x01);
//			restore_pixels();
			//if (Debug) 
			printf("Left button: evalue=%d\n", mouse_ev.value );
		}
		if (mouse_ev.code == BTN_RIGHT) {
			right = UNHANDLED_EVENT | (mouse_ev.value & 0x01);
			if (Debug) printf("Right button: value=%d\n", mouse_ev.value);
		}
	} else 	if (mouse_ev.type == EV_SYN) {
		for (int f=0; f<m_num_fingers_down; f++) {
			m_finger_history[f].push_back( m_fingers[f] );
			if (Debug) printf("xy %d = <%6.2f, %6.2f> %d 53:54 \t", f, m_fingers[f].x, m_fingers[f].y, m_finger_history[f].size() );		
		}
	}
	else
	{
		if (Debug) printf("Unknown type=%d; code=%d value=%d \n", mouse_ev.type, mouse_ev.code, mouse_ev.value);
	}	
}

// for multi-touch screens
void Mouse::extract_finger_info()
{	
	static int received = 0;	
	//printf("Absolute Touch event! Type=%d; Code=%d; value=%d\t", mouse_ev.type, mouse_ev.code, mouse_ev.value );
	switch(mouse_ev.code) 
	{
	case 0:	x = mouse_ev.value;
			//m_fingers[m_finger].x = mouse_ev.value;		
			received |= 0x01;
			break;
	case 1: y = max_y - mouse_ev.value;
			//m_fingers[m_finger].y = mouse_ev.value;
			received |= 0x02;
			break;
	case 24: //printf("Absolute Touch event! Code=%d; value=%d\t", mouse_ev.code, mouse_ev.value );
			m_fingers[m_finger].pressure = mouse_ev.value;
			if (Debug) printf (" Pressure = %d  %6.2f\n", mouse_ev.value, m_fingers[m_finger].pressure);
			break;
	case 53: /*  ABS_MT_POSITION_X  */
			m_fingers[m_finger].x = mouse_ev.value;		
			received |= 0x10;
			//printf("Absolute Touch event! Code=%d; value=%d %x\n", mouse_ev.code, mouse_ev.value, received ); 
			break;
	case 54: /*  ABS_MT_POSITION_Y  */
			m_fingers[m_finger].y = mouse_ev.value;			
			received |= 0x20;
			//printf("Absolute Touch event! Code=%d; value=%d %x\n", mouse_ev.code, mouse_ev.value, received ); 
			break;

	case 57: //printf("Absolute Touch event! Code=%d; value=%d\t", mouse_ev.code, mouse_ev.value );
			if (mouse_ev.value != -1) {
				if (Debug) printf(" Finger Down.");
				m_fingers[m_finger].state = 1;
				//left = (UNHANDLED_EVENT|(0x01));
				if (m_num_fingers_down==0) m_num_fingers_down = 1;
				for (int f=0; f<10; f++)
					m_finger_history[f].clear();
			} else {
				if (Debug) printf(" Finger Released!");
				m_fingers[m_finger].state = 0;
				//left = UNHANDLED_EVENT | (0);
				m_num_fingers_down = 0;
			}
			break; 
	case 47:// printf("Absolute Touch event! Code=%d; value=%d\t", mouse_ev.code, mouse_ev.value ); 
			/* ABS_MT_SLOT */	// multi-touch finger number.
			m_finger = mouse_ev.value;
			if ((m_finger+1) > m_num_fingers_down)
				m_num_fingers_down = m_finger+1;			
			if (Debug) printf(" Finger # %d/%d", mouse_ev.value, m_num_fingers_down);			
			break;			
	default: if (Debug) printf("Absolute Touch event! Code=%d; value=%d", mouse_ev.code, mouse_ev.value ); 
			break;			
	}	
	if ((received & 0x30)==0x30) {		
		received = (received & 0x0F);
	}
	//printf(" Fingers %d \n", m_num_fingers_down);
}

Mouse::Mouse()
{
	Dprintf("Mouse::Mouse()\n");
}
Mouse::~Mouse()
{
}

void Mouse::load_mouse_images( )
{
}

void Mouse::Initialize()
{ 
	if (Debug) printf("Mouse::Initialize() ...\n");
	
	m_finger 		= 0;
	m_icon_selected = 0;
	m_num_fingers_down = 0;
	mouse_mode 		= 0;

	// get these values from display manager.	
	x=0;  y=0;
	left=0;  middle=0; right=0;
	max_x=MainDisplay.screen_width; max_y =MainDisplay.screen_height;
	
	//int 	cur_sx, cur_sy, cur_w, cur_h;	// cursor location and dimensions
	cur_saved=0;						// amount of data saved in cursor image backup

	// Allocate memory for restoring mouse covered pixels:
	cursorx = max_x / 2;
	cursory = max_y / 2;
	cbsize = (CUR_SIZ * 2) + 1;
	CursorBuffer = vgCreateImage(VG_sABGR_8888, cbsize, cbsize, VG_IMAGE_QUALITY_BETTER);
	init_fingers();  

	Dprintf("Mouse this=%p\n", this );
	
	// CREATE A THREAD TO READ FROM DEVICE:
	int result = pthread_create( &inputThread_id, NULL, &eventThread, this );
	if (result != 0) {
		if (Debug) fprintf(stderr, "Unable to initialize the mouse\n");
	}
} 	

// saveCursor saves the pixels under the mouse cursor
void Mouse::save_pixels( int curx, int cury, int screen_width, int screen_height, int s) 
{
	int sx, sy, ex, ey;

	sx = curx - s;					   // horizontal 
	ex = curx + s;
	if (sx < 0)				sx = 0;
	if (ex > screen_width)	ex = screen_width;

	cur_sx = sx;
	cur_w  = ex - sx;
	sy = cury - s;					// vertical 
	ey = cury + s;
	if (sy < 0)				sy = 0;
	if (ey > screen_height)	ey = screen_height;

	cur_sy = sy;
	cur_h  = ey - sy;

	vgGetPixels(CursorBuffer, 0, 0, cur_sx, cur_sy, cur_w, cur_h);
	cur_saved = cur_w * cur_h;
}
void Mouse::save_mouse()
{
	save_pixels( cursorx, cursory, max_x, max_y, CUR_SIZ);
}

// restoreCursor restores the pixels under the mouse cursor
void Mouse::restore_pixels( )
{
	if (cur_saved) {
		vgSetPixels(cur_sx, cur_sy, CursorBuffer, 0, 0, cur_w, cur_h);
	}
}
void Mouse::hide_mouse()
{
	restore_pixels(); 
}

// circleCursor draws a translucent circle as the mouse cursor
void Mouse::circleCursor(int curx, int cury, int width, int height, int s) {
	Fill(255, 255, 0, 0.50);
	Circle(curx, cury, s);
	Fill(255, 0, 0, 1);
	Circle(curx, cury, 2);
}
// circleCursor draws a translucent circle as the mouse cursor
void Mouse::crosshairsCursor(int curx, int cury, int width, int height) 
{
	Fill(255, 255, 0, 0.50);
	Line(curx-width, cury, curx+width, cury );
	Fill(255, 0, 0, 1);
	Line(curx, cury-height, curx, cury+height );	
}
// for text entry:
void Mouse::carretCursor(int curx, int cury, int width, int height) 
{
	Fill(255, 255, 0, 0.50);
	float hat_length = width/3;
	Line(curx-hat_length, cury-height, curx+hat_length, cury-height );
	Line(curx-hat_length, cury+height, curx+hat_length, cury+height );
	Fill(255, 0, 0, 1);
	Line(curx, cury-height, curx, cury+height );	
}

void Mouse::init_fingers( )
{
	for (int f=0; f<10; f++)
	{
		m_fingers[f].finger_number = f;
		m_fingers[f].x = 0;
		m_fingers[f].y = 0;
		m_fingers[f].pressure = 0;		
		m_fingers[f].state = 0;
	}
}


int	Mouse::time_slice()
{
	if (x != cursorx || y != cursory) 	// if the mouse moved...
	{		
		hide_mouse();
		cursorx = x;		// save for comparison on next timeslice.
		cursory = y;
		show_mouse();
	}
	if (left & UNHANDLED_EVENT)
	{
		left &= (~UNHANDLED_EVENT);	// clear the flag.
		int retval = left;
		return (retval<<4);			// 1=> button down.  0=> button up
	}
	if (right & UNHANDLED_EVENT)
	{
		right &= (~UNHANDLED_EVENT);	// clear the flag.
		return ((right)<<4);			// 1=> button down.  0=> button up
	}
	return 0;
} 

void Mouse::close_fd()
{ 
	close(mouse_fd);
	restore_pixels();			   // not strictly necessary as display will be closed
	vgDestroyImage(CursorBuffer);			   // tidy up memory
	finish();					   	// Graphics cleanup
}

void Mouse::show_mouse()
{
	save_mouse();
	Dprintf("show_mouse:  mode=%d\n",mouse_mode);
	switch (mouse_mode)
	{
	//void draw_cursor(int curx, int cury, int width, int height, int s) 
	case 0 : circleCursor(cursorx, cursory, max_x, max_y, CUR_SIZ);		break;
	case 1 : crosshairsCursor(cursorx, cursory, max_x, max_y);			break;	
	case 2 : carretCursor(cursorx, cursory, max_x, max_y);				break;
	default: break;
	}
}


void Mouse::select_mouse_image(int mIndex)
{
	m_icon_selected = mIndex;
	//m_icons[mIndex];
}