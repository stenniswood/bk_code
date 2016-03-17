// line graph OpenVG program 
// Steve Tenniswood
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include "VG/openvg.h"
#include "VG/vgu.h"
#include <shapes.h>
#include <fontinfo.h>
#include "line_graph.hpp"
#include "display.h" 

#include "legend.hpp"

 
#define Debug 1 


Legend::Legend( int Left, int Right, int Top, int Bottom )
:Control(Left, Right, Top, Bottom)
{ 
	Initialize();
}
Legend::Legend()
{
	Initialize(); 
}
Legend::~Legend()
{ 
}

void  Legend::Initialize			(					)
{ 
//	Control::Initialize();
	text_size = 12;
}
void Legend::set_data_series		( long mColor, char* mLabel )
{ 
	m_color.push_back( mColor );
	m_names.push_back( mLabel );
}
void  Legend::reset	  	(  )
{
	m_color.clear();
	m_names.clear();
}
// This links the Datasets together!
void  Legend::add_data_series	  	( DataSet& NewData )
{ 
	m_color.push_back( NewData.color );
	m_names.push_back( NewData.name );
}
void  Legend::remove_data_series	( int index /*DataSet* OldData*/ )
{ 
	m_color.erase( m_color.begin()+index );
	m_names.erase( m_names.begin()+index );
}
void  Legend::calc_metrics			(					  )
{ 
}
const int LINE_LENGTH = 15;

int   Legend::draw			(	)
{ 
	Control::draw();
	float div = m_names.size();
	if (div==0.0) div = 1.0;
	float y_inc = height / (div+1);
	float y = bottom+height-y_inc;
	float margin = width * 0.05;
	for(int i=0; i<m_names.size(); i++)
	{
		Stroke_l ( m_color[i] );
		Fill_l   ( m_color[i]  );
		Line(left+margin, y, left+margin+LINE_LENGTH, y);		
		//printf("x1, x2,y=%6.1f %6.1f %6.1f; %x\n",left+margin, left+margin+LINE_LENGTH, y, m_color[i] );
				
		Text(left+margin+LINE_LENGTH, y, m_names[i].c_str(), SerifTypeface, text_size );
		y -= y_inc;
	}
}		// Draw calls all the sub draw routines.

