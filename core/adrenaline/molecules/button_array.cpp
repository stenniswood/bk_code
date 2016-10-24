// line graph OpenVG program
// Steve Tenniswood
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "VG/openvg.h"
#include "VG/vgu.h"
#include "/home/pi/openvg/fontinfo.h"
#include "/home/pi/openvg/shapes.h"
//#include "CAN_Interface.h"
#include "line_graph.hpp"
#include "dataset.hpp"
#include "histogram.hpp"
#include "line_graph.hpp"
#include "leveler.hpp"
#include "display.h"
#include "display_manager.hpp"
#include "button.hpp"
#include "listbox.hpp"
#include "checkbox.hpp"
#include "progressbar.hpp"
#include "button_array.hpp"
//#include "window_layouts.hpp"



#define Debug 1
#define Red 0xFFFF0000


ButtonArray::ButtonArray()
{
	m_rows = 0;
	m_cols = 0;
}

ButtonArray::ButtonArray(int rows, int cols )
{
	set_rows_cols(rows,cols);
}

ButtonArray::ButtonArray( int Left, int Right, int Top, int Bottom )
:Control( Left, Right, Top, Bottom )
{
}

void ButtonArray::set_rows_cols( int rows, int  mCols )
{
	m_rows = rows;
	m_cols = mCols;
}

ButtonRC::ButtonRC()
{
}
ButtonRC::ButtonRC( int Left, int Right, int Top, int Bottom )
:Button(Left,Right,Top,Bottom)
{
}
/*void ButtonArray::button_array_callback( void* mVoid )
{
	Button* ba = (Button*) mVoid;
	for (int r=0; r < m_rows; r++)
		for (int c=0; c < m_cols; c++)
		{
			if (m_buttons[r][c] == ba)
				m_callback( r,c );
		} 
}*/

void ButtonArray::create()
{
	float BWidth  = (width) / m_cols;	
	float BHeight = (height) / m_rows;	
	Button* tmp = NULL;
	vector<Button*> tmpB;
	
	// First, free up old buttons.  No memory leaks!
	for (size_t r=0; r<m_rows; r++)
		for (size_t c=0; c<m_cols; c++)
		{
			if ( r<m_buttons.size() )
				if ( c<m_buttons[r].size() )
					if (m_buttons[r][c])
						delete m_buttons[r][c]; 
		}
	for (size_t r=0; r<m_rows; r++) 
	{
		tmpB.clear();
		for (size_t c=0; c<m_cols; c++)
		{
			tmp = new Button( left+c*BWidth, left+(c+1)*BWidth, bottom+(r+1)*BHeight, bottom+(r)*BHeight );
			//tmp->set_on_click_listener( button_array_callback, tmp );
			register_child( tmp );
			tmpB.push_back( tmp );
		}
		m_buttons.push_back( tmpB );
	}
}

void ButtonArray::place	( )
{
	for (uint16_t r=0; r<m_rows; r++) 
		for (uint16_t c=0; c<m_cols; c++)
		{
			//m_buttons[r][c].move_to(  );			
		}	
}
	
// chance to load resources, call functions which use fonts (already loaded before this call) etc.
int	ButtonArray::onCreate (  )
{
	int retval = Control::onCreate();
	printf("ButtonArray::onCreate\n");
//	create();
	return retval;
}
void ButtonArray::set_button_text_row( int row, vector<string>&  mTexts )
{
	vector<Button*>::iterator iter   = m_buttons[row].begin();
	vector<string>::iterator source = mTexts.begin();
	while (iter != m_buttons[row].end() )
	{
		(*iter)->set_text( source->c_str(),false );
		iter++;
		source++;
	}
}

void ButtonArray::set_callback( void (*callback)(int,int) )
{
	m_callback = callback;
}

int ButtonArray::draw( )
{
	return Control::draw();
}

int	ButtonArray::onClick( int x, int y, bool mouse_is_down )
{
	Control* result = ChildrenHitTest(x,y);
	if (result)
	{
		size_t r,c;
		for (r=0; r<m_rows; r++) 
			for (c=0; c<m_cols; c++)
			{
				if (m_buttons[r][c] == result)
				{
					//printf("Clicked a registered child: r,c=%d,%d\n", r,c );
					m_callback(r,c);
					//result->onClick(x,y,mouse_is_down);
				}
			}
		return 1;
	}
	return -1;
}

Control* ButtonArray::HitTest(int x, int y)
{
	if (Visible==false) return NULL;

	/*Control* result = ChildrenHitTest( x, y );
	if (result) {
		return result;
	}*/

	float right = left+width;
	float top   = bottom+height;
	
	if ((x>left) && (x<right) && (y>bottom) && (y<top))
	{
	   return this;
	   // could add in a border check.  ie. if with 2 pixels of left or right, etc
	} else	
		return NULL;
}


