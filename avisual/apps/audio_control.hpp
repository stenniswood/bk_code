/* This will include :
	Volume slider,
	Live Volume display
	Source selector (HDMI, ouput plug, network, etc)
	
	

*/
#ifndef _AUDIO_CONTROL_
#define _AUDIO_CONTROL_

#include "Graphbase.hpp"
#include "dataset.hpp"


class AudioVolumePanel : public Window
{
public:
	AudioVolumePanel();
	AudioVolumePanel( int Left, int Right, int Top, int Bottom );

	virtual void 	Initialize(	);
	int				place_views();
	

	StereoPowerLevels 	spl;
	Leveler			 	VolumeSlider;
	
	RadioButton			m_HDMI_output_selector;
	RadioButton			m_headphones_output_selector;
	RadioButton			m_network_output_selector;
	RadioButton			m_aux_output_selector;
	
	
private:

};
 
 
#endif
