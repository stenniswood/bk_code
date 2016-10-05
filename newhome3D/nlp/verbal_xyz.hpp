#ifndef _VERBAL_XYZ_
#define _VERBAL_XYZ_


#include <vector>

using namespace std;
//#include "ThreeSpace_defs.h"
#include "vector.hpp"

/*********************************************************************
	Class Object  connects words in the sentence with actual physical 
	objects in the database.  Note uses the home_inventory database
	instead of the nlp_dictionary!
*********************************************************************/
void connect_inventory_db();


class Verbal_XYZ
{
public:
	Verbal_XYZ();
	~Verbal_XYZ();

	// THREED
	void			set				( float x, float y, float z );
	void			set				( MathVector xyz );	
	void			add_point_nq	( );
	void			retrieve_point_nq( int m_id	);
	void 			update_point_nq	( int m_id  );
	size_t 			query			( bool mRetrieving=true );

	void			sql_add			( );
	void			sql_load		(int m_id);
	void			sql_update		( );

	// These will locate relatively:
	void			set_orientation ( Verbal_XYZ mPeer, vector<float> mForwardUnitVector );
	void			compute_next_to ( Verbal_XYZ mPeer );
	void			compute_left_of ( Verbal_XYZ mPeer );
	void			compute_right_of ( Verbal_XYZ mPeer );
	void			compute_above_of ( Verbal_XYZ mPeer );
	void			compute_below_of ( Verbal_XYZ mPeer );	
	void			compute_front_of ( Verbal_XYZ mPeer );
	void			compute_back_of ( Verbal_XYZ mPeer );		// behind

	void			print_info		(		);
	void 			print_contents	( 		);	

	float			m_x;
	float			m_y;
	float			m_z;
	int				m_threed_id;		// index into threed table of database

	// These are all Peer object info 
/*	vector<float>	m_forward_unit_vector;
	vector<float>	m_front_left_corner;
	vector<float>	m_box;  */
		
	MYSQL_RES*		result;
	int				row_count;
	string			query_string;

};



#endif

