// line graph OpenVG program
// Steve Tenniswood
#include <shapes.h>
#include <fontinfo.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <math.h>

#include "VG/openvg.h"
#include "VG/vgu.h"
#include <shapes.h>
#include <fontinfo.h>
#include "CAN_Interface.h"

#include <linux/input.h>

#include "dataset.hpp"
#include "display.h"
#include "adrenaline_windows.h"
#include "adrenaline_graphs.h"
#include "display_manager.hpp"
#include "client_memory.hpp"
#include "client_list_control.hpp"
#include "keyboard.hpp"
#include "bk_system_defs.h"

#define KEY_WIDTH  50
#define KEY_HEIGHT 50
#define KEY_SPACING_COL 64
#define KEY_SPACING_ROW 64

#define Debug 1


void close_keyboard_cb( void* mKeyboard )
{
	printf("close_keyboard_cb()\n");
	Keyboard* ptr = (Keyboard*) mKeyboard;
	ptr->hide();
	ptr->Invalidate();
	MainDisplay.draw();	
}

Keyboard::Keyboard( int Left,  int Right, int Top, int Bottom )
:Window(Left, Right, Top,Bottom)
{
	Initialize();	
}

Keyboard::Keyboard()
{
	Initialize();
}

Keyboard::~Keyboard()
{

}


void	Keyboard::initialize_keys()
{
	dprintf("Initilizing Keyboard\n");
	struct stKey key;
	key.width  = KEY_WIDTH;
	key.height = KEY_HEIGHT;		// we'll adjust the height to match the 16:9 aspect ratio for which the data is intended.	

	// ROW #4 :  Special Row		// #0	NUM_LOCK
	key.x  = 5 + KEY_SPACING_COL/2 + KEY_WIDTH/2;
	key.y  = 10;
	strcpy(key.text, "123");
	m_keys.push_back(key);

	key.x += KEY_SPACING_COL;		// #1	SPACE Bar
	key.width = 2*KEY_WIDTH;
	strcpy(key.text, " ");
	m_keys.push_back(key);

	key.x += KEY_SPACING_COL + KEY_WIDTH;
	key.width = 2*KEY_WIDTH;
	strcpy(key.text, "Return");
	m_keys.push_back(key);

	key.width  = KEY_WIDTH;

	// ROW #3 : 
	char array3[] = { '^', 'Z','X','C','V','B','N','M', ',', '.' };	
	//if (Debug) printf("===SHIFT_KEY should be %d !!\n", m_keys.size() );
	key.x  = 5+KEY_SPACING_COL/2+KEY_WIDTH/2;
	key.y += (KEY_SPACING_ROW);
	for (int col=0; col<10; col++)
	{
		key.x += KEY_SPACING_COL;
		key.text[0] = array3[col];
		key.text[1] = 0;
		m_keys.push_back(key);
	}

	// ROW #2 : asdfghjkl
	char array2[] = { 'A','S','D','F','G','H','J','K','L', '<=' };
	key.x  = 5+KEY_SPACING_COL/2;
	key.y += (KEY_SPACING_ROW);
	for (int col=0; col<10; col++)
	{
		key.x += KEY_SPACING_COL;
		key.text[0] = array2[col];
		key.text[1] = 0;
		m_keys.push_back(key);
	}
	strcpy( m_keys[m_keys.size()-1].text, "<=" );
	// BACK_SPACE_KEY = m_keys.size()-1; 22

	// ROW #1 : ^ ZXCVBNM 
	char array1[] = { 'Q','W','E','R','T','Y','U','I','O','P' };	
	key.x = 5;
	key.y += KEY_SPACING_ROW;
	for (int col=0; col<10; col++)
	{
		key.x += KEY_SPACING_COL;
		key.text[0] = array1[col];
		key.text[1] = 0;
		m_keys.push_back(key);
	}
}

