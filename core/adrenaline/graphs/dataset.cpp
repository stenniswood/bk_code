// line graph OpenVG program
// Steve Tenniswood
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include "VG/openvg.h"
#include "VG/vgu.h"
#include "/home/pi/openvg/fontinfo.h"
#include "/home/pi/openvg/shapes.h"
#include "dataset.hpp"

//
DataSet::DataSet( int NumberElements )
{
	Data = new float[NumberElements];
	n    = NumberElements;
	last_filled_index = 0;	
	Next = NULL;
	Prev = NULL;
}

DataSet::DataSet( float* data, int NumberElements )
: Data(data), n(NumberElements), last_filled_index(0)
{
	Next = NULL;
	Prev = NULL;
}

float DataSet::calc_average( )
{
	float sum = 0.0;
	for (int i=0; i<last_filled_index; i++)
		sum += Data[i];
	average = sum / ((float)last_filled_index);
	return average;
}

float DataSet::calc_stddev( )
{
	float dev_sum = 0.0;
	float diff = 0.0;
	for (int i=0; i<last_filled_index; i++)
	{
		diff = (Data[i]-average);
		dev_sum += (diff * diff);
	}
	stddev = sqrt(dev_sum / ((float)last_filled_index)); 
	return stddev;
}

float DataSet::calc_max( )
{
	max = Data[0];
	for (int i=0; i<last_filled_index; i++)
	{
		if (Data[i] > max)
			max = Data[i];			
	}
	return max;
}

float DataSet::calc_min( )
{
	min = Data[0];
	for (int i=0; i<last_filled_index; i++)
	{
		if (Data[i] < min)
			min = Data[i];			
	}
	return min;
}

void DataSet::add( float new_member ) 
{
	Data[last_filled_index++] = new_member;
	if (last_filled_index> n)
		last_filled_index = n;
}

void DataSet::shift( )
{
	for (int i=0; i<n; i++)
		Data[i] = Data[i+1];
	Data[n] = 0;

	last_filled_index--;
}

// removes first element adds new to end.
void DataSet::shift_add( float new_member )		
{
	if (last_filled_index >= n)	
		shift();
	add(new_member);	
}

int DataSet::count_samples_between( float min, float max )
{
	int count=0;
	for (int i=0; i<last_filled_index; i++)
	{
		if ((Data[i] > min) && (Data[i] < max))
			count++;
	}
	//printf("==========%d count_samples_between %6.1f %6.1f\n", count, min, max);
	return count;
}
	
void  DataSet::compute_stats()	// Does the crunching
{
	calc_average();
	calc_max();
	calc_min();
	calc_stddev();
	//printf("================dataset Average=%6.3f=========\n", average);
	//for (int i=0; i<last_filled_index; i++)
	//	printf(": %6.1f  ", Data[i]);
}

/* this is done in specific graph
float DataSet::calc_scale( )
{
	float max = get_max(Data, n);
	float min = get_min(Data, n);

	float xpixels = mInfo->right - mInfo->left;
	float ypixels = mInfo->top - mInfo->bottom;

	mInfo->xscale = 1.0; //(float)n / xpixels;
	mInfo->yscale = ypixels/(max-min);

	mInfo->xoffset = 0;
	mInfo->yoffset = min;
} */


