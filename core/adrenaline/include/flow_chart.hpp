#ifndef _FLOW_CHART_
#define _FLOW_CHART_

#include "control.hpp"

/* Could draw this with 3D renderer to get shadows,etc. */
const int FLOW_CIRCLE  =1;
const int FLOW_SQUARE  =1;
const int FLOW_DIAMOND =1;
const int FLOW_PARALLEL=1;
const int FLOW_ELIPSE  =1;

struct stFlowObject {
	float x;
	float y;
	float width;
	float height;
	char* text;
	int   shape;	// See above definitions
};

struct stPoint2D {
	float x;
	float y;	
};

class FlowChart : public Control 
{
public:
	FlowChart( );
	FlowChart( int Left, int Right, int Top, int Bottom );
	FlowChart( int Width, int Height 					);
	~FlowChart();
	
	virtual void 	Initialize(	);

	void   			add_object			( stFlowObject* mObject );
	int   			add_square			( float x, float y, char* text 	);
	int   			add_circle			( float x, float y, char* text	);
	int   			add_diamond			( float x, float y, char* text	);
	int   			add_parallelagram	( float x, float y, char* text	);
	int   			add_elispse 		( float x, float y, char* text	);

	int   			add_connector 		( stPoint2D mStart, stPoint2D mEnd	);
	int   			remove_connector 	( 	);
		
	virtual int   	draw 				(	);
	int   			draw_object 		(	);
	int   			draw_connector		(	);

	int				set_on_click_listener( void (*callback)(void*), void* mOn_click_context );
	virtual int		onClick(int x, int y, bool mouse_is_down=true);
	
private:
	vector<stFlowObject>	m_object_info;
	vector<VGPath>			m_objects;
	vector<VGPath>			m_connectors;
	
	void (*on_click_callback)(void*);
	void* on_click_context;				// often this pointer to a Control*	
};

 
#endif
