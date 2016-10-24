//
//  robot_interpolator.h
//  home3D
//
//  Created by Stephen Tenniswood on 7/14/15.
//  Copyright (c) 2015 Stephen Tenniswood. All rights reserved.
//

#ifndef __home3D__robot_interpolator__
#define __home3D__robot_interpolator__

#include <stdio.h>
#include "robot.hpp"
#include "robot_moves.h"


class glRobotInterpolator : public glStaticMovesRobot
{
public:
    glRobotInterpolator();
    ~glRobotInterpolator();
    
    void        save_one_vector   ( std::ofstream &mofs, int mIndex, bool mSaveArms=false );
    void        read_sequence_file();       // These can be played on the "asequencer" app.
    void        save_sequence_file( string mFilename );
    
    
    float      interpolate_one ( float mAngle1, float mAngle2,
                                 float mNumberOfSamples, float mIndex );
                                
    struct stBodyPositionVector*  interpolate( struct stBodyPositionVector* bp1, 
    									struct stBodyPositionVector* bp2,
                                        int number_samples, 
                                        int index_within_samples );

    void    interpolate_all( struct stBodyPositionVector* bp1,
                             struct stBodyPositionVector* bp2,
                             int number_samples );
    bool    play_next  ( );
    void    morph_demos( );
    
    vector<struct stBodyPositionVector>   m_sequence;
    int     m_interpolated_index;
};

#endif /* defined(__home3D__robot_interpolator__) */
