#ifndef _RECT_H_
#define _RECT_H_


#define LEFT 	0
#define RIGHT 	1
#define TOP 	2
#define BOTTOM 	3

/* 
 This did not work when named as Rect.  
 Kept giving "error: ‘Rect’ does not name a type" even when it was included.
 So put it back as Rectangle!  (one of those weird things)
 For an obvious reason: shapes.h has a drawing routine called Rect !!

*/

class Rectangle 
{
public:
	Rectangle( int   left,  int   right, int   Width, int Height  );
	Rectangle( float left,  float right, float Width, float Height);
	Rectangle( float   Width, float   Height );
	Rectangle( );

	float 	set_left	( float mLeft   )	{ coords[LEFT] =mLeft;    left = mLeft;    		};
	float 	set_right	( float mRight  )	{ coords[RIGHT]=mRight;   width= mRight-left;   };
	float 	set_top		( float mTop    )	{ coords[TOP]  =mTop;     height= mTop-bottom;	};
	float 	set_bottom	( float mBottom )	{ coords[BOTTOM]=mBottom; bottom=mBottom; 		};
	
	float 	set_width	( float mWidth  )	{ coords[RIGHT]=coords[LEFT]+mWidth;    width = mWidth;  };
	float 	set_height	( float mHeight )	{ coords[TOP]  =coords[BOTTOM]+mHeight; height= mHeight; };

	inline float 	get_left	( )		{ return coords[LEFT];   };
	inline float 	get_right	( )		{ return coords[RIGHT];  };
	inline float 	get_top		( )		{ return coords[TOP];    };
	inline float 	get_bottom	( )		{ return coords[BOTTOM]; };

	inline float 	get_width	( )		{ return coords[RIGHT]-coords[LEFT];   };
	inline float 	get_height	( )		{ return coords[TOP] - coords[BOTTOM]; };

	void	draw		();
	int		hit_test	(float x, float y);	
	void	move_to		( float left, float bottom );
	void 	set_size	( float width, float height);

private:
	float	coords[4];
	float 	left;
	float 	bottom;
	float 	width;
	float 	height;
};


#endif
