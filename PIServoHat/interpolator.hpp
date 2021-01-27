#ifndef _INTERPOLATOR_HPP_
#define _INTERPOLATOR_HPP_


#include "vector.hpp"
//#include "robot.hpp"


class OneLimbInterpolator
{
public:
	OneLimbInterpolator();
	OneLimbInterpolator( int mNumInterpolations, int mNumServos );
	~OneLimbInterpolator();
	
	void	redimension( int mNumInterpolations, int mNumServos );
	/* Given 2 vectors, create n vector sequence so that the end points are reached
   simultaneously */
	void 	time_resample  ( struct stFloatVector mStart, 
						     struct stFloatVector mEnd );

	void	set_new_vector ( struct stFloatVector mNewPos  );
	void	get_play_vector( struct stFloatVector& mNewPos, int mIndex=-1 );

	void	restart()		{ m_index = 0; };
	void	print_sequence();
	
private:
	int		m_num_points;
	int		m_index;
	struct stFloatVector			 m_currpos;	// for each limb.	
	
	std::vector<struct stFloatVector> 	  m_Sequence;
};



//////////////////////////////////////////////////////////////////
/* the vector has 1 FloatVector for each body limb */
typedef std::vector<struct stFloatVector> full_body_vector;


class FullInterpolator
{
public:
	FullInterpolator(  );
	~FullInterpolator();

	void	set_num_points( int mNum ) 		  { m_num_points = mNum; };	
	void	clear_dimensions( int mNumLimbs ) { m_limb.clear(); };
	void	add_empty_limb  ( int mNumServos );	
	
	/* Given 2 vectors, create n vector sequence so that the end points are reached
   simultaneously */
	void 	time_resample(	struct stFloatVector mStart, 
							struct stFloatVector mEnd, int n );

	void	set_new_vector ( struct stFloatVector mNewPos );

	void	get_play_vector( int mLimb, struct stFloatVector& mNewPos, int mSeqIndex=-1 );
	void	restart()		{ m_play_index = 0; };
	void	bump_sequence()	{ m_play_index++;   };
	bool	has_completed()	{ return (m_play_index >= m_num_points);   };


private:
	int		m_num_points;
	int		m_play_index;
	std::vector<OneLimbInterpolator> m_limb;	// for each limb

};


#endif


