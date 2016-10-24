/*******************************************************
* Line graph OpenVG program
* 
* Steve Tenniswood
********************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include "VG/openvg.h"
#include "VG/vgu.h"
#include <shapes.h>
#include <fontinfo.h>
#include "Graphbase.hpp"
#include "control.hpp"
#include "button.hpp"
#include "display.h"
#include "text_view.hpp"


AudioVolumePanel::AudioVolumePanel()
{

}
AudioVolumePanel::AudioVolumePanel( int Left, int Right, int Top, int Bottom )
{

}


void AudioVolumePanel::Initialize()
{


	VolumeSlider.set_level_percent( 50.0    );	
	VolumeSlider.set_max		  ( 100.0   );
	VolumeSlider.set_width_height ( 70,200  );
	VolumeSlider.move_to		  ( 30, 100 );	
	VolumeSlider.set_text		  ( "Volume" );

	// also spl stereo power level: 
	spl.set_max					( 100.0	   );
	spl.set_min					(   0.0	   );
	spl.set_level_left			(  50.0    );
	spl.set_level_right			(  50.0    );
	spl.set_number_boxes		(  -1 	   );
	spl.set_width_height		( 100, 200 );
	spl.move_to					( 100, 100 );

}

void	AudioVolumePanel::place_views()
{
	m_HDMI_output_selector.set_position(10,20,40,10);
	m_headphones_output_selector.set_position_below( m_HDMI_output_selector );
	m_network_output_selector.set_position_below( m_headphones_output_selector );
	m_aux_output_selector.set_position_below( m_network_output_selector );

}

