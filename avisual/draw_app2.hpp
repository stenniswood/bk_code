#ifndef _DRAW_APP2_LAYOUTS_
#define _DRAW_APP2_LAYOUTS_


/**** These function handle the menu commands:   ****/
void draw_zoom_in			();
void draw_zoom_out			();
void draw_show_mixer		();
void draw_show_frequency	();
void draw_show_fft  		();

void draw_play					();
void draw_stop					();
void draw_rewind				();
void draw_select_output_device  ();
void draw_record_from			();
void draw_broadcast_to			();

void draw_reverb			 ();	
void draw_equalize			 ();	
void draw_backwards		 	 ();	
void draw_convert_to_mono	 ();	
void draw_convert_to_stereo  ();	
/***** END OF MENU COMMAND FUNCTIONS *****/

void configure_wave_views	( int mChannels, short* mDataCh1, short* mDataCh2 );

class DrawApp : public Application
{
public:
	DrawApp ();
	DrawApp ( Rectangle* mRect );
	~DrawApp();

	virtual void 	Initialize		(	);	// create all the objects here.
	int				onPlace			(	);

	virtual void	setup_menu  	( );
	virtual void	setup_app_menu  ( );
	virtual void 	register_with_display_manager();	
	
	int				About			(	);
	int				Preferences		(	);

	virtual void	file_new		( );
	virtual void	file_open		( );	
	virtual void	file_open_recent( );	
	virtual void	file_save		( );
	virtual void	file_save_as	( );

	void 			draw_play		( );
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

extern DrawApp* draw_app;

#endif


