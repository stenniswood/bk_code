//
//  face_recog.cpp
//  opencv_test
//
//  Created by Stephen Tenniswood on 7/29/16.
//  Copyright © 2016 Stephen Tenniswood. All rights reserved.
//
#include <string>
#include "face_recog.hpp"
#include "face_detect.hpp"
#include "cal_screen.hpp"

/*
 * Copyright (c) 2011. Philipp Wagner <bytefish[at]gmx[dot]de>.
 * Released to public domain under terms of the BSD Simplified license.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of the organization nor the names of its contributors
 *     may be used to endorse or promote products derived from this software
 *     without specific prior written permission.
 *
 *   See <http://www.opensource.org/licenses/bsd-license>
 */
#include <unistd.h>
#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/face.hpp"
#include "opencv2/core/utility.hpp"

#include <iostream>
#include <fstream>
#include <sstream>
#include <map>

using namespace cv;
using namespace cv::face;
using namespace std;

Ptr<BasicFaceRecognizer> model;

string model_basepath        = "/home/pi/Pictures/face_recog/";
string face_db_basepath      = "/home/pi/Pictures/face_recog/";
string face_test_db_basepath = "/home/pi/Pictures/face_recog/att_test/";
string FaceRecogModelName    = model_basepath + "face-rec-model.txt";

std::map<int, std::string> known_people;
std::string  message = "no event";

/* This loads the csv file and each image specified in it.
 Each line can specify a filename, or a directory name.
 If a directory, each image in there will be assigned to the label number on that line.
 So you could have
 Example CSV:
 ./people/steve/; 1;
 ./people/mark/;2;
 ./people/dave/;3;
 ./people/lynn/;4;
 ./people/martha/;5;
 */
static void read_csv(const string& basepath, const string& filename,
                     vector<Mat>& images, vector<int>& labels, 
                     std::map<int, string>& labelsInfo,
                     char separator = ';')
{
    //char * dir = getcwd(NULL, 0); // Platform-dependent, see reference link below
    //printf("Current dir: %s\n\n", dir);
    
    string fullfilename = basepath + filename;
    ifstream csv(fullfilename.c_str());
    if (!csv)
        CV_Error(Error::StsBadArg, "No valid input file was given, please check the given filename.");
    
    string line, path, classlabel, info;
    while (getline(csv, line))
    {
        stringstream liness(line);
        path.clear(); classlabel.clear(); info.clear();
        getline(liness, path,       separator);
        getline(liness, classlabel, separator);
        getline(liness, info,       separator);
        if(!path.empty() && !classlabel.empty())
        {
            cout << "Processing " << path << endl;
            int label = atoi(classlabel.c_str());
            if(!info.empty())
                labelsInfo.insert(std::make_pair(label, info));
            
            // 'path' can be file, dir or wildcard path
            String          root (basepath + path.c_str());
            vector<String>  files;
            glob(root, files, true);
            for (vector<String>::const_iterator f = files.begin(); f != files.end(); ++f)
            {
                cout << "\t" << *f << endl;
                Mat img = imread(*f, IMREAD_GRAYSCALE);
                static int w=-1, h=-1;
                static bool showSmallSizeWarning = true;
                if(w>0 && h>0 && (w!=img.cols || h!=img.rows))
                    cout << "\t* Warning: images should be of the same size! r,c=<" << img.cols<<", " << img.rows << ">" << endl;
                if(showSmallSizeWarning && (img.cols<50 || img.rows<50)) {
                    cout << "* Warning: for better results images should be not smaller than 50x50!" << endl;
                    showSmallSizeWarning = false;
                }
                images.push_back(img  );
                labels.push_back(label);
            }
        }
    }
}

