#ifndef _WAVE_VIEW_H_
#define _WAVE_VIEW_H_

//#include "../core/can/bk_system_defs.h"
#include "bk_system_defs.h"



#define DefaultPadding 15
class Window;

/* This class shows an audio waveform
   green, yellow, red color gradients.  with rectangular boxes. */

class WaveView : public Control
{
public:
	WaveView( int Left, int Right, int Top, int Bottom );
	WaveView( int Width, int Height 					 );
	WaveView();
	~WaveView();

	void 		Initialize();
	void		set_data	 ( short* mData )	{ data = mData;	};
	void		set_max  	 ( short mMax 	)	{ if (mMax==-1) max = 32767; else max=mMax; }; 
	void		show_number_boxes( int mBoxes );


	float		sample_to_y ( short mSampleValue );
	int 		get_VG_path_coords( float* &mCoords, int mChannel, 
									float mZoomFactor );

	void 		draw_box 	( float mx, float my, long int mColor );
	int 		draw_wave ( );
	int 		draw_wave2( int mChannel, float mZoom );	
	virtual int draw		( );
	void 		calc_metrics( );

	// The wave may be shared amoung multiple views!
	// So we must select a channel too.
	Wave*	m_wave;
	byte	m_channel;

protected:
	short*	data;
	long int data_length;
	
	int		pix_height;
	int		center_y;

	float	gain;
	float	max;				// usually 100, 0;
	float 	level;	
	float	margin;				// percentage of width
};

 
#endif
