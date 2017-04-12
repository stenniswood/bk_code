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

#include "control.hpp"
#include "button.hpp"
#include "display.h"
#include "text_view.hpp"
#include "edit_box.hpp"
#include "display_manager.hpp"


#define Debug 1
#define Printf if (Debug) printf

EditBox::EditBox( int Left, int Right, int Top, int Bottom )
:TextView( Left, Right, Top, Bottom )
{
	m_capacity = SMALL_EDIT_TEXT;
	Initialize();
}

EditBox::EditBox( long int mSizeCatagory )
:TextView( )
{
	m_capacity = mSizeCatagory;
	Initialize();
}

/*EditBox::EditBox( )
:TextView( )
{
	if (m_capacity==-1)		m_capacity = SMALL_EDIT_TEXT;
	Initialize();
}*/

void EditBox::Realloc( long int mSize )
{
	if (text)	delete text;
	m_capacity = mSize;
	text = new char[m_capacity];
}

void EditBox::Initialize(	)
{
	TextView::Initialize();
	strcpy (class_name, "EditBox");	
	//printf("EditBox::Initialize\n");
	text = new char[m_capacity];
	background_color = 0xFF000000;
	text_color		 = 0xFFFFFFFF;
	m_cursor		 = text;
	m_selection_start = m_cursor;
	m_selection_end   = m_cursor;	
	
}

int	EditBox::calc_metrics()
{
	return 0;
}

// Allocates and copies!
void EditBox::set_text( const char* NewText, bool mWrapContent )
{
	size_t length = strlen(NewText);
	if (length> (m_capacity/2.))   {
		m_capacity = length * 2;
		free(text);
		text = new char[length];
	}	
	strcpy(text, NewText);
//	if (mWrapContent)
//		wrap_content();
}

char* EditBox::get_text( )
{
	return text;
}

void EditBox::set_return_key_listener( void (*callback)(void*), void* mReturnCallback )
{
	on_return_callback = callback;
	on_return_context  = mReturnCallback;
}


/* Insert char */ 
void EditBox::insert( char* mKey )
{
	long int insert_length = strlen(mKey);	
	long int length        = strlen(text);
	int index_into_text    = (m_cursor-text);
	int bytes_to_shift     = (length-index_into_text)+1;
	
	printf("EditBox::insert()  inserting %ld into %ld, cursor at %d; remaining Bytes=%d\n", 
				insert_length, length, index_into_text, bytes_to_shift  );
	
	memmove( m_cursor+insert_length, m_cursor, bytes_to_shift );
	printf("EditBox::insert  text =%s\n", text);
	
	char tmp = m_cursor[insert_length];
	strcpy( m_cursor, mKey );
	m_cursor[insert_length] = tmp;
	printf("EditBox::insert  text =%s\n", text);
	m_cursor+=insert_length;
}

void EditBox::backspace(  )
{
	long int length = strlen(text);
	m_cursor--;
	memcpy( m_cursor, m_cursor+1, length );
}

void EditBox::delete_chars( int mNumber )	// from cursor position!
{
	long int length = strlen(text);
	memcpy( m_cursor, m_cursor+mNumber, length );	
}

int EditBox::onKey( char mKey )
{
	char tmp[2];
	tmp[0]=mKey;
	tmp[1]='\0';		// end of string.
	
	if (Debug) printf("EditBox::onKey ( %x = %d = %c )\n", mKey, mKey, mKey );
	if (mKey == BACK_SPACE_KEY)	{
		if (Debug) printf("EditBox::onKey BACKSPACE  %d %c \n", mKey, mKey);
		backspace();
	} else if (mKey == RETURN_KEY) {
		printf("RETURN_KEY pressed calling callback\n");
		on_return_callback( on_return_context );
	} else
		insert( tmp );
	if (Debug) printf("EditBox::%s|\n", text );
	Invalidate();
	return 1;
}

int	EditBox::onReceiveFocus( 		 	)
{
	return 0;
}
int	EditBox::onEndFocus	( 			 	)
{
	return 0;
}

int	EditBox::onClick( int x, int y, bool mouse_is_down )
{
	Printf("EditBox::onClick()\n");
	
	if (text) {
		m_length = strlen(text);	
		m_cursor = text+m_length;
	}
	if (m_first_click)
	{	
		// select all text : 

		m_selection_start = text;
		m_selection_end   = text + m_capacity-1;
		m_first_click 	  = false;
	}

	MainDisplay.grab_focus( this );
	m_keyboard.set_composition( text );
	MainDisplay.show_keyboard();
	return 1;	
}

///////////////// TEXT VIEW CODE MODIFIED TO HIGHLIGHT SELECTION ONLY /////////////////
/*
mVerticalPix - is height relative to bottom of the control.

char* EditBox::draw_one_line( char* mtext, int mVerticalPix )
{
	char* eol = get_end_of_line( mtext );
	char tmp = *eol;
	*eol = 0;

	if (style & CENTER_HORIZONTAL)
	{
		VGfloat font_width = TextWidth( mtext, *font, text_size );
		int space = (width-(font_width))/2;		
		Text(left+space,  bottom+mVerticalPix, mtext, *font, text_size );
	} else {
		Text(m_left_margin, bottom+mVerticalPix, mtext, *font, text_size );
	}
	*eol = tmp;
	return eol;
}

void  TextView::set_text_size( float TextSize )
{
	ScrollControl::set_text_size( TextSize );
	calc_metrics();
}
*/
/********************************************************************
 Comment on RoundRect & TextMid!!!!
	RoundRect Uses width / height as 2nd and 3rd parameters.
	Text characters come out the color of the Fill, not stroke!!
*********************************************************************
int EditBox::draw()
{
	ScrollControl::draw();
 
	if (height==0) 	return 1;
	if (text==NULL) return 1;

	Fill_l(text_color);					
	line_height 	= 1.25*text_size;

	
	int vertical    = height-line_height;
	char* ptr       = text-1;
	
	if (style & CENTER_VERTICAL)
	{
		int num_lines   = count_num_lines_present();
		int text_height = (num_lines*line_height);
		// we print from the top down, and vertical is base of the first line.
		vertical = (height- text_height) / 2.0 + (text_height-line_height);	
	}
	int total_length = strlen(text);

	do {
		ptr = draw_one_line( ptr+1, vertical );

		vertical -= line_height;
		if (vertical < 0)
			break;

	} while ( (ptr-text) < total_length ); 
} */