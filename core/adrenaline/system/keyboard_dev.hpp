#ifndef _KEYBOARD_DEV_
#define _KEYBOARD_DEV_


#define UNHANDLED_EVENT 0x80


char map_key(int code);

int dev_keyboard_init	  ();
int dev_keyboard_timeslice();
int dev_keyboard_close	  (float screen_width, float screen_height);



#endif
