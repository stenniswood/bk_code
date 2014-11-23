#ifndef _RECT_H_
#define _RECT_H_


#define LEFT 	0
#define RIGHT 	1
#define TOP 	2
#define BOTTOM 	3


class Rect 
{
	Rect(int left, int right, int Width, int Height);
	Rect(float left, float right, float Width, float Height);
	Rect(int Width, int Height);

	float 	get_left	( )		{ return coords[LEFT];   };
	float 	get_right	( )		{ return coords[RIGHT];  };
	float 	get_top		( )		{ return coords[TOP];    };
	float 	get_bottom	( )		{ return coords[BOTTOM]; };

	float 	get_width	( )		{ return coords[RIGHT]-coords[LEFT];   };
	float 	get_height	( )		{ return coords[TOP] - coords[BOTTOM]; };

	void	move_to		( float left, float bottom );
	void 	set_size	( float width, float height);
		
private:
	float	coords[4];
	
};


#endif
