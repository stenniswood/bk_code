







FileEditor::FileEditor() 
: Control()
{ 
}

FileEditor::FileEditor( Rectangle* mRect ) 
: Control(Rectangle* mRect)
{ 
}

FileEditor::FileEditor( int Width, int Height ) 
: Control(int Width, int Height)
{ 
}

FileEditor::~FileEditor() 
{ 
}

void 	FileEditor::Initialize(	) 
{ 
	Control::Initialize();
}


void			FileEditor::open_directory( string mPath ) 
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
				if ((show_hidden_files==false) && (is_hidden_file))
				{
					continue;		// skip!
				}
				line_text = new vector<string>;
				line_text->push_back( dir->d_name );
				add_row( line_text );
				icon_id.push_back( FILE_ID );
			}        
		}
		//rewinddir(d);
		closedir(d);
		//calc_widths_from_text();
		calc_column_positions_from_widths();
	}
}	// all files in 1 directory.

void			FileEditor::open_file	( string mFilename ) 
{ 
}

void			FileEditor::close_file	(  ) 
{ 
}

int   	FileEditor::draw 		(	) 
{ 
}	

int		FileEditor::onClick(int x, int y, bool mouse_is_down=true) 
{ 
}
