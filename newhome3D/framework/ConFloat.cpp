//
//  ConFloat.cpp
//  home3D
//
//  Created by Stephen Tenniswood on 4/8/16.
//  Copyright © 2016 Stephen Tenniswood. All rights reserved.
//

#include "ConFloat.hpp"




CtrlFloat::CtrlFloat()
{
    
}
CtrlFloat::CtrlFloat(float mInitValue)
{
    
}
CtrlFloat::~CtrlFloat()
{
    
}

/*****************************************/
Range::Range( float mMin, float mMax)
{
    
}

bool Range::inRange( float mValue )
{
    if ((mValue>Min) && (mValue<Max))
        return true;
    else
        return false;
}

/*****************************************/
LearnedRange::LearnedRange( float mAvg, float mStd )
{
    
}
void LearnedRange::learn_value( float mValue )       // add a new value which is to be considered in range.
{
    Sum += mValue;
    Count++;
    Avg = (Sum / Count);
    
    StdDev = 1.0;
}

bool LearnedRange::inRange    ( float mValue )
{
    float six_sigma = StdDev * 6.0;
    float upper = Avg + six_sigma;
    float lower = Avg - six_sigma;
    
    if ((mValue>lower) && (mValue<upper))
        return true;
    else
        return false;
}



