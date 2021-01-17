#ifndef _GRAPH_HPP_
#define _GRAPH_HPP_


#include <vector>


struct stDataPoint {
	gdouble x;
	gfloat  y;
};

#define NUM_DATA_POINTS 640
extern struct stDataPoint data[NUM_DATA_POINTS];

struct stColor {
	gfloat red;
	gfloat green;
	gfloat blue;
	gfloat alpha;
};

extern const struct stColor WHITE ;
extern const struct stColor LIGHT_BLUE   ;	// 0x80 B9 e4
extern const struct stColor LIGHT_GREEN  ; // 6f aa 6f
extern const struct stColor YELLOW 	  ; // ec e7 36
extern const struct stColor ORANGE 	  ;  // d8 9a 25
extern const struct stColor RED 		  ;  // c6 1c 1e
extern const struct stColor RED_PURPLE   ;  // 78 2d 85
extern const struct stColor BLUE_PURPLE  ;  // 35 31 8a  
extern const struct stColor BLUE 		  ;  // 29 49 9b
extern const struct stColor BLUE_GREEN   ;  // 3a 75 76
extern const struct stColor GREEN 		  ;  // 3a 72 2e
extern const struct stColor YELLOW_GREEN ;  // 91 b2 2c 
extern const struct stColor YELLOW_BROWN ;  // 53 46 1a
extern const struct stColor RED_BROWN    ;  // 56 23 22
extern struct stColor pallette[14];


struct stSeriesInfo 
{
	struct stDataPoint* data;
	int     data_length;
	stColor color;
	char name[80];
};
extern std::vector<struct stSeriesInfo> series_data;

struct stGraphInfo
{
	float x_scale;
	float y_scale;
	char  title[128];
	char  xaxis_label[128];
	char  yaxis_label[128];	
	stColor backgroundColor;
	bool  show_legend;
	bool  show_grid;
	float x_axis_margin;	// y space from edge to start of graph
	float y_axis_margin;	// x space from edge to start of graph
};

extern struct stGraphInfo graph_info;
void create_graph( const char* mTitle, const char* mxAxis, const char* myAxis, float mWindowWidth, float mWindowHeight );


gfloat get_min_x(struct stDataPoint* mDP, int mLength );
gfloat get_max_x(struct stDataPoint* mDP, int mLength );

gfloat get_max_y  (struct stDataPoint* mDP, int mLength );
gfloat get_y_scale(struct stDataPoint* mDP, int mLength );
gfloat compute_yscale_all_series(  );	
gfloat compute_xscale_all_series(  );

void draw_graph      ( cairo_t *cr, int window_width );
void draw_all_series ( cairo_t *cr);
void draw_legend     ( cairo_t *cr, int window_width );
void draw_title      ( cairo_t *cr, int screen_width );
void draw_axis_labels();

#endif
