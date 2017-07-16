#ifndef _DISPLAY_H_
#define _DISPLAY_H_


#include "tk_colors.hpp"


#ifdef  __cplusplus
extern "C" {
#endif

//extern int screen_width, screen_height;

void start_screen();
void end_screen();

void onesimus	( );
int init_screen	( );
int draw_screen	( float angleDegrees );
int close_screen( );
int tilt_beyond	( float angleDegrees );
//void Fill_l		( unsigned long int Color	 );
//void Stroke_l	( unsigned long int Color	 );

void Stroke_c	( Color c 			 );
void Fill_c		( Color c 			 );


#ifdef  __cplusplus
}
#endif

#endif
