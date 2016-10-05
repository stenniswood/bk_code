//
//  stats.h
//  home3D
//
//  Created by Stephen Tenniswood on 7/11/15.
//  Copyright (c) 2015 Stephen Tenniswood. All rights reserved.
//

#ifndef __home3D__stats__
#define __home3D__stats__

#include <stdio.h>
#include <vector>
#include <string>
using namespace std;

class q_stat
{
public:
    q_stat();
    ~q_stat();
    
    double   average();
    double   stddev();
    double   population();       // sqrt of variance.
    void     crunch();
    
    string			m_name;		// for debug, showing calculations.
    vector<double>  m_elements;
    
    double          m_average;
    double          m_stddev;    
};


class xy_stat
{
public:
    xy_stat();
    ~xy_stat();
    void linefit();
    
    vector<double>  m_xs;
    vector<double>  m_ys;
    
    double Slope;
    double Yintercept;
};



#endif /* defined(__home3D__stats__) */
