#ifndef _TEST_COMBO_LAYOUTS_
#define _TEST_COMBO_LAYOUTS_


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



void print_combo_test_list	(			);
void load_combo_test_screen ( int number);
void init_okay_cancel_dlg	(			);
void init_audio_view		(			);

// 12, 7, 16,  not working


#endif


