#ifndef _KEYBOARD_
#define _KEYBOARD_

#include <vector>
#include "window.hpp"

const int ALT_KEY    = 0;
const int SHIFT_KEY  = 3;
const int RETURN_KEY = 2;
const int BACK_SPACE_KEY = 22;

const int BUFFER_LENGTH = 255;

struct stKey
{
	int x;
	int y;
	int width;
	int height;
	char text[8];		// A, B, C, Return, etc
};

class Keyboard : public Window
{
public:
	Keyboard( int Left,  int Right, int Top, int Bottom );
	Keyboard();
	~Keyboard();

	void	initialize_alt_keys	();	
	void	initialize_keys		();
	void	Initialize			();
	void	adjust_height		();
	int 	handle_special_keys ( int mKeyIndex );
	void	append_character	(char mChar);
		
	void				draw_keys();
	void				draw_alt_keys();	
	virtual int			draw();

	int					KeyHitTest		( int x, int y 	);
	int					AltKeyHitTest	( int x, int y 	);
	
	virtual Control*	HitTest	( int x, int y 	);
	virtual int 		onClick	( int x, int y, bool mouse_is_down );

	char m_composition[BUFFER_LENGTH];		// the typed text buffer.
	int  m_index;		
	vector<struct stKey>	m_keys;
	vector<struct stKey>	m_alt_keys;	
	bool					m_shift_down;
	bool					m_alt_down;

	long int 				key_color;
	float 					m_aspect_ratio;				// of the screen.
};


// float	compute_height_scale( float aspect_ratio );
	
#endif

