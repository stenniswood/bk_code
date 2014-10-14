#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#ifdef  __cplusplus
extern "C" {
#endif

//extern int screen_width, screen_height;

void start_screen();
void end_screen();

void onesimus();
int init_screen();
int draw_screen(float angleDegrees);
int close_screen();
int tilt_beyond(float angleDegrees);
void Fill_l( long int Color);
void Stroke_l( long int Color);


#ifdef  __cplusplus
}
#endif

#endif
