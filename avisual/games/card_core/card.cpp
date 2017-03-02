/*******************************************************
* 
* 
* Steve Tenniswood
********************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <jpeglib.h>
#include "VG/openvg.h"
#include "VG/vgu.h"
#include "bcm_host.h"

#include <fontinfo.h>
#include <shapes.h>
#include "control.hpp"
#include "button.hpp"
#include "display.h"
#include "icon.hpp"
#include "card.hpp"
#include "global.h"

char e_card_values[] = {
'A','2','3','4','5','6','7','8','9','T','J','Q','K',
'A','2','3','4','5','6','7','8','9','T','J','Q','K',
'A','2','3','4','5','6','7','8','9','T','J','Q','K',
'A','2','3','4','5','6','7','8','9','T','J','Q','K',
};
char e_card_suits[] = {
'H','H','H','H','H','H','H','H','H','H','H','H','H','H',
'C','C','C','C','C','C','C','C','C','C','C','C','C','C',
'D','D','D','D','D','D','D','D','D','D','D','D','D','D',
'S','S','S','S','S','S','S','S','S','S','S','S','S','S'
};

char const* e_card_names[] = {
"ace_H.jpg",
"two_H.jpg",
"three_H.jpg",
"four_H.jpg",
"five_H.jpg",
"six_H.jpg",
"seven_H.jpg",
"eight_H.jpg",
"nine_H.jpg",
"ten_H.jpg",
"j_H.jpg",
"q_H.jpg",
"k_H.jpg",

"ace_C.jpg",
"two_C.jpg",
"three_C.jpg",
"four_C.jpg",
"five_C.jpg",
"six_C.jpg",
"seven_C.jpg",
"eight_C.jpg",
"nine_C.jpg",
"ten_C.jpg",
"j_C.jpg",
"q_C.jpg",
"k_C.jpg",

"ace_D.jpg",
"two_D.jpg",
"three_D.jpg",
"four_D.jpg",
"five_D.jpg",
"six_D.jpg",
"seven_D.jpg",
"eight_D.jpg",
"nine_D.jpg",
"ten_D.jpg",
"j_D.jpg",
"q_D.jpg",
"k_D.jpg",

"ace_S.jpg",
"two_S.jpg",
"three_S.jpg",
"four_S.jpg",
"five_S.jpg",
"six_S.jpg",
"seven_S.jpg",
"eight_S.jpg",
"nine_S.jpg",
"ten_S.jpg",
"j_S.jpg",
"q_S.jpg",
"k_S.jpg"
};


VGImage				Card::back_image;
struct image_info 	Card::back_ImageInfo;
char*				Card::CardBackFilename = NULL;


Card::Card	( char* mFilename, char Value, char suit )
:IconView()
{
	set_file( mFilename );	
	if (back_image==0)
	CardBackFilename = NULL;
}

Card::Card	( int  mIndex )
:IconView()
{
	char* filename = new char[80];
	strcpy  ( filename, "./games/card_images/" );
	int length = strlen(filename);
	strcat  ( filename, e_card_names[mIndex] );
	set_file( filename );

	is_face_down= false;
	value = e_card_values[mIndex];
	suit  = e_card_suits[mIndex];
	delete filename;
}

int Card::get_value( )
{
	switch(value)
	{
	case '2' : return 2;
	case '3' : return 3;
	case '4' : return 4;
	case '5' : return 5;
	case '6' : return 6;
	case '7' : return 7;
	case '8' : return 8;
	case '9' : return 9;
	case 'T' : return 10;	
	case 'J' : return 10;
	case 'Q' : return 10;	
	case 'K' : return 10;	
	case 'A' : return 11;	// 11 or 1 
	default  : return -1;
	}
}
#include <algorithm>
#include <math.h>

//#define min(X,Y) (((X) < (Y)) ? : (X) : (Y))

int	Card::draw( )
{
	//print_positions();
	if (is_face_down==true)
	{
		//Control::draw();	
		Fill_l( background_color );
		Rect  ( left, bottom, +width, +height );	
		VGfloat l = left;    //+left_margin;
		VGfloat b = bottom;  //+bottom_margin;
		unsigned int w = trunc(width);
		unsigned int h = trunc(height);
		int min_w = std::min(back_ImageInfo.width,  w );
		int min_h = std::min(back_ImageInfo.height, h);
		if (back_image != NULL)
			vgSetPixels(l, b, Card::back_image, 0, 0, min_w, min_h);		
	} else {
		IconView::draw();
	}
	return 1;
}

const char red_back[] = "./games/card_images/card_back_red.jpg";

int	Card::onCreate(  )
{
	//printf("Card::oncreate  \n");
	if (Card::CardBackFilename==NULL)
	{
		int len = strlen(red_back);
		Card::CardBackFilename = new char[len];
		strcpy( Card::CardBackFilename, red_back );
		//printf("%s\n", Card::CardBackFilename ); 
		back_image = createImageFromJpeg( Card::CardBackFilename, &Card::back_ImageInfo );		
	}
	load_resource();
	match_image_size();
	return 1;
}

