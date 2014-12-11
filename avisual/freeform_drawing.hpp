#ifndef _FREEFORM_DRAWING_
#define _FREEFORM_DRAWING_

#include <vector>
#include <list>
#include "control.hpp"


struct stCoords {
	int	  number_of_coords;
	VGfloat* ptr;
};

using namespace std;


class FreeFormDrawing : public Control
{
public:
	FreeFormDrawing();
	FreeFormDrawing( int Left, int Right, int Top, int Bottom );
	FreeFormDrawing( int Width, int Height 					 );
	~FreeFormDrawing();
	
	virtual void 	Initialize(	);

	virtual int   	draw 		(	);	
	virtual int		onClick(int x, int y, bool mouse_is_down=true);
	
private:
	list<vector<VGfloat>>	m_stroke_widths;
	list<vector<long>>	m_stroke_color;	
	list<vector<VGubyte>>	m_commands;
	list<vector<VGfloat>>	m_coords;

	vector<VGfloat>			m_current_coords;	// the latest curve being clicked in	
};

 
#endif
