// line graph OpenVG program
// Steve Tenniswood
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include "VG/openvg.h"
#include "VG/vgu.h"
#include <shapes.h>
#include <fontinfo.h>
#include "Graphbase.hpp"
#include "control.hpp"
#include "listbox.hpp"
#include "display.h"
#include "table_window.hpp"


#define Debug 0

TableWindow::TableWindow ( )
:Window()
{
}

TableWindow::TableWindow ( int Left, int Right, int Top, int Bottom )
:Window(Left, Right, Top, Bottom)
{
}
TableWindow::~TableWindow( )
{
}

void TableWindow::Initialize(   )
{

}

int   	TableWindow::draw 			 (	 )
{
	list<Control*>::iterator	iter = controls.begin();
	for (int i=0; iter!=controls.end(); i++ )
	{
		(*iter)->draw();
	}
}

void	TableWindow::calc_metrics	 (   )
{

}

// This will add the window offsets before adding:
int	TableWindow::add_control_local_coords( Control* mControl, int mRow, int mCol )
{
	int dLeft   = left;
	int dBottom = bottom;
	mControl->move_by( -left, -bottom );
	controls.push_back( mControl );
}

int	TableWindow::add_control_absolute_coords( Control* mControl, int mRow, int mCol )
{
	controls.push_back( mControl );
}



void TableWindow::set_column_width( int mWidth  )	
{
	cols      = (width / (float)mWidth);
	col_width = mWidth;
}

void TableWindow::set_row_height	( int mHeight )	
{
	rows       = (height / (float)mHeight);
	row_height = mHeight;
}


Control*	TableWindow::HitTest	( int x, int y )
{
	Control* result = Control::HitTest(x,y);
	if (result)
	{
		// Disperse to affected child:
		list<Control*>::iterator	iter = controls.begin();
		for (int i=0; iter!=controls.end(); i++ )
		{
			return (*iter)->HitTest( x, y );
		}
	}
}



