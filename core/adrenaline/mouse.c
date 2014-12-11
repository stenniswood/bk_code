// first OpenVG program, with mouse 
/*******
	To summarize, eventThread reads from the mouse driver and 
	puts the info into mouse structure.
	
*********/
// to do list:
// show cursor before first mouse movement
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "VG/openvg.h"
#include "VG/vgu.h"
#include <fontinfo.h>
#include <shapes.h>
#include <linux/input.h>
#include <fcntl.h>
#include <pthread.h>
#include "mouse.h"


static int 			mouse_fd;
struct input_event 	mouse_ev;		// info read from driver.

struct mouse_t 	mouse;					// global mouse state
int 		left_count = 0;
int 		quitState = 0;
#define    	CUR_SIZ  16				// cursor size, pixels beyond centre dot


// eventThread reads from the mouse input file
void *eventThread(void *arg) 
{
	// Open mouse driver
//	if ((mouse_fd = open("/dev/input/mouse0", O_RDONLY)) < 0) {	
//	if ((mouse_fd = open("/dev/input/event0", O_RDONLY)) < 0) {	
//	if ((mouse_fd = open("/dev/input/event1", O_RDONLY)) < 0) {	
	if ((mouse_fd = open("/dev/input/event2", O_RDONLY)) < 0) {
		fprintf(stderr, "Error opening Mouse!\n");
		quitState = 1;
		return &quitState;
	}

	mouse.x = mouse.max_x / 2;			   //Reset mouse
	mouse.y = mouse.max_y / 2;

	while (1) {
		read(mouse_fd, &mouse_ev, sizeof(struct input_event));
		printf("[%4.0f,%4.0f]\r",mouse.x,mouse.y);

		// Check events
		//mouse.left = CUR_SIZ * 2;		   // Reset Mouse button states
		//mouse.right = CUR_SIZ * 2;

		if (mouse_ev.type == EV_REL) {
			if (mouse_ev.code == REL_X) {
				mouse.x += (VGfloat) mouse_ev.value;
				if (mouse.x < 0) {
					mouse.x = 0;
				}
				if (mouse.x > mouse.max_x) {
					mouse.x = mouse.max_x;
				}
			}
			if (mouse_ev.code == REL_Y) {	   //This ones goes backwards hence the minus
				mouse.y -= (VGfloat) mouse_ev.value;
				if (mouse.y < 0) {
					mouse.y = 0;
				}
				if (mouse.y > mouse.max_y) {
					mouse.y = mouse.max_y;
				}
			}
		}

		if (mouse_ev.type == EV_KEY) {
			//printf("Time Stamp:%d - type %d, code %d, value %d\n",
			//      mouse_ev.time.tv_usec,mouse_ev.type,mouse_ev.code,mouse_ev.value);
			if (mouse_ev.code == BTN_LEFT) {
				mouse.left = UNHANDLED_EVENT | (mouse_ev.value & 0x01);
				printf("Left button: evalue=%d\n", mouse_ev.value );
			}
			if (mouse_ev.code == BTN_RIGHT) {
				mouse.right = UNHANDLED_EVENT | (mouse_ev.value & 0x01);
				printf("Right button: value=%d\n", mouse_ev.value);
			}
		}
	}
}

static int cur_sx, cur_sy, cur_w, cur_h;	// cursor location and dimensions
static int cur_saved = 0;	// amount of data saved in cursor image backup

// saveCursor saves the pixels under the mouse cursor
void save_pixels(VGImage CursorBuffer, int curx, int cury, int screen_width, int screen_height, int s) {
	int sx, sy, ex, ey;

	sx = curx - s;					   // horizontal 
	if (sx < 0) {
		sx = 0;
	}
	ex = curx + s;
	if (ex > screen_width) {
		ex = screen_width;
	}
	cur_sx = sx;
	cur_w = ex - sx;

	sy = cury - s;					   // vertical 
	if (sy < 0) {
		sy = 0;
	}
	ey = cury + s;
	if (ey > screen_height) {
		ey = screen_height;
	}
	cur_sy = sy;
	cur_h = ey - sy;

	vgGetPixels(CursorBuffer, 0, 0, cur_sx, cur_sy, cur_w, cur_h);
	cur_saved = cur_w * cur_h;
}

// restoreCursor restores the pixels under the mouse cursor
void restore_pixels( VGImage CursorBuffer )
{
	if (cur_saved) {
		vgSetPixels(cur_sx, cur_sy, CursorBuffer, 0, 0, cur_w, cur_h);
	}
}

// circleCursor draws a translucent circle as the mouse cursor
void circleCursor(int curx, int cury, int width, int height, int s) {
	Fill(255, 255, 0, 0.50);
	Circle(curx, cury, s);
	Fill(255, 0, 0, 1);
	Circle(curx, cury, 2);
}


// get these values from display manager.
int 	cursorx, cursory, cbsize;
VGImage CursorBuffer;


int mouse_init(int screen_width, int screen_height)
{
	mouse.max_x  = screen_width;
	mouse.max_y  = screen_height;

	// Allocate memory for restoring mouse covered pixels:
	cursorx = mouse.max_x / 2;
	cursory = mouse.max_y / 2;
	cbsize = (CUR_SIZ * 2) + 1;
	CursorBuffer = vgCreateImage(VG_sABGR_8888, cbsize, cbsize, VG_IMAGE_QUALITY_BETTER);

	// CREATE A THREAD TO READ FROM DEVICE:
	pthread_t inputThread;
	int result = pthread_create(&inputThread, NULL, &eventThread, NULL);
	if (result != 0) {
		fprintf(stderr, "Unable to initialize the mouse\n");
	}
} 

void draw_cursor(int curx, int cury, int width, int height, int s) 
{
	circleCursor(cursorx, cursory, mouse.max_x, mouse.max_y, CUR_SIZ);
}

void hide_mouse()
{
	restore_pixels(CursorBuffer);

}
void save_mouse()
{
	// draw cursor:
	save_pixels(CursorBuffer, cursorx, cursory, mouse.max_x, mouse.max_y, CUR_SIZ);
	draw_cursor(cursorx, cursory, mouse.max_x, mouse.max_y, CUR_SIZ);
}

int mouse_timeslice() 
{	
	if (mouse.x != cursorx || mouse.y != cursory) 	// if the mouse moved...
	{
		restore_pixels(CursorBuffer);
		cursorx = mouse.x;		// save for comparison on next timeslice.
		cursory = mouse.y;
		save_pixels(CursorBuffer, cursorx, cursory, mouse.max_x, mouse.max_y, CUR_SIZ);

		// draw cursor:
		draw_cursor(cursorx, cursory, mouse.max_x, mouse.max_y, CUR_SIZ);
	}
	if (mouse.left & UNHANDLED_EVENT)
	{
		mouse.left &= (~UNHANDLED_EVENT);	// clear the flag.
		int retval = mouse.left;
		return (retval<<4);			// 1=> button down.  0=> button up
	}
	if (mouse.right & UNHANDLED_EVENT)
	{
		mouse.right &= (~UNHANDLED_EVENT);	// clear the flag.
		return ((mouse.right)<<4);			// 1=> button down.  0=> button up
	}
	return 0;
}


int mouse_close()
{
	close(mouse_fd);
	restore_pixels(CursorBuffer);			   // not strictly necessary as display will be closed
	vgDestroyImage(CursorBuffer);			   // tidy up memory
	finish();					   // Graphics cleanup
}

