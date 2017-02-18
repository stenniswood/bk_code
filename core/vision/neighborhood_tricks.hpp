
using namespace std;

typedef struct stPlane
{
	float Slope_ud;
	float Slope_lr;	
	float Intercept;
	
	float Slope_ud_Dev;
	float Slope_lr_Dev;
	float Intercept_Dev;
	int centerX;
	int centerY;
} ST_PLANE;


/* Each element of the vector is a group of similar planes.  */

//extern std::vector<std::list<ST_PLANE>> Combos;
extern cv::Mat	planeLabels;


// Create list of voxels:
void scan_image_neighborhoods( cv::Mat& mRawDepth );
// Combine the list of voxels to similar ones:
void combine_all_voxels( );
void annotate_combos(cv::Mat& mDepth);


void segment_planes( cv::Mat& mRawDepth, cv::Mat& mPlaneLabels );