#include <string.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <float.h>

#include "dataseries.hpp"


DataSeries::DataSeries()
//:m_data(10240, { 0.0, 0.0 } )
{
	strcpy (m_name, "unnamed");	
}

DataSeries::~DataSeries()
{

}
	
void DataSeries::delete_all( )
{
	m_data.erase( m_data.begin(), m_data.end() );
}

void	DataSeries::append_datum		( struct stDataPoint mNewDataPoint )
{
	m_data.push_back( mNewDataPoint );
}

void	DataSeries::scroll_new_data		( struct stDataPoint mNewDataPoint )
{
	
	m_data.erase( m_data.begin() );
	m_data.push_back( mNewDataPoint );
	reindex_x();
}

void	DataSeries::reindex_x			()
{
	size_t len = size();
	for (int i=0; i<len; i++)
		m_data[i].x = 100 * ((float)i/(float)len);
}

void	DataSeries::set_name			( const char* mName )
{
	strcpy(m_name, mName );	
}

double	DataSeries::get_min_x()
{
	double min_dp = FLT_MAX;	
	size_t len = m_data.size();
	for (int i=0; i<len; i++)
	{
		if (m_data[i].x < min_dp)	min_dp = m_data[i].x;
	}
	return min_dp;

}
double	DataSeries::get_max_x()
{
	double max_dp = -FLT_MAX;	
	size_t len = m_data.size();
	for (int i=0; i<len; i++)
	{
		if (m_data[i].x > max_dp)	max_dp = m_data[i].x;
	}
	return max_dp;

}

double	DataSeries::get_min_y()
{
	double min_dp = FLT_MAX;	
	size_t len    = m_data.size();
	for (int i=0; i<len; i++)
	{
		if (m_data[i].y < min_dp)	min_dp = m_data[i].y;
	}
	return min_dp;
}

double	DataSeries::get_max_y()
{
	double max_dp = -FLT_MAX;	
	size_t len = m_data.size();
	for (int i=0; i<len; i++)
	{
		if (m_data[i].y > max_dp)	max_dp = m_data[i].y;
	}
	return max_dp;
}



void 	DataSeries::compute_average		( )
{
	size_t len = m_data.size();
	double avg = 0;
	for (int i=0; i<len; i++)
	{
		avg += m_data[i].y;
	}
	avg /= len;
	printf("Computed Average = %7.4f;  n=%ld\n", avg, len);
	m_mean = avg;	
}

void 	DataSeries::compute_stddev 		( )
{
	size_t len = m_data.size();
	double stddev = 0;
	for (int i=0; i<len; i++)
	{
		float dev = (m_data[i].y - m_mean);
		stddev += (dev * dev);
	}	
	stddev /= len;
	m_stddev = sqrt( stddev );
}

void	DataSeries::set_color( struct stColor mColor )
{
	m_color = mColor;
}

void	DataSeries::print_data			()
{
	printf("DataSeries: \n");
	size_t len = size();
	for (int i=0; i<len; i++)
	{
		printf("%7.4f, ", m_data[i].y);
		
	}
}
	