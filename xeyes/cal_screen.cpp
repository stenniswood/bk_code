
#include "VG/openvg.h"
#include "VG/vgu.h"
#include <math.h>
#include <shapes.h>
#include <fontinfo.h>
#include <string>
#include <vector>
#include <sys/types.h>
#include <dirent.h>

#include <opencv2/core/core.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;
using namespace std;

#include "cal_screen.hpp"
const int CAL_POINT_RADIUS = 20;
int screen_width  = 1280;
int screen_height = 720;


void draw_cal_point(int x, int y)
{
    
	Circle( x,y, CAL_POINT_RADIUS );
	//TextMid(width / 2, height / 2, "hello, world", SerifTypeface, width / 10);
}

void draw_corners( int SWidth, int SHeight )
{
	Fill(44, 77, 232, 1);				    // Big blue marble
	float x,y;
	x = 10;  	       y=10;
	draw_cal_point(x,y);
	x = SWidth-10;     y=10;
	draw_cal_point(x,y);

	x = SWidth-10;         y=SHeight-10;
	draw_cal_point(x,y);
	x = 10;                y=SHeight-10;
	draw_cal_point(x,y);
}

void draw_grid_points( int rows, int cols, int SWidth, int SHeight )
{
	float x_increment = SWidth  / (cols+1);
	float y_increment = SHeight / (rows+1);
	int   x,y;
	Fill(144, 77, 132, 1);				    // Big blue marble
	
	for (int c=0; c<cols; c++)
		for (int r=0; r<rows; r++)
		{	
			x = trunc(x_increment * (c+1));
			y = trunc(y_increment * (r+1));
			draw_cal_point(x,y);
		}	
}

int draw_cal_screen() 
{
    char s[3];
    init(&screen_width, &screen_height);                  // Graphics initialization

    Start(screen_width, screen_height);                   // Start the picture
    Background(0, 0, 0);                    // Black background

	draw_corners( screen_width, screen_height );
	draw_grid_points( 3, 5, screen_width, screen_height );
	
    //Circle(width / 2, 0, width);            // The "world"
	//Fill(255, 255, 255, 1);                 // White text
    //TextMid(width / 2, height / 2, "hello, world", SerifTypeface, width / 10);  // Greetings 
    End();                                  // End the picture    
}

void close_cal_screen()
{
	finish();
}


/*********************** VIEW FACES ****************************/
void populate_directory_files( string mPath, vector<string>& mFileList )
{
	bool   show_hidden_files=false;
	bool   is_hidden_file   =false;
	DIR    *d;
	struct dirent *dir;
	d 	   = opendir(mPath.c_str());
	if (d)
	{
		// Go thru all FILES:
		while ((dir = readdir(d)) != NULL)
		{
			if (dir->d_type==DT_REG)
			{
				is_hidden_file  = (dir->d_name[0]=='.') ? true:false ;
				if ((show_hidden_files==false) && (is_hidden_file))
				{
					continue;
				}
				
				mFileList.push_back( dir->d_name );
			}
		}
	}
	closedir(d);
}

int	 rows,cols;
cv::Mat DispImage;

const int trained_width  = 92;
const int trained_height = 112;

void view_directory_of_images( string mPath )
{
	cv::Mat  image,cimage;
	string   filename;
	float    x = 20;
	float    y = 20;
	int 	row,col;
	float    row_height;
	float    imgw = 100;
	float    imgh = 100;
	vector<string>  fileList;
	populate_directory_files( mPath, fileList );
	int xpixels = 0;
	int ypixels = 0;
	CvSize      size( 1200, 900 );
	DispImage.create( 900, 1280, CV_8UC3);
	//DispImage = cvCreateImage(size, 8, 3 );
	printf("DispImage:  wh = %d, %d \n", DispImage.cols, DispImage.rows ); 
	Size final_size(trained_width,trained_height);
			
	for (int i=0; i<fileList.size(); i++)
	{	
		filename = mPath + fileList[i];
		image = imread(filename);
		printf("%s : xy=%6.2f,%6.2f;  r,c=<%d,%d>\n", filename.c_str(), x,y, image.rows, image.cols );
		cv::resize( image, image, final_size );

		// Set the image ROI to display the current image
		cv::Rect roi( x, y, (int)(image.cols), (int)(image.rows) );
		Mat workImg = DispImage(roi);
		image.copyTo(workImg);
		// Resize the input image and copy the it to the Single Big Image
		//cvResize	   (image, DispImage);
		// Reset the ROI in order to display the next image
		//cvResetImageROI(DispImage);

		col += 1;
		x += image.cols;
		if (image.rows > row_height) 
			row_height = image.rows;		
		
		if ((x+image.cols) > screen_width)
		{
			x = 20;
			y += row_height;
			if ( (y+image.rows) > DispImage.rows )
				return;
		}
		if (col > cols)		cols = col;		

			
	}
	//CvSize      size( xpixels, ypixels );
	//DispImage = cvCreateImage(size, 8, 3 );
}

int show_person_faces( string person_folder_name ) 
{
	view_directory_of_images( person_folder_name );	
	imshow("Person Training Data", DispImage);	    
}

/* WARNING : Converts all images in the directory to the size.
			Not undoable!
*/
void make_training_size( string mPath )
{
	cv::Mat  image,cimage;
	string   filename;
	int 	 row,col;
	float    row_height;
	vector<string>  fileList;
	populate_directory_files( mPath, fileList );
	CvSize      size( 1200, 900 );
	Size final_size(trained_width,trained_height);
			
	for (int i=0; i<fileList.size(); i++)
	{	
		filename = mPath + fileList[i];
		image = imread(filename);
		cv::resize( image, image, final_size );
		cv::imwrite ( filename, image );
	}
}

/* blocks until done speaking */
void text_to_speech_pico( const char* mText )
{
	char cmd[512];
	sprintf( cmd, "/usr/bin/pico2wave -w /tmp/tmp.wav \"%s\" && aplay /tmp/tmp.wav", mText );
	system ( cmd );
}

/* blocks until done speaking */
void text_to_speech_festival( const char* mText )
{
	char cmd[512];
	sprintf( cmd, "echo \"%s\" | festival --tts ", mText );
	system ( cmd );
}
