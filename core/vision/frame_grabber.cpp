#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <assert.h>
#include "libfreenect.h"
#include "bk_system_defs.h"
#include "frame_grabber.hpp"
#include "face_detect.hpp"


//#define USE_KINECT_SENSOR 1
  
cv::VideoCapture 	capture(-1);
cv::Mat 			frame, gray_frame, prev_frame, prev_gray_frame, flow, original, raw_depth, cv_depth;

extern cv::Mat				planeLabels;	// defined in neighborhood_tricks.cpp


volatile int die = 0;
uint8_t *depth_mid, *depth_front;
uint8_t *rgb_back, *rgb_mid, *rgb_front;

unsigned int gl_depth_tex;
unsigned int gl_rgb_tex;
float 	camera_angle = 0.0;
int 	camera_rotate = 0;
int 	tilt_changed = 0;

freenect_context *f_ctx;
freenect_device  *f_dev;
int freenect_angle = 0;
int freenect_led;

freenect_video_format requested_format = FREENECT_VIDEO_RGB;
freenect_video_format current_format = FREENECT_VIDEO_RGB;

pthread_cond_t 	gl_frame_cond = PTHREAD_COND_INITIALIZER;
int got_rgb   = 0;
int got_depth = 0;

uint16_t t_gamma[2048];
pthread_mutex_t gl_backbuf_mutex = PTHREAD_MUTEX_INITIALIZER;

void depth_cb(freenect_device *dev, void *v_depth, uint32_t timestamp)
{
	int i;
	uint16_t *depth = (uint16_t*)v_depth;

	uint16_t *rdepth = (uint16_t*)raw_depth.data;
					
	pthread_mutex_lock(&gl_backbuf_mutex);
	for (i=0; i<640*480; i++) {
		int pval = t_gamma[depth[i]];
		int lb = pval & 0xff;

		rdepth[i] = depth[i];
		lb = (depth[i] >> 3);
		depth_mid[3*i+0] = 255-lb;
		depth_mid[3*i+1] = 255-lb;
		depth_mid[3*i+2] = 255-lb;

/*		switch (pval>>8) {
			case 0:
				depth_mid[3*i+0] = 255;
				depth_mid[3*i+1] = 255-lb;
				depth_mid[3*i+2] = 255-lb;
				break;
			case 1:
				depth_mid[3*i+0] = 255;
				depth_mid[3*i+1] = lb;
				depth_mid[3*i+2] = 0;
				break;
			case 2:
				depth_mid[3*i+0] = 255-lb;
				depth_mid[3*i+1] = 255;
				depth_mid[3*i+2] = 0;
				break;
			case 3:
				depth_mid[3*i+0] = 0;
				depth_mid[3*i+1] = 255;
				depth_mid[3*i+2] = lb;
				break;
			case 4:
				depth_mid[3*i+0] = 0;
				depth_mid[3*i+1] = 255-lb;
				depth_mid[3*i+2] = 255;
				break;
			case 5:
				depth_mid[3*i+0] = 0;
				depth_mid[3*i+1] = 0;
				depth_mid[3*i+2] = 255-lb;
				break;
			default:
				depth_mid[3*i+0] = 0;
				depth_mid[3*i+1] = 0;
				depth_mid[3*i+2] = 0;
				break;
		}*/
	}
	got_depth++;

	// COPY DATA INTO OPENCV IMAGE:
//	memcpy( cv_depth.data, depth, 640*480*2 );
	memcpy( cv_depth.data, depth_mid, 640*480*3 );	
	//cv::cvtColor( cv_depth, cv_depth, cv::COLOR_BGR2RGB);
	
	pthread_cond_signal(&gl_frame_cond);
	pthread_mutex_unlock(&gl_backbuf_mutex);
}

void rgb_cb(freenect_device *dev, void *rgb, uint32_t timestamp)
{
	pthread_mutex_lock(&gl_backbuf_mutex);

	// swap buffers
	assert (rgb_back == rgb);
	rgb_back = rgb_mid;
	freenect_set_video_buffer(dev, rgb_back);
	rgb_mid = (uint8_t*)rgb;

	got_rgb++;
	
	// COPY DATA INTO OPENCV IMAGE:
	//memcpy( cv_depth.data, rgb, 640*480*3 );	
	memcpy( frame.data, rgb, 640*480*3 );	
	cv::cvtColor( frame, frame, cv::COLOR_BGR2RGB);

	pthread_cond_signal(&gl_frame_cond);
	pthread_mutex_unlock(&gl_backbuf_mutex);
}