void	Keyboard::initialize_alt_keys()
{
	dprintf("Initilizing Symbol Keyboard\n");
	struct stKey key;
	key.width  = KEY_WIDTH;
	key.height = KEY_HEIGHT;		// we'll adjust the height to match the 16:9 aspect ratio for which the data is intended.	

	// ROW #4 :  Special Row		// #0	NUM_LOCK
	key.x  = 5 + KEY_SPACING_COL/2 + KEY_WIDTH/2;
	key.y  = 10;
	strcpy(key.text, "abc");
	m_alt_keys.push_back(key);


	// ROW #4 :  
	char array4[] = { '\\', '|', '-', '=', '_', '+',  ',', '.', '?' };	
	key.x  = 5+KEY_SPACING_COL/2+KEY_WIDTH/2;
	key.y  = 10;
	for (int col=0; col<9; col++)
	{
		key.x += KEY_SPACING_COL;
		key.text[0] = array4[col];
		key.text[1] = 0;
		m_alt_keys.push_back(key);
	}

	// ROW #3 : 
	char array3[] = { '<', '>',';',':','\'','\"','[',']', '{', '}' };	
	key.x  = 5;
	key.y += (KEY_SPACING_ROW);
	for (int col=0; col<10; col++)
	{
		key.x += KEY_SPACING_COL;
		key.text[0] = array3[col];
		key.text[1] = 0;
		m_alt_keys.push_back(key);
	}

	// ROW #2 : 
	char array2[] = { '!','@','#','$','%','^','&','*','(', ')' };
	key.x = 5;
	key.y += (KEY_SPACING_ROW);
	for (int col=0; col<10; col++)
	{
		key.x += KEY_SPACING_COL;
		key.text[0] = array2[col];
		key.text[1] = 0;
		m_alt_keys.push_back(key);
	}

	// ROW #1 :  
	char array1[] = { '1','2','3','4','5','6','7','8','9','0' };	
	key.x = 5;
	key.y += KEY_SPACING_ROW;
	for (int col=0; col<10; col++)
	{
		key.x += KEY_SPACING_COL;
		key.text[0] = array1[col];
		key.text[1] = 0;
		m_alt_keys.push_back(key);
	}
}

void Keyboard::Initialize()
{	
	initialize_keys();
	initialize_alt_keys();
	adjust_height();
	strcpy (class_name, "Keyboard");	
	
	m_close.set_on_click_listener( close_keyboard_cb, this );
	m_close.set_text(" . ", true);
	m_close.move_to( 8,8 );
	
	text_color       = 0xFF000000;
	background_color = 0xFF7F7FFF;
	key_color  		 = 0xFFFFFFFF;	
	text_size = 16;

	m_shift_down = false;
	m_alt_down   = false;
	m_index = 0;
	memset(m_composition, 0, 127);
	
	register_child( &m_close );
	dprintf("===Keyboard::Initialize()  Key_color = %x\n", key_color );	
}

void Keyboard::adjust_height()
{
	height = 5 * KEY_SPACING_ROW;
}

void Keyboard::draw_keys()
{
	char text[10];
	int size 		 = m_keys.size();
	// Draw the keyboard:
	for (int i=0; i<size; i++)
	{
		// Draw a filled rectangle:		
		Stroke_l   ( 0xFF000000 );
		Fill_l     ( key_color );		// background_color
		Roundrect( m_keys[i].x, m_keys[i].y, m_keys[i].width, m_keys[i].height, 15.0, 15.0 );

		// Print the letter:
		float x = m_keys[i].x + (m_keys[i].width)/2.0;	
		float y = m_keys[i].y + (m_keys[i].height - text_size)/2.0;	
		Stroke_l   ( 0xFFFF0000 );
		Fill_l ( text_color);
		strcpy ( text, m_keys[i].text );
		if (m_shift_down)
			text[0] = toupper(text[0]);
		else
			text[0] = tolower(text[0]);
		TextMid( x, y, text, SerifTypeface, text_size );
	}	
}
void Keyboard::draw_alt_keys()
{
	// Draw the keyboard:
	int size 		 = m_alt_keys.size();
	for (int i=0; i<size; i++)
	{
		// Draw a filled rectangle:		
		Stroke_l   ( 0xFFFFF00 );
		Fill_l     ( key_color );			// background_color
		Roundrect  ( m_alt_keys[i].x, m_alt_keys[i].y, m_alt_keys[i].width, m_alt_keys[i].height, 15.0, 15.0 );

		// Print the letter:
		float x = m_alt_keys[i].x + (m_alt_keys[i].width)/2.0;	
		float y = m_alt_keys[i].y + (m_alt_keys[i].height - text_size)/2.0;	
		Stroke_l   ( 0xFFFF0000 );
		Fill_l 	   ( text_color );
		char text[10];
		strcpy ( text, m_alt_keys[i].text );
		TextMid( x, y, text, SerifTypeface, text_size );
	}	
}
int Keyboard::draw()
{
	if (Visible==false)	 return 0;
	Window::draw();

	float sample_y   = height - (1.5*text_size)*2;
	
	StrokeWidth(2);			
	Stroke_l   ( 0xFF000000 );
	Fill_l     ( key_color );			// background_color
	Roundrect  ( 0, sample_y,  width, 1.5*text_size, 15.0, 15.0 );

	// Draw the text:
	Stroke_l   ( 0xFFFF0000 );
	Fill_l 	   ( text_color );
	Text	   ( 0, sample_y, m_composition, SerifTypeface, 16 );
	//if (Debug) printf("Keyboard::draw() %s\n", m_composition );		
	
	if (m_alt_down)
		draw_alt_keys();
	else 
		draw_keys();

	//dprintf("Keyboard::draw() done\n" );		
}

