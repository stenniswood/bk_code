#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include <ctype.h>
#include "VG/openvg.h"
#include "VG/vgu.h"
#include <jpeglib.h>
#include "VG/openvg.h"
#include "VG/vgu.h"
#include "EGL/egl.h"
#include "GLES/gl.h"
#include "bcm_host.h"
#include <fontinfo.h>
#include <shapes.h>

#include "adrenaline_windows.h"
#include "application.hpp"



Application::Application () 
{ 
	Initialize();
}
Application::Application ( Rectangle* mRect ) 
{ 
	Initialize();
}
Application::Application ( int Width, int Height ) 
{ 
	Initialize();
}
Application::~Application() 
{ 
}

void 	Application::Initialize(	) 
{ 

}

int		Application::calc_metrics() 
{ 
}

// chance to load resources, call functions which use fonts
int	Application::onCreate	(  ) 
{ 
	MainDisplay.set_menu( &m_hMenu );
//	DisplayMain.load_sidebar	(		);
	MainDisplay.m_status.set_text( "Closed app" );
	
}	// First Time application is run - load resources etc.

int		Application::onActivate	() 
{ 
}		// Future reactivation - set menus, sidebars, status, etc.

// Standard Menu Handlers:	
void	Application::file_new		() 
{ 
}
void	Application::file_open		() 
{ 
}	
void	Application::file_open_recent() 
{ 
}	
void	Application::file_save		() 
{ 
}
void	Application::file_save_as	() 
{ 
}
void	Application::file_quit	() 
{
	MainDisplay.set_menu		( NULL 	);
//	MainDisplay.unload_sidebar	(		);
	MainDisplay.m_status.set_text( "Closed app" );
}

int		Application::onClick(int x, int y, bool mouse_is_down=true) 
{ 
}
int   	Application::draw		 	(	) 
{ 
}
