// first OpenVG program
// Anthony Starks (ajstarks@gmail.com)
#include <shapes.h>
#include <fontinfo.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "VG/openvg.h"
#include "VG/vgu.h"
#include "tk_colors.hpp"
#include "display.h"



int screen_width, screen_height;

void Stroke_c( Color c )
{
	Stroke( c.m_red, c.m_green, c.m_blue, c.get_alpha() );
}
void Fill_c( Color c )
{
	Fill( c.m_red, c.m_green, c.m_blue, c.get_alpha() );
}


void Stroke_l( long int Color )
{
	float alpha = ((Color & 0xFF000000)>>24) / 255.;
	Stroke( (Color & 0x00FF0000)>>16, (Color & 0x0000FF00)>>8, (Color & 0x000000FF), alpha );
}
void Fill_l( long int Color)
{
	float alpha = ((Color & 0xFF000000)>>24) / 255.;
	Fill( (Color & 0x00FF0000)>>16, (Color & 0x0000FF00)>>8, (Color & 0x000000FF), alpha );
}

// rotext draws text, rotated around the center of the screen, progressively faded
void rotext(int w, int h, int n, char *s) {
	VGfloat fade = (100.0 / (VGfloat) n) / 100.0;
	VGfloat deg = 120.0 / n;
	VGfloat x = screen_width / 2, y = screen_height / 2;
	VGfloat alpha = 1.0;	// start solid
	int i, size = w / 16;

	//Start(w, h);
	//Background(90, 90, 90);
	Translate(x, y);
	for (i = 0; i < n; i++) {
		Fill(255, 0, 125, alpha);
		Text(0, 0, s, SerifTypeface, size);
		//alpha -= fade;				   // fade
		//size += n;				   // enlarge
		Rotate(deg);
	}
	//End();
}
int tilt_beyond(float angleDegrees) 
{
	VGfloat fade  = (100.0 / (VGfloat) 1) / 100.0;
	VGfloat deg   = angleDegrees;
	VGfloat x     = screen_width / 2, y = screen_height / 2;
	VGfloat alpha = 1.0;	// start solid
	int     i, size = screen_width/64;

	Translate(x, y);
	Rotate(deg);	
	Fill(255, 0, 125, alpha);
	//Text(0, 0, "Beyond", SerifTypeface, size);
	TextMid  ( 0, 0, "Beyond ", SerifTypeface, screen_width/10);
	Rotate   ( -deg    );	
	Translate( -x, -y );
}

void play(float angleDegrees)
{
	Fill(255, 0, 0, 1);				   // Red text
	TextMid(screen_width / 2, screen_height-150, "Onesimus", SerifTypeface, screen_width / 10);
	
	Fill(0, 255, 0, 1);				   // Red text
	TextMid(125, screen_height / 2, "By ", SerifTypeface, screen_width / 10);
	
	Fill(255, 0, 0, 1);				   // Red text
//	TextMid(2*screen_width/4, (screen_height/4)+75, "Beyond ", SerifTypeface, screen_width/10);
	tilt_beyond(angleDegrees);
	
	Fill(0, 0, 255, 1);				   // Red text
	TextMid(2*screen_width/4, (screen_height/4)-75, "Kinetics, Inc.", SerifTypeface, screen_width/10);
}

int init_screen()
{
	init(&screen_width, &screen_height);				   // Graphics initialization
/*	Start(screen_width, screen_height);				   // Start the picture
	Background(0, 0, 0);				   // Black background
	Fill(44, 77, 232, 1);				   // Big blue marble
	Circle(screen_width / 2, 0, screen_width);			   // The "world"
*/
}
void start_screen()
{
	Start(screen_width, screen_height);		// Start the picture
	Background(0, 0, 0);					// Black background
}
void end_screen()
{
	End();
}

int close_screen()
{
	char s[3];

	fgets(s, 2, stdin);			   // look at the pic, end with [RETURN]
	finish();					   // Graphics cleanup
	exit(0);
}

int draw_screen(float angleDegrees) 
{	
	Start(screen_width, screen_height);				   // Start the picture
	Background(0, 0, 0);				   // Black background
	
	Fill(44, 77, 232, 1);				   // Big blue marble
	Circle(screen_width / 2, 0, screen_width);			   // The "world"
	Fill(255, 255, 255, 1);				   // White text
	//TextMid(screen_width / 2, screen_height / 2, "hello, world", SerifTypeface, screen_width / 10);	// Greetings 
	play(angleDegrees);
	//rotext(screen_width/4, screen_height/4, 6, "Tilt");	
	End();						   // End the picture	
}
void test_scr()
{
	init_screen();
	draw_screen(0.0);
//	close_screen();
}
void onesimus()
{
	char s[3];
	init(&screen_width, &screen_height);				   // Graphics initialization
	
	Start(screen_width, screen_height);				   // Start the picture
	Background(0, 0, 0);				   // Black background
	
	Fill(44, 77, 232, 1);				   // Big blue marble
	Circle(screen_width / 2, 0, screen_width);			   // The "world"
	Fill(255, 255, 255, 1);				   // White text
	//TextMid(screen_width / 2, screen_height / 2, "hello, world", SerifTypeface, screen_width / 10);	// Greetings 
	play(0.0);
	rotext(screen_width/4, screen_height/4, 6, "Tilt");

	Image(100, 100, 200, 200, "/home/pi/openvg/client/desert1.jpg");
	

	End();						   // End the picture
	fgets(s, 2, stdin);				   // look at the pic, end with [RETURN]
	finish();					   // Graphics cleanup
	exit(0);
}
