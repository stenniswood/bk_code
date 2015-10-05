#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include <ctype.h>
#include <jpeglib.h>

#include "VG/openvg.h"
#include "VG/vgu.h"
#include "EGL/egl.h"
#include "GLES/gl.h"
#include "bcm_host.h"
#include <fontinfo.h>
#include <shapes.h>

#include "adrenaline_windows.h"
#include "file_editor_app.hpp"
#include "file_editor.hpp"


FileEditorApp* file_app = NULL;

void init_file_editor_app()
{
	if (file_app==NULL)	
		file_app = new FileEditorApp();
	if (file_app)
		file_app->register_with_display_manager();
}

FileEditorApp::FileEditorApp () 
{ 
}
FileEditorApp::FileEditorApp ( Rectangle* mRect ) 
{ 
}
FileEditorApp::~FileEditorApp() 
{ 
}

void 	FileEditorApp::Initialize		(	) 
{ 
	/*  Base class is initialized in the base class constructor.
		ie. The Application::Initialize is invoked there (not this one)
		Even though the function is virtual, for the base class,
		it calls the same level (base class) Initialize()
		Application::Initialize();	This will get called anyway!
		Therefore it is uneccessary and should not be put in.
	*/
	m_application_name = "File Editor";
	m_main_window = new FileEditor( );

	setup_app_menu();
	setup_main_menu    ();
	onPlace();	
}	// create all the objects here.

int		FileEditorApp::onPlace			(	) 
{ 
	Application::onPlace();
	((FileEditorApp*)m_main_window)->onPlace();	
}

void	FileEditorApp::setup_main_menu  	( ) 
{ 
	Application::setup_main_menu( );
}
void	FileEditorApp::setup_app_menu  ( ) 
{ 
	Application::setup_app_menu  ( ) ;
}
void 	FileEditorApp::register_with_display_manager() 
{ 
	Application::register_with_display_manager();
}	

int		FileEditorApp::About			(	) 
{ 
	Application::About			(	);
}
int		FileEditorApp::Preferences		(	) 
{ 
	Application::Preferences		(	); 
}

void	FileEditorApp::file_new		( ) 
{ 
}
void	FileEditorApp::file_open		( ) 
{ 
}	
void	FileEditorApp::file_open_recent( ) 
{ 
}	
void	FileEditorApp::file_save		( ) 
{ 
}
void	FileEditorApp::file_save_as	( ) 
{ 
}