void freenect_init(int mDeviceNumber )
{
	depth_mid 	= (uint8_t*)malloc(640*480*3);
	depth_front = (uint8_t*)malloc(640*480*3);
	rgb_back 	= (uint8_t*)malloc(640*480*3);
	rgb_mid 	= (uint8_t*)malloc(640*480*3);
	rgb_front 	= (uint8_t*)malloc(640*480*3);
	
	int i;
	for (i=0; i<2048; i++) 
	{
		float v = i/2048.0;
		v = powf(v, 3)* 6;
		t_gamma[i] = v*6*256;
	}

	if (freenect_init(&f_ctx, NULL) < 0) {
		printf("freenect_init() failed\n");
		return ;
	}
	freenect_set_log_level    (f_ctx, FREENECT_LOG_DEBUG);
	freenect_select_subdevices(f_ctx, (freenect_device_flags)(FREENECT_DEVICE_MOTOR | FREENECT_DEVICE_CAMERA));
	int nr_devices = freenect_num_devices(f_ctx);
	printf ("Number of devices found: %d\n", nr_devices);

	int user_device_number = 0;
	user_device_number = mDeviceNumber;

	if (nr_devices < 1) {
		freenect_shutdown(f_ctx);
		return ;
	}
	printf("open device:\n");
	if (freenect_open_device(f_ctx, &f_dev, user_device_number) < 0) {
		printf("Could not open device\n");
		freenect_shutdown(f_ctx);
		return ;
	}

	freenect_set_tilt_degs		(f_dev, freenect_angle);
	freenect_set_led			(f_dev, LED_RED );
	freenect_set_depth_callback (f_dev, depth_cb);
	freenect_set_video_callback (f_dev, rgb_cb  );
	freenect_set_video_mode		(f_dev, freenect_find_video_mode(FREENECT_RESOLUTION_MEDIUM, current_format));
	freenect_set_depth_mode		(f_dev, freenect_find_depth_mode(FREENECT_RESOLUTION_MEDIUM, FREENECT_DEPTH_11BIT));
	freenect_set_video_buffer	(f_dev, rgb_back);

	printf("\nStarting depth & video: \n");
	freenect_start_depth(f_dev);
	freenect_start_video(f_dev);
	printf("freenect_init() done \n");

	
/*	int res = pthread_create(&freenect_thread, NULL, freenect_threadfunc, NULL);
	if (res) {
		printf("pthread_create failed\n");
		freenect_shutdown(f_ctx);
		return 1;
	} */
}

void frame_grab_init(int mDeviceNumber )
{
	if (USE_KINECT_SENSOR) {
		frame.create       ( 480, 640, CV_8UC3 );
		raw_depth.create   ( 480, 640, CV_16UC1);
		cv_depth.create    ( 480, 640, CV_8UC3 );
		planeLabels.create ( 480, 640, CV_8UC3 );

		freenect_init(mDeviceNumber);
				
		cv::namedWindow("depth",CV_WINDOW_NORMAL);
		cv::moveWindow ("depth", 750, 100);		
	}
	else {
		// webcam init		
		if( capture.isOpened()==false) 
		{
			printf("Warning: Camera is not on!\n");
			exit(1);
		}
	}
}


void frame_grabs()
{
	// Get 1 frame : 
	prev_gray_frame = gray_frame;	// save last frame (for optic flow)
	prev_frame 		= frame;		// save last frame (for optic flow)
	if (USE_KINECT_SENSOR) 
	{
		freenect_threadfunc(NULL);
		cv::imshow( "depth", cv_depth );
		//printf("\n\nshowing DEPTH\n\n");		
	}
	else  capture.read(frame);

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
	printf("\nshutting down streams...\n");
	if (USE_KINECT_SENSOR) {
		freenect_stop_depth  (f_dev);
		freenect_stop_video  (f_dev);	
		freenect_close_device(f_dev);
		freenect_shutdown    (f_ctx);

		free(depth_mid);
		free(depth_front);
		free(rgb_back);
		free(rgb_mid);
		free(rgb_front);		
	}
}

void *freenect_threadfunc(void *arg)
{
	int accelCount = 0;

	//while (!die && freenect_process_events(f_ctx) >= 0)
	if  (!die && freenect_process_events(f_ctx) >= 0)
	{
		//Throttle the text output
		if (accelCount++ >= 2000)
		{
			accelCount = 0;
			freenect_raw_tilt_state* state;
			freenect_update_tilt_state(f_dev);
			state = freenect_get_tilt_state(f_dev);
			double dx,dy,dz;
			freenect_get_mks_accel(state, &dx, &dy, &dz);
			printf("\r raw acceleration: %4d %4d %4d  mks acceleration: %4f %4f %4f", state->accelerometer_x, state->accelerometer_y, state->accelerometer_z, dx, dy, dz);
			fflush(stdout);
		}

		if (requested_format != current_format) {
			freenect_stop_video    (f_dev);
			freenect_set_video_mode(f_dev, freenect_find_video_mode(FREENECT_RESOLUTION_MEDIUM, requested_format));
			freenect_start_video   (f_dev);
			current_format = requested_format;
		}
	}
	printf("-- done!\n");
	return NULL;
}

/*	printf("'w' - tilt up, 's' - level, 'x' - tilt down, '0'-'6' - select LED mode, '+' & '-' - change IR intensity \n");
	printf("'f' - change video format, 'm' - mirror video, 'o' - rotate video with accelerometer \n");
	printf("'e' - auto exposure, 'b' - white balance, 'r' - raw color, 'n' - near mode (K4W only) \n");
*/
