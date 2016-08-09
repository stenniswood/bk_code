
#include "VG/openvg.h"
#include "VG/vgu.h"
#include <shapes.h>
#include <fontinfo.h>

#include "cal_screen.hpp"


void draw_cal_point()
{
	Circle(width/2, 0, width);            
	//TextMid(width / 2, height / 2, "hello, world", SerifTypeface, width / 10);
}

void draw_cal_grid( int SWidth, int SHeight )
{
	float x,y;
	x = 10;  	       y=10;
	draw_cal_point(x,y);

	// 
	x = SWidth-10;         y=10;
	draw_cal_point(x,y);

	x = SWidth-10;         y=SHeight-10;
	draw_cal_point(x,y);
	x = 10;                y=SHeight-10;
	draw_cal_point(x,y);
}

int draw_cal_screen() 
{
    char s[3];
    int width, height;

    init(&width, &height);                  // Graphics initialization

    Start(width, height);                   // Start the picture
    Background(0, 0, 0);                    // Black background
    Fill(44, 77, 232, 1);                   // Big blue marble
    
    //Circle(width / 2, 0, width);            // The "world"
    Fill(255, 255, 255, 1);                 // White text
    //TextMid(width / 2, height / 2, "hello, world", SerifTypeface, width / 10);  // Greetings 
    End();                                  // End the picture
}