static void load_csv_lookups(
					 const string& basepath, const string& filename,                     
                     std::map<int, string>& labelsInfo,
                     char separator = ';')
{    
    string fullfilename = basepath + filename;
    cout << "load_csv_lookups() " << fullfilename << " " << endl;
                
    ifstream csv(fullfilename.c_str());
    if (!csv)
        CV_Error(Error::StsBadArg, "No valid input file was given, please check the given filename.");
    
    //labelsInfo.clear();
    string line, path, classlabel, info;
            cout << "Starting read" << endl;
    while (getline(csv, line))
    {
        stringstream liness(line);
        cout << line << endl;
        path.clear(); classlabel.clear(); info.clear();
        getline(liness, path,       separator);
        getline(liness, classlabel, separator);
        getline(liness, info,       separator);
        if(!path.empty() && !classlabel.empty() && !info.empty())
        {
            cout << "Processing " << path << " " << classlabel << " " << info << " " << endl;
            int label = atoi(classlabel.c_str());
			cout << label << endl;
      //      labelsInfo[label] = info;
			known_people.insert ( std::pair<int,string>(label,info) );            
            //labelsInfo.insert( std::make_pair(label, info) );
        }
    }
}

bool does_model_exist()
{
    ifstream model_file( FaceRecogModelName.c_str() );
    if (!model_file)
        return false;
    model_file.close();
    return true;
}

void train_model(string mfn_csv)
{
    // These vectors hold the images and corresponding labels.
    vector<Mat>           images;
    vector<int>           labels;
    //std::map<int, string> labelsInfo;

    // Read in the data. This can fail if no valid
    // input filename is given.
    try {
        read_csv( face_db_basepath, mfn_csv, images, labels, known_people );
    } catch (cv::Exception& e) {
        cerr << "Error opening file \"" << mfn_csv << "\". Reason: " << e.msg << endl;
        exit(1);
    }
    
    // Quit if there are not enough images for this demo:
    if(images.size() <= 1) {
        string error_message = "This demo needs at least 2 images to work. Please add more images to your data set!";
        CV_Error( Error::StsError, error_message );
    }
    
    int nlabels    = (int)labels.size();
    
    // The following lines create an Eigenfaces model for
    // face recognition and train it with the images and
    // labels read from the given CSV file.
    // This here is a full PCA, if you just want to keep
    // 10 principal components (read Eigenfaces), then call
    // the factory method like this:
    //
    //      cv::createEigenFaceRecognizer(10);
    //
    // If you want to create a FaceRecognizer with a
    // confidennce threshold, call it with:
    //
    //      cv::createEigenFaceRecognizer(10, 123.0);
    //
    model = createEigenFaceRecognizer();
    for( int i = 0; i < nlabels; i++ )
        model->setLabelInfo(i, known_people[i]);
    model->train( images, labels );
    
    cout << "Saving the trained model to " << FaceRecogModelName << endl;
    model->save (FaceRecogModelName);
}

void open_model( string  model_filename, string mcsv_fn )
{
	load_csv_lookups( face_db_basepath, mcsv_fn, known_people );
	
    cout << "Loading the trained model from " << model_filename << endl;
    model = createEigenFaceRecognizer();
    model->load( model_filename );
    
    string testSample = "/home/pi/Pictures/face_recog/s10/5.pgm";
    cv::Mat test = imread(testSample, 0 );
    printf("Test Image: %d,%d, chn=%d \n", test.cols, test.rows, test.channels() );    
    int predictedLabel = model->predict( test );
    printf("Test Image: %d\n", predictedLabel);
}

Mat testSample;

void test_model( string mfn_csv )
{
    vector<Mat>           timages;
    vector<int>           labels;
    std::map<int, string> labelsInfo;
    
    // Read in the data.
    // This can fail if no valid input filename is given.
    try {
        read_csv( face_test_db_basepath, mfn_csv, timages, labels, known_people );
    } catch (cv::Exception& e) {
        cerr << "Error opening file \"" << mfn_csv << "\". Reason: " << e.msg << endl;
        exit(1);
    }
    
    // The following line predicts the label of a given
    // test image:
    int predictedLabel;
    for ( int i=0; i<timages.size(); i++ )
    {
        predictedLabel = model->predict( timages[i] );
        // To get the confidence of a prediction call the model with:
        //
        //      int predictedLabel = -1;
        //      double confidence = 0.0;
        //      model->predict(testSample, predictedLabel, confidence);
        //
        string result_message = format("Predicted class = %d / Actual class = %d.", predictedLabel, labels[i] );
        cout << result_message << endl;
        bool match = (predictedLabel == labels[i]);
        if( match && !model->getLabelInfo(predictedLabel).empty() ) // Print the comment if available.
            cout << format("%d-th label's info: %s", predictedLabel, model->getLabelInfo(predictedLabel).c_str()) << endl;
    }
    testSample = timages[timages.size()-1];
}

