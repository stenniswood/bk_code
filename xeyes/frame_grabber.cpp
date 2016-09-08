#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>

#include <assert.h>
#include "libfreenect.h"
#include "bk_system_defs.h"
#include "frame_grabber.hpp"
#include "face_detect.hpp"


cv::VideoCapture 	  capture(-1);
cv::Mat frame, gray_frame, prev_frame, prev_gray_frame, flow, original;

uint8_t *depth_mid, *depth_front;
uint8_t *rgb_back, *rgb_mid, *rgb_front;

unsigned int gl_depth_tex;
unsigned int gl_rgb_tex;
float camera_angle = 0.0;
int camera_rotate = 0;
int tilt_changed = 0;

freenect_context *f_ctx;
freenect_device *f_dev;
int freenect_angle = 0;
int freenect_led;

freenect_video_format requested_format = FREENECT_VIDEO_RGB;
freenect_video_format current_format = FREENECT_VIDEO_RGB;

pthread_cond_t gl_frame_cond = PTHREAD_COND_INITIALIZER;
int got_rgb   = 0;
int got_depth = 0;


void frame_grab_init(int mDeviceNumber )
{
	depth_mid 	= (uint8_t*)malloc(640*480*3);
	depth_front = (uint8_t*)malloc(640*480*3);
	rgb_back 	= (uint8_t*)malloc(640*480*3);
	rgb_mid 	= (uint8_t*)malloc(640*480*3);
	rgb_front 	= (uint8_t*)malloc(640*480*3);
	
	if (freenect_init(&f_ctx, NULL) < 0) {
		printf("freenect_init() failed\n");
		return ;
	}
	freenect_set_log_level    (f_ctx, FREENECT_LOG_DEBUG);
	freenect_select_subdevices(f_ctx, (freenect_device_flags)(FREENECT_DEVICE_MOTOR | FREENECT_DEVICE_CAMERA));
	int nr_devices = freenect_num_devices (f_ctx);
	printf ("Number of devices found: %d\n", nr_devices);

	int user_device_number = 0;
	user_device_number = mDeviceNumber;

	if (nr_devices < 1) {
		freenect_shutdown(f_ctx);
		return ;
	}

	if (freenect_open_device(f_ctx, &f_dev, user_device_number) < 0) {
		printf("Could not open device\n");
		freenect_shutdown(f_ctx);
		return ;
	}	
}

void frame_grabs()
{
	// Get 1 frame : 
	prev_gray_frame = gray_frame;	// save last frame (for optic flow)
	prev_frame 		= frame;		// save last frame (for optic flow)
	capture.read(frame);
	if( frame.empty() )
	{
		printf(" --(!) No captured frame -- Break!");
		return;
	}
	frame.copyTo( original );
	single_channel_gray( frame, gray_frame );
}

void frame_grab_close()
{

}
