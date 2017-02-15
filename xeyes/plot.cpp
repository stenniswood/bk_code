#include <sys/time.h>
#include <sys/stat.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <signal.h>
#include <opencv2/highgui.hpp>
#include <opencv2/plot.hpp>


#include "plot.hpp"

using namespace std;
using namespace cv;

/* note plot is upside down...
	top of image is toward left side!
	
*/
void plot( cv::Mat& mDepth )
{
	
	int height = mDepth.rows;
	int centerW = mDepth.cols / 2;

	Scalar color (50,50,255);
	Point pt1(centerW+2,0); 
	Point pt2(centerW+2,height);
	line(mDepth,  pt1,  pt2, color);

	// Plot data must be a 1xN or Nx1 matrix.
    // Plot data type must be double (CV_64F) 
    Mat tmp( height, 1, CV_8UC3 );
    Mat dst( height, 1, CV_8UC1 );    
    Mat data( height, 1, CV_64F );
    randu( data, 0, 500 ); // random values

	mDepth.col(centerW).copyTo   ( tmp );
	cvtColor(tmp, dst, CV_RGB2GRAY);
	dst.convertTo(data, CV_64F);
	
    Mat plot_result;
    Ptr<plot::Plot2d> plot = plot::createPlot2d( data );
    plot->setPlotBackgroundColor( Scalar( 0, 50, 50 ) ); // i think it is not implemented yet
    plot->setPlotLineColor( Scalar( 0, 50, 255 ) );
    plot->setMaxY(255);
    plot->setMinY(0);
    plot->render( plot_result );

    imshow( "plot", plot_result );
 //   waitKey();

}