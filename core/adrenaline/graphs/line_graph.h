#ifdef  __cplusplus
extern "C" {
#endif
 
struct info 
{
	int 	left;
	int 	right;
	int 	top;
	int 	bottom;	
	float 	xscale, xoffset;
	float 	yscale, yoffset;
	char* 	xAxisLabel;
	char* 	yAxisLabel;
	char* 	title;
};

void init_1graph();

int draw_graph		(										) ;
float calc_scale	(struct info* mInfo, float* data, int n );
float get_min		( float* data, int n );
float get_max		( float* data, int n );
void line_plot		( struct info* mInfo, float* mYdata, int n );
void scatter_plot	(float* mXdata, float* mYdata );
int draw_title		(struct info* mInfo) ;
int draw_x_axis_label	(struct info* mInfo) ;
int draw_y_axis_label	(struct info* mInfo) ;
void draw_vertical_lines(struct info* mInfo ) ;
void draw_horizontal_lines( struct info* mInfo );
 
#ifdef  __cplusplus
}
#endif
