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



CANApp::CANApp() 
{ 
}
CANApp::CANApp( int Left, int Right, int Top, int Bottom ) 
{ 
}
CANApp::CANApp( int Width, int Height 					 ) 
{ 
}
CANApp::~CANApp() 
{ 
}
void 	CANApp::Initialize(	) 
{ 
}
int		CANApp::calc_metrics() 
{ 
}
int		CANApp::place_views()
{
}
int		CANApp::handle_incoming_msg	( struct sCAN* msg ) 
{ 
	struct sCAN* result = m_filter_view.filter_incoming_msg( msg );
	if (result)	
		m_msg_view.add_message( msg );
	// test if it's a trigger for another message.
	//     then send response	
	
}
int		CANApp::setup_periodic_msg	( struct sCAN* msg, int mTimePeriod_ms ) 
{ 
}
int		CANApp::setup_triggered_response_msg( struct sCAN* mTriggerMsg, struct sCAN* mResponse ) 
{ 
}
int   	CANApp::draw		 		(				) 
{ 
}
int		CANApp::get_hit_index		( int Mousex, int Mousey ) 
{ 
}
int		CANApp::onClick(int x, int y, bool mouse_is_down=true) 
{ 
}
