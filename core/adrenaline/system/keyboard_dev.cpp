#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <linux/input.h>
#include <fcntl.h>
#include <pthread.h>
//#include <string.h>
#include <string>
#include <vector>
#include <map>
#include <cctype>
#include "VG/openvg.h"
#include "VG/vgu.h"
#include <fontinfo.h>
#include <shapes.h>

#include "keyboard_dev.hpp"
#include "display_manager.hpp"


std::map <int, char> key_map;

static int 			keyboard_fd;
struct input_event 	keyboard_ev;		// info read from driver
bool shift_down = false;

#define Debug 0


static const char *const evval[3] = {
    "RELEASED",
    "PRESSED ",
    "REPEATED"
};

char map_key(int code)
{
	char ch = key_map[code];
	if (shift_down) {
		ch = toupper(ch);
		if (ch == '1')  ch='!';
		if (ch == '2')  ch='@';
		if (ch == '3')  ch='#';
		if (ch == '4')  ch='$';
		if (ch == '5')  ch='%';
		if (ch == '6')  ch='^';								
		if (ch == '7')  ch='&';
		if (ch == '8')  ch='*';
		if (ch == '9')  ch='(';
		if (ch == '0')  ch=')';
		if (ch == '-')  ch='_';
		if (ch == '=')  ch='+';				
		if (ch == '[')  ch='{';				
		if (ch == ']')  ch='}';
		
		if (ch == ',')  ch='<';
		if (ch == '.')  ch='>';
		if (ch == '/')  ch='?';
		if (ch == ';')  ch=':';
		if (ch == '\'')  ch='\"';
		if (ch == '\\')  ch='|';		
	}
	return ch;
}

/* If a real keyboard is plugged in (via usb) */
void *Keyboard_eventThread(void *arg) 
{
	const char *dev = "/dev/input/by-path/platform-20980000.usb-usb-0:1.2.1:1.0-event-kbd";
    ssize_t n;
    int fd;

    keyboard_fd = open(dev, O_RDONLY);
    if (keyboard_fd == -1) {
        fprintf(stderr, "Cannot open %s: %s.\n", dev, strerror(errno));
        return EXIT_FAILURE;
    }
    printf("Opened /dev/input/by-path/KEYBOARD\n");
    
    char ch;
	while (1) {

		n = read(keyboard_fd, &keyboard_ev, sizeof(struct input_event));
		if (n == (ssize_t)-1) {		// incase of buggy kernel/driver partial event.
            if (errno == EINTR)  continue;
            else  break;
        } else if (n != sizeof keyboard_ev) {
            errno = EIO;
            break;
        }
        
		// Read Linux Event for the key        
		if (keyboard_ev.type==EV_KEY)
		{
			
			if ((keyboard_ev.code == KEY_LEFTSHIFT) || (keyboard_ev.code == KEY_RIGHTSHIFT))
			{	if (keyboard_ev.value==0)
					shift_down = false;
				else shift_down = true;
			} else if (keyboard_ev.value>=1  && keyboard_ev.value<=2)
			{
					if (shift_down)  dprintf("shift down");
					
					ch = map_key(keyboard_ev.code);
					MainDisplay.relay_key( ch );					
					if (Debug) printf("%c\n", ch);	
					//printf("%s 0x%04x (%d) %c\n", evval[keyboard_ev.value], (int)keyboard_ev.code, 
					//							(int)keyboard_ev.code, key_map[keyboard_ev.code]);		
			}
		}
	}
 	fflush (stdout);
    fprintf(stderr, "%s.\n", strerror(errno));
    return EXIT_FAILURE;
}


int dev_keyboard_init()
{
	key_map[KEY_MINUS]		= '-';
	key_map[KEY_EQUAL]		= '=';
	key_map[KEY_LEFTBRACE]	= '[';
	key_map[KEY_RIGHTBRACE]	= ']';
	key_map[KEY_BACKSLASH]	= '\\';
	key_map[KEY_SPACE]		= ' ';
	key_map[KEY_ENTER]		= '\n';
	key_map[KEY_BACKSPACE]	= KEY_BACKSPACE;

	key_map[KEY_1] = '1';
	key_map[KEY_2] = '2';
	key_map[KEY_3] = '3';
	key_map[KEY_4] = '4';
	key_map[KEY_5] = '5';
	key_map[KEY_6] = '6';
	key_map[KEY_7] = '7';
	key_map[KEY_8] = '8';
	key_map[KEY_9] = '9';
	key_map[KEY_0] = '0';

	key_map[KEY_Q] = 'q';
	key_map[KEY_W] = 'w';
	key_map[KEY_E] = 'e';
	key_map[KEY_R] = 'r';
	key_map[KEY_T] = 't';
	key_map[KEY_Y] = 'y';
	key_map[KEY_U] = 'u';
	key_map[KEY_I] = 'i';
	key_map[KEY_O] = 'o';
	key_map[KEY_P] = 'p';

	key_map[KEY_A] = 'a';
	key_map[KEY_S] = 's';
	key_map[KEY_D] = 'd';
	key_map[KEY_F] = 'f';
	key_map[KEY_G] = 'g';
	key_map[KEY_H] = 'h';
	key_map[KEY_J] = 'j';
	key_map[KEY_K] = 'k';
	key_map[KEY_L] = 'l';
	key_map[KEY_SEMICOLON] = ';';	
	key_map[KEY_APOSTROPHE] = '\'';		

	key_map[KEY_Z] = 'z';
	key_map[KEY_X] = 'x';
	key_map[KEY_C] = 'c';
	key_map[KEY_V] = 'v';
	key_map[KEY_B] = 'b';
	key_map[KEY_N] = 'n';
	key_map[KEY_M] = 'm';
	key_map[KEY_COMMA] = ',';
	key_map[KEY_DOT]   = '.';
	key_map[KEY_SLASH] = '/';
	//printf(" $$$$$    DEV_keyboard_init() keys mapped, creating thread !! \n");

	// CREATE A THREAD TO READ FROM DEVICE:
	pthread_t inputThread;
	int result = pthread_create(&inputThread, NULL, &Keyboard_eventThread, NULL);
	if (result != 0) {
		fprintf(stderr, "Unable to initialize the mouse\n");
	}    
}

int dev_keyboard_timeslice()
{
	return 0;
}

int dev_keyboard_close(float screen_width, float screen_height)
{
    close (keyboard_fd);
}

