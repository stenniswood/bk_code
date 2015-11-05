// line graph OpenVG program
// Steve Tenniswood
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <string> 
#include <math.h>
#include <dirent.h> 
#include <sys/stat.h>
#include <errno.h>
#include "VG/openvg.h"
#include "VG/vgu.h"
#include <shapes.h>
#include <fontinfo.h>
#include "Graphbase.hpp"
#include "control.hpp"
#include "listbox.hpp"
#include "display.h"
#include "icon.hpp"
#include "tabular_listbox.hpp"
#include "directory_listbox.hpp"
#include "resource_manager.hpp"



ResourceManager::ResourceManager()
{
	
}

void ResourceManager::load_icon_directory( string mPath )
{
	bool show_hidden_files = false;
	DIR           *d;
	struct dirent *dir;
	d 	= opendir(mPath.c_str());  
	string 				name = NULL;
	bool				is_hidden_file = false;

	if (d)	{
		// First, go thru all FILES:
		while ((dir = readdir(d)) != NULL)
		{
			if (dir->d_type==DT_REG)  
			{
				is_hidden_file = (dir->d_name[0]=='.')?true:false;
				if ((show_hidden_files==false) && (is_hidden_file))
				{
					continue;		// skip!
				}
				name =  dir->d_name;
				m_image_names.push_back( name );
				m_icon_id.push_back( FILE_ID );
			}        
		}
		//rewinddir(d);
		closedir(d);
	}
}

void ResourceManager::load_sound_clips( string mPath )
{

}

