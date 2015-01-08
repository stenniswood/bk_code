// line graph OpenVG program
// Steve Tenniswood
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include <math.h>
#include "VG/openvg.h"
#include "VG/vgu.h"
#include <shapes.h>
#include <fontinfo.h>
#include "Graphbase.hpp"
#include "control.hpp"
#include "display.h"

#include "wave.hpp"
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
	m_wave 		= NULL;
	m_channel   = 0;
	
	margin		= 0.05;		// 5%
	max 		= 32767;
	gain		= 8.0;
}

WaveView::~WaveView()
{
}

void WaveView::calc_metrics( )
{
	pix_height = floor((float)height/2.)-1;
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

void WaveView::set_data( short* mData )
{ 
	data = mData;
	set_samples_visible( m_wave->get_samples_recorded() );
	
}
	
/*int WaveView::draw_wave( )
{
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
}*/


int WaveView::get_VG_path_coords( float* &mCoords, int mChannel, 
									float mZoomFactor )
{
	// By skipping every other zoom factor sample, the waveform is not interesting at all
	// the aliasing leaves 
	int samples = m_wave->get_samples_recorded();
	int vis_samples = (width / mZoomFactor);
	if (vis_samples < samples)
		samples = vis_samples;
	mCoords = new float[samples*2];

	// a VGPath is an array of (x,y) pairs : 
	int index = 1;
	float center = bottom + height/2.;
	float scale  = height/(2*32767.) ;

	for (int i = 0; i<samples; i++)
	{
		(mCoords)[index] = center + scale * m_wave->GetSample(i, mChannel);
		index += 2;		// skip to next y value
	}

	// Fill in the X coordinates:
	index = 0;
	for (int i=0; i<samples; i++)
	{
		(mCoords)[index] = left + (i*mZoomFactor);
		index += 2;
	}
	return samples;
}

int WaveView::create_path( int mChannel, float mZoom )
{
	if (m_wave==NULL) return 0;	
	
	VGint numCoords = get_VG_path_coords( coords, mChannel, mZoom );
	printf( "WaveView::create_path()\n" );
	
	numCmds	  = numCoords;
	commands  = new VGubyte[numCmds];

	path 	  = vgCreatePath(VG_PATH_FORMAT_STANDARD,
								VG_PATH_DATATYPE_F,
								1.0f, 0.0f, 		// scale,bias
								numCmds, numCoords,
								VG_PATH_CAPABILITY_ALL );

	commands[0] = VG_MOVE_TO_ABS;
	for (int i=1; i<numCmds; i++)
		commands[i] = VG_LINE_TO_ABS;

	vgAppendPathData(path, numCmds, commands, coords);

	printf( "WaveView::draw_wave2() drawPath done\n" );
}

int WaveView::delete_path( )
{
	delete commands;
	delete coords;
	//
	printf( "WaveView::draw_wave2() delete commands done\n" );
	//
	printf( "WaveView::draw_wave2() delete coords done\n" );

}

/* Times we want to re-create the path:

		Zoom change.
		Scroll of waveform.
		Live audio as buffers arrive.

	Anytime the waveform on the screen changes.
*/

void WaveView::set_samples_visible( int SamplesVisible )
{
	float zoom = m_wave->calc_zoom( width, SamplesVisible );
	if (SamplesVisible>0)
	{
		printf("SamplesVisible=%d;   zoom=%1.6f\n", SamplesVisible, zoom );
		create_path( m_channel, zoom );
	}
}

int WaveView::draw( )
{
	printf("WaveView::draw()\n");
	Control::draw();
	calc_metrics();

	// Pick waveform color:
	long int color = 0xFF00FF00;		// start at green.
	Fill_l  (color);
	Stroke_l(color);
	StrokeWidth(1.0);

	vgDrawPath(path, VG_STROKE_PATH );
	printf("WaveView::draw() done\n");
	return 1;
}

