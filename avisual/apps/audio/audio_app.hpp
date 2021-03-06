#ifndef _AUDIO_APPS_
#define _AUDIO_APPS_

#include "application.hpp"

/* These function handle the menu commands:   */
void audio_file_new			();		
void audio_file_open		();		
void audio_file_open_recent	();
void audio_file_save		();		
void audio_file_save_as		();	

void audio_zoom_in			();		
void audio_zoom_out			();		
void audio_show_mixer		();		
void audio_show_frequency	();
void audio_show_fft  		();		

void audio_play					();
void audio_stop					();
void audio_rewind				();
void audio_select_output_device ();
void audio_record_from			();
void audio_broadcast_to			();

void audio_reverb			 ();	
void audio_equalize			 ();	
void audio_backwards		 ();	
void audio_convert_to_mono	 ();	
void audio_convert_to_stereo ();	
/***** END OF MENU COMMAND FUNCTIONS *****/


void configure_wave_views(int mChannels, short* mDataCh1, short* mDataCh2 );

void init_audio_app	( );

// 12, 7, 16,  not working

class AudioApp : public Application
{
public:
	AudioApp ();
	AudioApp ( Rectangle* mRect );
	virtual ~AudioApp();

	virtual void 	Initialize		(	);	// create all the objects here.

	virtual void	setup_main_menu ( );
	virtual void	setup_app_menu  ( );
	virtual void 	register_with_display_manager();	
	
	void			About			(	);
	void			Preferences		(	);
			
	virtual void	file_new		( );
	virtual void	file_open		( );	
	virtual void	file_open_recent( );	
	virtual void	file_save		( );
	virtual void	file_save_as	( );

	void 			audio_play		( );
	void 			configure_wave_views(int mChannels, short* mDataCh1, short* mDataCh2 );

	void 			zoom_in			();
	void 			zoom_out		();
	void 			show_mixer		();
	void 			show_frequency	();
	void 			show_fft  		();

	Wave 				dWave;			// active wave

	TextView 			WaveformInfo;
	StereoPowerLevels 	spl;
	FrequencyView	 	freq_view;
	WaveView 		 	wave_view_left ;
	WaveView 		 	wave_view_right;
	Leveler			 	VolumeSlider;
};

extern AudioApp* audio_app;

#endif


