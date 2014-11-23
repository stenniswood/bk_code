// line graph OpenVG program
// Steve Tenniswood
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include "VG/openvg.h"
#include "VG/vgu.h"
#include <shapes.h>
#include <fontinfo.h>
#include "Graphbase.hpp"
#include "control.hpp"
#include "display.h"
#include "wave_view.hpp"

#define margin_percent 0.07

WaveView::WaveView(int Left, int Right, int Top, int Bottom )
{
	set_position( Left, Right, Top, Bottom );
	Initialize();
}

WaveView::WaveView( int Width, int Height )
{
	left=0; bottom=0; 
	width  = Width;
	height = Height;
	Initialize();
}

WaveView::WaveView()
{
	set_position( 0, 0, 0, 0 );
	Initialize();
}

void WaveView::Initialize()
{
	background_color = 0xFF000000;

	margin		= 0.05;		// 5%
	max 		= 32767;
	gain		= 8.0;
}

WaveView::~WaveView()
{
}

void WaveView::calc_metrics( )
{
	pix_height = ceil((float)height/2.);
	center_y   = bottom + height/2;	
}

float WaveView::sample_to_y( short mSampleValue )
{
	// draw 1 sample per pixel:		
	float samp1_y = (gain*mSampleValue / max) * pix_height;	
	if (samp1_y > pix_height)	samp1_y = pix_height;		// clip top
	if (samp1_y < -pix_height)	samp1_y = -pix_height;		// clip bottom
	samp1_y += center_y;
	return samp1_y;
}

int WaveView::draw( )
{
	Control::draw();
	calc_metrics();
	
	// Pick waveform color:
	long int color = 0xFF00FF00;		// start at green.
	Fill_l  (color);
	Stroke_l(color);

	// DRAW : 
	float y1,y2;
	int start 	   = 0;		// first sample visible!
	int i		   = start;	
	for ( float x=left; x<(left+width); x++,i++ )
	{
		y1 = sample_to_y( data[i]   );
		y2 = sample_to_y( data[i+1] );
		Line( x, y1, x+1, y2 );
	}	
}


