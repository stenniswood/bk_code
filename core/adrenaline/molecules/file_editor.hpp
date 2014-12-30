#ifndef _FILE_EDITOR_
#define _FILE_EDITOR_

#include "control.hpp"
#include <vector_file>
using namespace std;

class FileEditor : public Control 
{
public:
	FileEditor();
	FileEditor( Rectangle* mRect );
	FileEditor( int Width, int Height );
	~FileEditor();
	
	virtual void 	Initialize(	);

	void			open_directory( string mPath );	// all files in 1 directory.
	void			open_file	( string mFilename );
	void			close_file	(  );

	virtual int   	draw 		(	);	
	virtual int		onClick(int x, int y, bool mouse_is_down=true);
	
protected:
	ListBox*			m_open_documents;	//
	vector<string>		m_file_names;		// 1 to 1 on the ListBox.

	vector<TextView*>	m_editor;		// change to TextEdit!

};

 
#endif
