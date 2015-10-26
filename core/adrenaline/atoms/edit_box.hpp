#ifndef _EDIT_TEXT_H_
#define _EDIT_TEXT_H_

#include "text_view.hpp"

/*

*/

#define TINY_EDIT_TEXT 			40
#define SMALL_EDIT_TEXT 		255
#define MEDIUM_EDIT_TEXT 		16384
#define LARGE_EDIT_TEXT 		65535
#define EXTRA_LARGE_EDIT_TEXT 	1000000


class EditBox : public TextView
{
public:
	EditBox( int Left, int Right, int Top, int Bottom );
	EditBox( long int mSizeCatagory = SMALL_EDIT_TEXT);
//	EditBox( );
	
	virtual void 	Initialize	(	);
	int				calc_metrics(   );
	void			insert		( char mKey );
	void			backspace	(  );
	void			delete_chars( int mNumber );	// from cursor position!
	void			Realloc		( long int mSize );
	
	virtual int		onKey	  	( char mKey 		);
	virtual int		onClick		( int x, int y, bool mouse_is_down=true );
	
	char*		m_cursor;	
	int			m_capacity;
	
};



#endif
