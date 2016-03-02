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
	
	virtual void 	Initialize	(	);
	int				calc_metrics(   );
	void			insert		( char* mKey   );
	void			backspace	(  			  );
	void			delete_chars( int mNumber );	// from cursor position!
	void			Realloc		( long int mSize );
	virtual void  	set_text  	( const char* NewText, bool mWrapContent=false );	// Allocates and copies!
	char*			get_text	( );

			void	set_return_key_listener( void (*callback)(void*), void* mReturnCallbackContext	);
		
	virtual int		onReceiveFocus( 		 	);
	virtual int		onEndFocus	( 			 	);
	
	virtual int		onKey	  	( char mKey 	);
	virtual int		onClick		( int x, int y, bool mouse_is_down=true );
//	virtual int		onFlinger	( FlingerEvent mFlinger );

	size_t		m_length;
	char*		m_cursor;	
	char*		m_selection_start;	
	char*		m_selection_end;	
	int			m_capacity;
	bool		m_first_click;
	
private:
	void 	 (*on_return_callback)(void*);	// When Return key is pressed on the keyboard.
	void* 	 on_return_context;				// often this pointer to a Control*

};



#endif
