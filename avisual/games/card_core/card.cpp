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

char const* e_card_names[] = {
"1_H.jpg",
"two_H.jpg",
"three_H.jpg",
"four_H.jpg",
"five_H.jpg",
"six_H.jpg",
"seven_H.jpg",
"eight_H.jpg",
"nine_H.jpg",
"ten_H.jpg",
"J_H.jpg",
"Q_H.jpg",
"K_H.jpg",
"A_H.jpg",

"1_C.jpg",
"two_C.jpg",
"three_C.jpg",
"four_C.jpg",
"five_C.jpg",
"six_C.jpg",
"seven_C.jpg",
"eight_C.jpg",
"nine_C.jpg",
"ten_C.jpg",
"J_C.jpg",
"Q_C.jpg",
"K_C.jpg",
"A_C.jpg",

"1_D.jpg",
"two_D.jpg",
"three_D.jpg",
"four_D.jpg",
"five_D.jpg",
"six_D.jpg",
"seven_D.jpg",
"eight_D.jpg",
"nine_D.jpg",
"ten_D.jpg",
"J_D.jpg",
"Q_D.jpg",
"K_D.jpg",
"A_D.jpg",

"1_S.jpg",
"two_S.jpg",
"three_S.jpg",
"four_S.jpg",
"five_S.jpg",
"six_S.jpg",
"seven_S.jpg",
"eight_S.jpg",
"nine_S.jpg",
"ten_S.jpg",
"J_S.jpg",
"Q_S.jpg",
"K_S.jpg",
"A_S.jpg"
};

Card::Card	( char* mFilename, char Value, char suit )
:IconView()
{
	set_file( mFilename );	
}

Card::Card	( int  mIndex )
:IconView()
{
	char* filename = new char[80];
	strcpy  ( filename, "./games/card_images/" );
	int length = strlen(filename);
	strcat  ( (char*) &(filename[length]), e_card_names[mIndex] );
	set_file( filename );
	delete filename;
}

int	Card::draw( )
{
	if (is_face_down==true)
	{
		Control::draw();
	} else {
		IconView::draw();
	}
}


