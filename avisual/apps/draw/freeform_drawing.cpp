/*******************************************************
* Line graph OpenVG program
* 
* Steve Tenniswood
********************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include "VG/openvg.h"
#include "VG/vgu.h"
#include <jpeglib.h>
#include "VG/openvg.h"
#include "VG/vgu.h"
#include "EGL/egl.h"
#include "GLES/gl.h"
#include "bcm_host.h"

#include <string.h>
#include <fontinfo.h>
#include <shapes.h>
#include "Graphbase.hpp"
#include "control.hpp"
#include "button.hpp"
#include "display.h"
#include "icon.hpp"



FreeFormDrawing::FreeFormDrawing()
:Control()
{
}

FreeFormDrawing::FreeFormDrawing( int Left, int Right, int Top, int Bottom )
:Control(Left, Right, Top, Bottom)
{
}

FreeFormDrawing::FreeFormDrawing( int Width, int Height )
:Control(Width, Height)
{
}

FreeFormDrawing::~FreeFormDrawing()
{
}
	
void 	FreeFormDrawing::Initialize	(	)
{
}

	commands[0] = VG_MOVE_TO_ABS;
	commands[1] = VG_LINE_TO_ABS;
	commands[2] = VG_LINE_TO_ABS;
	commands[3] = VG_LINE_TO_ABS;
	commands[4] = VG_LINE_TO_ABS;
	commands[5] = VG_LINE_TO_ABS;
	commands[6] = VG_LINE_TO_ABS;
	commands[7] = VG_LINE_TO_ABS;
	commands[8] = VG_LINE_TO_ABS;
	commands[9] = VG_LINE_TO_ABS;
	commands[10]= VG_LINE_TO_ABS;
	
	coords[0]   = 12.0;		coords[1] = 24.0;
	coords[2]   = 100.0;	coords[3] = 350.0;
	coords[4]   = 150.0;	coords[5] = 400.0;
	coords[6]   = 200.0;	coords[7] = 500.0;
	coords[8]   = 300.0;	coords[9] = 350.0;
	coords[10]  = 125.0;	coords[11] =200.0;
	coords[12]  = 900.0;	coords[13] =250.0;
	coords[14]  = 1080.0;	coords[15] =200.0;
	coords[16]  = 500.0;	coords[17] =100.0;
	coords[18]  = 12.0;		coords[19] =24.0;

int   	FreeFormDrawing::draw 		(	)
{
	Control::draw();
	list<vector<VGuint>>::iterator   SCiter    = m_stroke_color.begin();
	list<vector<VGufloat>>::iterator SWiter    = m_stroke_widths.begin();
	list<vector<VGubyte>>::iterator  Cmditer   = m_commands.begin();
	list<vector<VGufloat>>::iterator Coorditer = m_coords.begin();

	for ( ; Coorditer!=m_coords.end(); )
	{
		int numCmds   = Cmditer.size();
		int numCoords = Coorditer.size();		
		VGPath path = vgCreatePath(VG_PATH_FORMAT_STANDARD,
							VG_PATH_DATATYPE_F,
							1.0f, 0.0f, 		// scale,bias
							numCmds, numCoords,
							VG_PATH_CAPABILITY_ALL);
		StrokeWidth	( *SWiter 	 );
		Stroke_l	( 0xFFFF00FF );

		vgAppendPathData(path, numCmds, commands, Coorditer->data() );
		vgDrawPath		(path, VG_STROKE_PATH			);		

		SCiter++;		SWiter++;
		Cmditer++;		Coorditer++;
	}
}

int		FreeFormDrawing::onClick	(int x, int y, bool mouse_is_down)
{
	float tmp;
	struct stPoints pts;
	if (mouse_is_down) {
		tmp = x;	m_current_coords.push_back( tmp );
		tmp = y;	m_current_coords.push_back( tmp );
	} else {		
		int size        = m_current_coords.size();
		m_strokewidths.push_back( 1.0 );
		vector<VGubyte> cmds;

		// First create a command set:
		cmds.push_back( VG_MOVE_TO_ABS );
		for (int i=1; i<size; i++)
			cmds.push_back( VG_LINE_TO_ABS );
		// Then add it to the list:
		m_commands.push_back( cmds );
		
		// Also add the coords to the list : 
		m_coords.push_back( m_current_coords );
	}
}

