#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include <ctype.h>
#include <jpeglib.h>
#include <dirent.h> 
#include <sys/stat.h>
#include <errno.h>
#include "global.h"

#include "VG/openvg.h"
#include "VG/vgu.h"
#include "EGL/egl.h"
#include "GLES/gl.h"
#include "bcm_host.h"
#include <fontinfo.h>
#include <shapes.h>
#include "tabular_listbox.hpp"

//#include "adrenaline_windows.h"
//#include "file_editor_app.hpp"
#include "file_editor.hpp"
#include "rectangle.hpp"


FileEditor::FileEditor() 
{ 
	Initialize();
}

FileEditor::FileEditor( Rectangle* mRect ) 
: TextView()
{ 
	Control::set_position(mRect);
	Initialize();	
}

FileEditor::FileEditor( int Width, int Height ) 
: TextView( Width,  Height)
{ 
	Initialize();
}

FileEditor::~FileEditor() 
{ 
}

void 	FileEditor::Initialize(	) 
{ 
	Control::Initialize();
}


void	FileEditor::open_directory( string mPath ) 
{ 
	DIR           *d;
	struct dirent *dir;
	d 	= opendir(mPath.c_str());  
	vector<string>* 	line_text = NULL;
	bool				is_hidden_file = false;

	if (d)
	{
		// First, go thru all FILES:
		while ((dir = readdir(d)) != NULL)
		{
			if (dir->d_type==DT_REG)  
			{
				is_hidden_file = (dir->d_name[0]=='.')?true:false;
				//if ((show_hidden_files==false) && (is_hidden_file))
				if (is_hidden_file)
				{
					continue;		// skip!
				}
				line_text = new vector<string>;
				line_text->push_back( dir->d_name );
				if (m_open_documents)
					m_open_documents->add_row( line_text );
				//icon_id.push_back( FILE_ID );
			}        
		}
		//rewinddir(d);
		closedir(d);
		//calc_widths_from_text();
		m_open_documents->calc_column_positions_from_widths();
	}
}	// all files in 1 directory.

void	FileEditor::open_file	( string mFilename ) 
{ 
}

void	FileEditor::close_file	(  ) 
{ 
}

/*int   	FileEditor::draw 		(	) 
{ 
}	

int		FileEditor::onClick(int x, int y, bool mouse_is_down) 
{ 
}
*/