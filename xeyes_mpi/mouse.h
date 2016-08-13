#ifndef _MOUSE_
#define _MOUSE_
#ifdef __cplusplus
extern "C" {
#endif

// The thread puts this into left, middle, right.
// The timeslice will clear it.
#define UNHANDLED_EVENT 0x80

#define LEFT_BUTTON_DOWN 0x10
#define RIGHT_BUTTON_DOWN 0x01


// Mouse state structure
struct mouse_t {
	float 			x,     y;
	int 			left,  middle, right;
	int 			max_x, max_y;
};

extern struct mouse_t 	mouse;		// global mouse state


int mouse_init(int screen_width, int screen_height);
int mouse_timeslice();
int mouse_close();

//void hide_mouse();
//void show_mouse();
void save_mouse();


#ifdef __cplusplus
}
#endif

#endif
