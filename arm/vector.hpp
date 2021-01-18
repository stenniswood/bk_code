#ifndef _VECTOR_HPP_
#define _VECTOR_HPP_

#include <list>
#include <vector>
#include <string>

using namespace std;

//extern float radians( float degrees );

/* An object in a 2D floor plan	*/
class MathVector
{
public:
	MathVector ( );
	MathVector ( int mDimension );
    MathVector ( float* mValues, int mDimension );
	MathVector ( string mName, int mDimension );	
	~MathVector( );

	void 			Initialize				(                   );
	void 			print					( bool mLF = true   );
	void 			dimension				( int mDimension    );
	void 			set_xyz                 ( double x, double y, double z );
    double          get_distance            ( MathVector& mPt2  );

	bool		 	operator==(const MathVector &rhs);	
	MathVector& 	operator=(const MathVector &rhs);
	void		 	operator+=(const MathVector &rhs);
    void		 	operator-=(const MathVector &rhs);
    
	void			operator*=( double mScaler );
    MathVector      operator/( double mScaler );
    void			operator/=( double mScaler );
    MathVector 		operator*( double mScaler );
    MathVector 		operator*( MathVector mScaler );        // element by element scaling.
	MathVector	 	operator+(const MathVector &rhs);		
	MathVector	 	operator-(const MathVector &rhs);
	MathVector		get_perp_2d();
    MathVector	    get_perp_xz();

    void            unitize  ( ) { *this /= magnitude(); }
    
	double			magnitude( );
	double		 	dot		 ( const MathVector &rhs);
    MathVector      cross    ( MathVector rhs );
	double&		 	operator[](int index);
	
	string			m_name;		// for debug, showing calculations.
	vector<double>  m_elements;
};

class Line
{
public:
	Line    ( );
    Line    (MathVector pt1, MathVector pt2);
    Line    ( int dimension );
    ~Line   ( );
    
    int			line_intersection( Line mLine2, float& t, float& s );
    MathVector  line_intersection_point( );
    
    void        unitize();
    void        print  ();
    
	MathVector	m_origin;
	MathVector	m_vector;
    float       t;          // scaler to intersection
};

class LineSegment : public Line
{
public:
    LineSegment ();    
    LineSegment (MathVector msource, MathVector mdest );
    bool        is_in_segment( MathVector mPoint);
    
    // 
    int         line_seg_intersection( LineSegment mLine2, float& ss, double* mX, double* mY );
    float       m_length;
    
};

class SampledLineSegment : public LineSegment
{
public:
    SampledLineSegment ();
    SampledLineSegment (MathVector msource, MathVector mdest, float mIncrement=1.0 );
    int                 generate_samples( float mIncrement );

    float               m_increment;
    vector<MathVector>  m_samples;
};

/* Represents a node comprise an entire tree */
class MathVectorTree
{
public:
    MathVectorTree();

    void        clear       ( );
    void        add_node    ( MathVector& mPoint        );
    void        add_nodes   ( list<MathVector>& mPoints );
    void        extract_path( vector<MathVector>& mVector      ); // Walk up the parents to the root. recording the item at each.
    void        extract_path_pointers( vector<MathVectorTree*>& mVector ); // Walk up the parents to the root. recording the item at each.
    
    MathVectorTree*      parent;
    MathVector           item;
    list<MathVectorTree> children;
};




#endif
