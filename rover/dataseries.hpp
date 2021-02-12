#ifndef _DATASERIES_HPP_
#define _DATASERIES_HPP_


#include <vector>

struct stColor {
	float red;
	float green;
	float blue;
	float alpha;
};

struct stDataPoint {
	double x;
	float  y;
};


class DataSeries
{
public:
	DataSeries();
	DataSeries( const DataSeries& ds );
	~DataSeries();
	

	void	append_datum		( struct stDataPoint mNewDataPoint );
	void	scroll_new_data		( struct stDataPoint mNewDataPoint );
	void	reindex_x			();

	void	set_name			( const char* mName );
	char* 	get_name			(  )	{ return m_name;  };

	void	set_scale			( float mScale )		{ m_scale = mScale; };
	float 	get_scale			( )						{ return m_scale;  };
	void	set_yoffset			( float mYoffset )		{ m_yoffset = mYoffset; };
	float 	get_yoffset			( )						{ return m_yoffset;  };

	
	struct stDataPoint*	get_data( int mIndex )		{ return &(m_data[mIndex]); };
	struct stDataPoint*	get_scaled_data( int mIndex );

	void	set_color			( struct stColor mColor );
	struct stColor	get_color	()		{ return m_color;  };
	void	delete_all			();
	size_t	size				()		{ return m_data.size(); };
	

	double	get_min_x			();
	double	get_max_x			();
	double	get_min_y			();
	double	get_max_y			();

	void 	compute_average		( );
	void 	compute_stddev 		( );
	double	get_mean			()	{ return m_mean;   };
	double	get_stddev			()  { return m_stddev; };
	//void	linefit();	
	void	print_data			();
	
protected:


private:
	float	m_scale;
	float	m_yoffset;
	std::vector<struct stDataPoint>	m_data;
	char 	m_name[128];
	double	m_mean;
	double	m_stddev;
	struct stColor	m_color;	

};



#endif
