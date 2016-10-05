//
//  stats.cpp
//  home3D
//
//  Created by Stephen Tenniswood on 7/11/15.
//  Copyright (c) 2015 Stephen Tenniswood. All rights reserved.
//

#include "stats.h"
#include <math.h>




q_stat::q_stat()
{
    
}
q_stat::~q_stat()
{
    
}

double   q_stat::average()
{
    double sum;
    long size = m_elements.size();
    for (int i=0; i<size; i++)
        sum += m_elements[i];
    m_average = sum / size;
    return m_average;
}

double   q_stat::stddev()
{
    long size = m_elements.size();
    for (int i=0; i<size; i++)
        m_stddev += (m_elements[i] - m_average)*(m_elements[i] - m_average);
    return (m_stddev / size);
}

double q_stat::population()
{
    double m_stddev = stddev();
    return sqrt(m_stddev);
} // sqrt of variance

void q_stat::crunch()
{
    average();
    stddev();
    population();
}


/*********************************************************************/

xy_stat::xy_stat()
{
    
}

xy_stat::~xy_stat()
{
    
}

void xy_stat::linefit()
{
    double sumX=0.0;
    double sumY=0.0;
    double sumX2=0.0;
    double sumXY=0.0;
    int    count=0;
    
    for (int i=0; i<m_xs.size(); i++)
    {
        sumX += m_xs[i];
        sumY += m_ys[i];
        sumX2 += m_xs[i]*m_xs[i];
        sumXY += m_xs[i]*m_ys[i];
    }
    double meanX = sumX / count;
    double meanY = sumY / count;
    
     Slope = (sumXY - sumX * meanY) / (sumX2 - sumX * meanX);
     Yintercept = meanY - Slope * meanX;
}