const int trained_width  = 92;
const int trained_height = 112;

/* Prepare a detected face for recognition
 */
cv::Mat& prep_detected_face( cv::Mat mIn )
{
	static cv::Mat mOut;
	// scale to required size:
	Size final_size(trained_width,trained_height);
	cv::resize( mIn, mOut, final_size );
	return mOut;
}

/*  
    Goes thru each detected face, and predicts who it might be.
	Return is a vector of face labels 
*/
std::string& face_recongition_tasks( cv::Mat&               mGray_frame, 
									 std::vector<cv::Rect>& faces, 
									 std::vector<int>&      predictedLabels, 
									 bool& 					mcapture_frame   )
{
	cv::Mat  face, prepared_face;
	
	int 	label;
	double 	confidence;

	for (int i=0; i<faces.size(); i++)
	{
		face 		   = mGray_frame       ( faces[i] );	 // Get Face color Sub image
		prepared_face  = prep_detected_face( face     );
		model->predict    ( prepared_face, label, confidence );
		printf("DONE Predicting...%d  %6.2f\n", label, confidence );
		predictedLabels.push_back( label );
	}

	if (predictedLabels.size())
	{
		// Welcome with voice greeting : 
		for (int i=0; i<predictedLabels.size(); i++) {
			message = "hello ";
			message += known_people[predictedLabels[i]];
			printf("%s\n", message.c_str());
			text_to_speech_pico( message.c_str() );			
			//message += known_people[ predictedLabels[0] ];
		}
		// or if any faces which left the scene - log in database :
		//eyes_write_server_event( message );

		// Inform Client of Presence :
		//message = "face detected";
	} else {
		// Unrecognized face:
	}
	return message;
}

void advanced_stuff()
{
    // Sometimes you'll need to get/set internal model data,
    // which isn't exposed by the public cv::FaceRecognizer.
    // Since each cv::FaceRecognizer is derived from a
    // cv::Algorithm, you can query the data.
    //
    // First we'll use it to set the threshold of the FaceRecognizer
    // to 0.0 without retraining the model. This can be useful if
    // you are evaluating the model:
    //
    model->setThreshold(0.0);
    // Now the threshold of this model is set to 0.0. A prediction
    // now returns -1, as it's impossible to have a distance below
    // it
    int predictedLabel = model->predict(testSample);
    cout << "Predicted class = " << predictedLabel << endl;
    // Here is how to get the eigenvalues of this Eigenfaces model:
    Mat eigenvalues = model->getEigenValues();
    // And we can do the same to display the Eigenvectors (read Eigenfaces):
    Mat W = model->getEigenVectors();
    // From this we will display the (at most) first 10 Eigenfaces:
    for (int i = 0; i < min(10, W.cols); i++) {
        string msg = format("Eigenvalue #%d = %.5f", i, eigenvalues.at<double>(i));
        cout << msg << endl;
        // get eigenvector #i
        Mat ev = W.col(i).clone();
        // Reshape to original size & normalize to [0...255] for imshow.
        Mat grayscale;
        normalize(ev.reshape(1), grayscale, 0, 255, NORM_MINMAX, CV_8UC1);
        // Show the image & apply a Jet colormap for better sensing.
        Mat cgrayscale;
        applyColorMap(grayscale, cgrayscale, COLORMAP_JET);
        imshow(format("%d", i), cgrayscale);
    }
    waitKey(0);
}

/*if (argc != 2 && argc != 3) {
 cout << "Usage: " << argv[0] << " <csv> [arg2]\n"
 << "\t<csv> - path to config file in CSV format\n"
 << "\targ2 - if the 2nd argument is provided (with any value) "
 << "the advanced stuff is run and shown to console.\n"
 << "The CSV config file consists of the following lines:\n"
 << "<path>;<label>[;<comment>]\n"
 << "\t<path> - file, dir or wildcard path\n"
 << "\t<label> - non-negative integer person label\n"
 << "\t<comment> - optional comment string (e.g. person name)"
 << endl;
 exit(1);
 }*/
