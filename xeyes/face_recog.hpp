//
//  face_recog.hpp
//  opencv_test
//
//  Created by Stephen Tenniswood on 7/29/16.
//  Copyright © 2016 Stephen Tenniswood. All rights reserved.
//

#ifndef face_recog_hpp
#define face_recog_hpp

#include <stdio.h>
#include <string>
#include <unistd.h>

#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/face.hpp"
#include "opencv2/core/utility.hpp"

extern cv::Ptr<cv::face::BasicFaceRecognizer> model;

extern std::string model_basepath        ;
extern std::string face_db_basepath      ;
extern std::string face_test_db_basepath ;
extern std::string FaceRecogModelName    ;

bool does_model_exist();
void open_model ( std::string model_filename );
void train_model( std::string mfn_csv );

void test_model ( std::string mfn_csv );


#endif /* face_recog_hpp */
