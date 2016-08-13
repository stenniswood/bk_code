
#include "VG/openvg.h"
#include "VG/vgu.h"
#include <math.h>
#include <shapes.h>
#include <fontinfo.h>

#include "cal_screen.hpp"


const int CAL_POINT_RADIUS = 20;

void draw_cal_point(int x, int y)
{
    
	Circle( x,y, CAL_POINT_RADIUS );
	//TextMid(width / 2, height / 2, "hello, world", SerifTypeface, width / 10);
}

void draw_corners( int SWidth, int SHeight )
{
	Fill(44, 77, 232, 1);				    // Big blue marble
	float x,y;
	x = 10;  	       y=10;
	draw_cal_point(x,y);
	x = SWidth-10;     y=10;
	draw_cal_point(x,y);

	x = SWidth-10;         y=SHeight-10;
	draw_cal_point(x,y);
	x = 10;                y=SHeight-10;
	draw_cal_point(x,y);
}

void draw_grid_points( int rows, int cols, int SWidth, int SHeight )
{
	float x_increment = SWidth  / (cols+1);
	float y_increment = SHeight / (rows+1);
	int   x,y;
	Fill(144, 77, 132, 1);				    // Big blue marble
	
	for (int c=0; c<cols; c++)
		for (int r=0; r<rows; r++)
		{	
			x = trunc(x_increment * (c+1));
			y = trunc(y_increment * (r+1));
			draw_cal_point(x,y);
		}	
}

int draw_cal_screen() 
{
    char s[3];
    int width  =1280;
    int height =720;
    init(&width, &height);                  // Graphics initialization

    Start(width, height);                   // Start the picture
    Background(0, 0, 0);                    // Black background

	draw_corners( width, height );
	draw_grid_points( 3, 5, width, height );
	
    //Circle(width / 2, 0, width);            // The "world"
	//Fill(255, 255, 255, 1);                 // White text
    //TextMid(width / 2, height / 2, "hello, world", SerifTypeface, width / 10);  // Greetings 
    End();                                  // End the picture
}