Control* Keyboard::HitTest( int x, int y )
{
	Control* retval = Window::HitTest(x,y);
	//dprintf("Keyboard::HitTest()  %x\n", retval); 
	return retval;
}

void Keyboard::set_composition	  ( char* mOriginalText )
{
	m_index = strlen( mOriginalText );
	strcpy(m_composition, mOriginalText );
}

void Keyboard::append_character	(char mChar)
{
	m_composition[m_index] 		= mChar;
	m_composition[m_index+1]    = 0; 
	m_index++;  
	dprintf("append_character()  m_index=%d; %s \n", m_index, m_composition);
}

int Keyboard::handle_special_keys( int mKeyIndex )
{
	if (mKeyIndex==ALT_KEY)
	{	m_alt_down = !m_alt_down;		return 1; }
	else if (mKeyIndex==SHIFT_KEY)
	{	m_shift_down = !m_shift_down;	return 1; }
	else if (mKeyIndex==RETURN_KEY)
	{
		append_character('\n');  
		MainDisplay.hide_keyboard();
		Invalidate();
		return 1; 
	}
	else if (mKeyIndex==BACK_SPACE_KEY)	{
		m_composition[--m_index]    = 0; 
		return 1; 
	}	
	return 0;
}


float Keyboard::geometric_distance( int x, int y, struct stKey key )
{
	float centroid_x = key.x + key.width/2.;
	float centroid_y = key.y + key.height/2.;
	
	float distance;
	distance = fabs(x - centroid_x) + fabs(y - centroid_y);
	return distance;
}

/* returns key index of closest */
int Keyboard::find_min_distance( int x, int y )
{
	int size = m_keys.size();
	if (m_alt_down)
		size = m_alt_keys.size();
		
	float min_distance = 1000;
	float distance     = 0;
	int   min_index    = 0;
	for (int i=0; i<size; i++) 
	{
		if (m_alt_down)
			distance = geometric_distance( x, y, m_alt_keys[i] );
		else 
			distance = geometric_distance( x, y, m_keys[i] );
			
		if (distance < min_distance)
		{
			min_distance = distance;
			min_index    = i;
			//dprintf("%d min_dist=%6.1f; %s\n", i, min_distance, m_keys[i].text );
		}
	}
	return min_index;	
}

char*	Keyboard::get_key_text ( int key_index )		// with current shift & alt states.
{
	static char key[8];

	if (m_alt_down)
	{
		strcpy(key, m_alt_keys[key_index].text );
		key[1] = 0;
		//key = m_alt_keys[key_index].text[0];	
	}
	else 
	{
		strcpy(key, m_keys[key_index].text );
		key[1] = 0;
		if (m_shift_down)	
			key[0] = toupper( key[0] );
		else 
			key[0] = tolower( key[0] );	
	}
	return key;
}
	
int Keyboard::onClick(int x, int y, bool mouse_is_down)
{
	int retval = Control::onClick(x,y,mouse_is_down); 
	if (retval>0) return retval;
	
	char* key_name;
	char key;
	//dprintf("Keyboard::onClick()  x,y = %d,%d\n", x, y );
	//if ((Debug) && m_alt_down) printf("===== ALT KEYBOARD ACTIVE ===\n");
	int closest_key = find_min_distance( x,y );

	int key_index = closest_key;
	key_name = get_key_text(closest_key);
	key = key_name[0];
	dprintf("Min Distance, closest key is %d = %s", closest_key, key_name );
	
	if (m_shift_down)	
		m_shift_down = false;	// just 1 key! 

	// hit - always will be with centroids
	Invalidate();
	int is_special_key = handle_special_keys( key_index );
	if (is_special_key)	 {
		MainDisplay.relay_key( key_index );
		return TRUE;
	} else
		MainDisplay.relay_key( key );

	append_character ( key );
	if (m_index>BUFFER_LENGTH)	m_index = BUFFER_LENGTH;
	
	dprintf(" Keyboard::xy=<%d,%d>; key_index=%d;  key:<%d,%d>  %c\n", x,y, key_index,
					m_keys[key_index].x, m_keys[key_index].y, m_keys[key_index].text[0] );												 
	dprintf(" Keyboard::%s\n", m_composition );

	return TRUE;
}


