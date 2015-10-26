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
#include "edit_box.hpp"
#include "display_manager.hpp"


#define Debug 0

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
	//printf("EditBox::Initialize\n");
	text = new char[m_capacity];
	background_color = 0xFF000000;
	text_color		 = 0xFFFFFFFF;
	m_cursor		 = text;
}

int	EditBox::calc_metrics()
{
	return 0;
}

void EditBox::insert( char mKey )
{
	long int length = strlen(text);
	memcpy( m_cursor+1, m_cursor, length );
	*m_cursor = mKey;
	m_cursor++;
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
	memcpy( m_cursor, m_cursor+1, length );	
}

int EditBox::onKey( char mKey )
{
	if (Debug) printf("EditBox::onKey ( %x = %d = %c )\n", mKey, mKey, mKey );
	if (mKey == BACK_SPACE_KEY)	{
		if (Debug) printf("EditBox::onKey BACKSPACE  %d %c \n", mKey, mKey);
		backspace();
	} else
		insert( mKey );
	if (Debug) printf("EditBox::%s|\n", text );
	Invalidate();
}

int	EditBox::onClick( int x, int y, bool mouse_is_down )
{
	MainDisplay.grab_focus( this );
	MainDisplay.show_keyboard();
	return 1;	
}

