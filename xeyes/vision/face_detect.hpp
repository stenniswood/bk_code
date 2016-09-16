#ifndef _FACE_DETECT_HPP_
#define _FACE_DETECT_HPP_


#define SHOW_EYES 0


//int 	fd_main(int argc, char* argv[]);

extern std::string 		  main_window_name;
extern std::string 		  face_window_name;

extern  cv::Mat 	frame;
extern  cv::Mat 	faceROIc;

extern int 	num_faces_present;
extern std::vector<cv::Rect> faces;

struct timeval GetTimeStamp ();
uint64_t 	   GetTimeStamp2();

void 	findEyes(cv::Mat frame_gray, cv::Rect face);
cv::Mat findSkin(cv::Mat &frame);

int 	fd_init		();
void 	fd_timeslice( cv::Mat& frame_gray, cv::Mat& original, bool mFindEyes=true );
void 	fd_close	();
void	single_channel_gray( cv::Mat& frame, cv::Mat& output_gray );
void	annotate_faces	   ( cv::Mat& mOriginal );

char*	get_new_frame_captured_filename();
void	save_faces( cv::Mat& mOriginal, std::vector<int>& predictedLabels );

#endif
