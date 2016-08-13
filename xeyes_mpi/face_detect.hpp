#ifndef _FACE_DETECT_HPP_
#define _FACE_DETECT_HPP_

//int 	fd_main(int argc, char* argv[]);

extern  cv::Mat 	frame;
extern  cv::Mat 	faceROIc;

extern int num_faces_present;
extern std::vector<cv::Rect> faces;

int 	fd_init		();
void 	fd_timeslice	();
void 	fd_close	();

#endif
