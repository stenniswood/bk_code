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
	~DataSeries();

	void	append_datum		( struct stDataPoint mNewDataPoint );
	void	scroll_new_data		( struct stDataPoint mNewDataPoint );
	void	reindex_x			();

	void	set_name			( const char* mName );
	char* 	get_name			(  )	{ return m_name;  };

	struct stDataPoint*	get_data( int mIndex )		{ return &(m_data[mIndex]); };

	void			set_data	( double mData[], int mLength );
	void			set_color	( struct stColor mColor );
	struct stColor	get_color	()		{ return m_color;  };

	void	delete_all			();
	size_t	size				()		{ return m_data.size(); };
	
	void	set_visible			( bool mShow )	{  m_is_visible = mShow; };
	bool	is_visible			()				{ return m_is_visible; };
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
	bool	m_is_visible;
	
	std::vector<struct stDataPoint>	m_data;
	char 	m_name[128];
	double	m_mean;
	double	m_stddev;
	struct stColor	m_color;	

};



#endif
