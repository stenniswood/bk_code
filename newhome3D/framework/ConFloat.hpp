//
//  ConFloat.hpp
//  home3D
//
//  Created by Stephen Tenniswood on 4/8/16.
//  Copyright © 2016 Stephen Tenniswood. All rights reserved.
//

#ifndef ConFloat_hpp
#define ConFloat_hpp

#include <stdio.h>

/*   A controlled float
        This float will never be over or under specified limits.
 
 */
class CtrlFloat
{
public:
    CtrlFloat();
    CtrlFloat(float mInitValue);
    ~CtrlFloat();
    float operator+=(float mRHV);
    float operator-=(float mRHV);
    float operator++();

private:
    float m_Lowest_limit;
    float m_Highest_limit;
    float m_Wrap_around;
};


class Range
{
public:
    Range( float mMin, float mMax);
    
    bool inRange( float mValue );
    
private:
    float   Min;
    float   Max;
    
};


class LearnedRange
{
public:
    LearnedRange    ( float mAvg, float mStd );
    
    void learn_value( float mValue );       // add a new value which is to be considered in range.
    bool inRange    ( float mValue );

private:
    float   Avg;
    float   StdDev;

    float   Sum;
    long int   Count;
};


#endif /* ConFloat_hpp */
