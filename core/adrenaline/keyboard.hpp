#ifndef _KEYBOARD_
#define _KEYBOARD_


#include <vector>

struct stKey
{
	int x;
	int y;
	int width;
	int height;
	char text[8];		// A, B, C, Return, etc
};

class Keyboard : public Control
{
public:
	Keyboard();
	~Keyboard();
	
	void	initialize_keys();
	void	Initialize();
	virtual int			draw();
	virtual Control*	HitTest	( int x, int y 	);
	virtual int 		onClick	( int x, int y, bool mouse_is_down );
	
	vector<struct stKey>	m_keys;
	
};


#endif

