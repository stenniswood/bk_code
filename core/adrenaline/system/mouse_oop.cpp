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

#define Debug 0
pthread_t inputThread_id;

//extern Mouse* mouse;
//extern DraggerGesture mouse; 
extern Mouse mouse;
signed char mBuff[4];

// eventThread reads from the mouse input file
static void* eventThread(void *arg) 
{
	Dprintf("eventThread(): \n");

	char dev_name[40];
	
	//Mouse* M = (Mouse*) arg;	
	Mouse* M = (Mouse*) &mouse;
	
	Dprintf("Mouse M=%p\n", M );
	Dprintf("Mouse mode=%d\n", M->mouse_mode );
	
	// Open mouse driver
	sprintf( dev_name, "/dev/input/mice" );
	mouse_fd = open( dev_name, O_RDONLY );
	if (mouse_fd >= 0) {
		printf( "Opened Mouse:  %s\n", dev_name );
	} else {
		fprintf(stderr, "Error opening Mouse!\n");
		quitState = 1;
		return &quitState;
	}

	M->x = M->max_x / 2;			   // Reset mouse
	M->y = M->max_y / 2;
	
	while (1) {
		M->m_prev_ev_time = M->mouse_ev.time;
		read( mouse_fd, mBuff, 3 );
		M->handle_event();
		//M->print_event_info();
		//M->hide_mouse();
		Dprintf("\n");
	}
}

void Mouse::print_event_info()
{
	if (Debug==0) return;

	Dprintf("lmr=<%d,%d,%d>\t", left, middle, right );
	int dx = (signed char)mBuff[1];
	int dy = mBuff[2];
	Dprintf("dx=%d, dy=%d\t", dx, dy );
	Dprintf("x=%6.1f, y=%6.1f\n", x,y );		
}

void Mouse::handle_event()
{	
	static int pleft=0;
	static int pright=0;
	static int pmiddle=0;
	
	left   = (mBuff[0] & 0x01)>0;
	right  = (mBuff[0] & 0x02)>0;
	middle = (mBuff[0] & 0x04)>0;
	
	if (left!=pleft)     left   |= UNHANDLED_EVENT;
	if (right!=pright)   right  |= UNHANDLED_EVENT;
	if (middle!=pmiddle) middle |= UNHANDLED_EVENT;

	pleft   = left;
	pright  = right;
	pmiddle = middle;

	x += mBuff[1];
	y += mBuff[2];		
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