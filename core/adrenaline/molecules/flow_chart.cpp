#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string> 
#include <math.h>
#include "VG/openvg.h"
#include "VG/vgu.h"
#include <shapes.h>
#include <fontinfo.h>
#include "Graphbase.hpp"
#include "control.hpp"
#include "listbox.hpp"
#include "display.h"
#include "side_bar.hpp"
#include "flow_chart.hpp"



FlowChart::FlowChart( )
:Control()
{
}
FlowChart::FlowChart( int Left, int Right, int Top, int Bottom )
:Control( Left, Right, Top, Bottom )
{
}
FlowChart::FlowChart( int Width, int Height 					)
:Control(Width, Height)
{
}
FlowChart::~FlowChart()
:Control()
{
}

void 	FlowChart::Initialize(	)
{
}
int   	FlowChart::add_object			( stFlowObject* mObject )
{
	m_objects.push_back( *mObject );
}
int   	FlowChart::add_square			( float x, float y, char* text 	)
{
	struct stFlowObject fo;
	fo.x = x;  
	fo.y = y;
	fo.text = text;
	fo.shape = FLOW_SQUARE;
	add_object( &fo );
}
int   	FlowChart::add_circle			( float x, float y, char* text	)
{
	struct stFlowObject fo;
	fo.x = x;  
	fo.y = y;
	fo.text = text;
	fo.shape = FLOW_CIRCLE;
	add_object( &fo );
}
int   	FlowChart::add_diamond			( float x, float y, char* text	)
{
	struct stFlowObject fo;
	fo.x = x;  
	fo.y = y;
	fo.text = text;
	fo.shape = FLOW_DIAMOND; 
	add_object( &fo );
}
int   	FlowChart::add_parallelagram	( float x, float y, char* text	)
{
	struct stFlowObject fo;
	fo.x = x;  
	fo.y = y;
	fo.text = text;
	fo.shape = FLOW_PARALLEL;
	add_object( &fo );
}
int   	FlowChart::add_elispse 			( float x, float y, char* text	)
{
	struct stFlowObject fo;
	fo.x = x;  
	fo.y = y;
	fo.text = text;
	fo.shape = FLOW_ELIPSE;
	add_object( &fo );
}

int   	FlowChart::add_connector 		( stPoint2D mStart, stPoint2D mEnd	)
{
}
int   	FlowChart::remove_connector 	( 	)
{
}	
int   	FlowChart::draw 				(	)
{
}

int   	FlowChart::draw_object 			(	)
{
}
int   	FlowChart::draw_connector		(	)
{
}
int		FlowChart::set_on_click_listener( void (*callback)(void*), void* mOn_click_context )
{
}
int		FlowChart::onClick(int x, int y, bool mouse_is_down=true)
{
}


