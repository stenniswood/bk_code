#ifndef _READ_PENDANT_H_
#define _READ_PENDANT_H_


#define NUM_SLIDERS 6
#define NUM_DIALS   4


extern int  Slider_values[NUM_SLIDERS];
extern int  Dial_values  [NUM_DIALS];


int  read_pendant(void);
void print_values();


#endif

