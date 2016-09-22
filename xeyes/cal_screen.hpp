#ifndef _CAL_SCREEN_HPP_
#define _CAL_SCREEN_HPP_

#include <string>


void close_cal_screen	();
int  draw_cal_screen	();

//====================================================
void view_directory_of_images( std::string mPath );
int  show_person_faces		 ( std::string person_folder_name );

void make_training_size		( std::string mPath );

void init_gpio				( );
void text_to_speech_pico	( const char* mText );
void text_to_speech_festival( const char* mText );

#endif
