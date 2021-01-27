#include <assert.h>

#include "interpolator.hpp"



OneLimbInterpolator::OneLimbInterpolator( )
{
}

OneLimbInterpolator::OneLimbInterpolator( int mNumInterpolations, int mNumServos )
{
	redimension( mNumInterpolations, mNumServos );
}

OneLimbInterpolator::~OneLimbInterpolator()
{

}

void OneLimbInterpolator::redimension( int mNumInterpolations, int mNumServos )
{
	m_num_points = mNumInterpolations;

	// 
	m_currpos.limb_num = 0;
	for (int i=0; i < mNumServos; i++)
		m_currpos.m_angles.push_back( 0.0 );
		
	// Create an Empty Sequence : 
	for (int i=0; i<mNumInterpolations; i++)
		m_Sequence.push_back( m_currpos );	
}

/* Given 2 vectors, create n vector sequence so that the end points are reached
   simultaneously */
void OneLimbInterpolator::time_resample(
				struct stFloatVector mStart, 
				struct stFloatVector mEnd )
{
	assert( mStart.m_angles.size() == mEnd.m_angles.size() );

	// Compute a Delta Vector:
	struct stFloatVector delta;	
	for ( int i=0; i<mStart.m_angles.size(); i++)
	{
		delta.m_angles.push_back((mEnd.m_angles[i] - mStart.m_angles[i]) / (float)(m_num_points-1));
	}
	printf("delta size = %d\n", delta.m_angles.size() );

	// Initialize intermediate (with mStart):
	struct stFloatVector intermediate;
	intermediate.limb_num = mStart.limb_num;
	
	for ( int i=0; i< mStart.m_angles.size(); i++)
		intermediate.m_angles.push_back( mStart.m_angles[i] );
	//printf("intermediate size = %d\n", intermediate.m_angles.size() );

	// UPDATE SEQUENCE : 
	for ( int step=0; step<m_num_points; step++ )
	{
		m_Sequence[step] = intermediate;

		//printf("mSequence size = %d\n", mSequence.size() );		
		// Update Next Intermediate:
		for (int i=0; i<3; i++)
		{
			intermediate.m_angles[i] += delta.m_angles[i];
		}
	}
}


void  OneLimbInterpolator::set_new_vector( struct stFloatVector mNewPos )
{
	assert(mNewPos.limb_num == m_currpos.limb_num );
		
	time_resample( m_currpos, mNewPos );	
	m_currpos = mNewPos;
}


void  OneLimbInterpolator::get_play_vector( struct stFloatVector& mNewPos, int mIndex )
{
	if (mIndex==-1) {
		mNewPos = m_Sequence[m_index];
		m_index++;
	} else 
		mNewPos = m_Sequence[mIndex];
}

void  OneLimbInterpolator::print_sequence( )
{
	for (int n=0; n<m_num_points; n++)
	{
		printf("%d:  ", n);
		m_Sequence[n].print();
	}		
}

///////////////////////////////////////////////////////////

FullInterpolator::FullInterpolator(  )
{
	//redimension( mNumInterpolations );
}

FullInterpolator::~FullInterpolator()
{

}

void FullInterpolator::get_play_vector( int mLimb, 
						struct stFloatVector& mNewPos, 
						int mSeqIndex )
{
	if (mSeqIndex==-1) {
		mSeqIndex = m_play_index;		
		m_limb[mLimb].get_play_vector( mNewPos, mSeqIndex );
	}
}

/*void FullInterpolator::redimension( int mNumInterpolations )
{
	int num_limbs = mRobot.get_num_limbs();
	m_num_points = mNumInterpolations;
	
	// INIT a OneLimbInterpolator for each limb:
	for (int i=0; i<num_limbs; i++)
	{
		int num_servos = 0;
		Limb* l =  mRobot.get_limb( i );
		if (l)
			num_servos = l->get_num_servos();
			
		OneLimbInterpolator one( m_num_points, num_servos );
		m_limb.push_back( one );
	}	
}*/

void  FullInterpolator::add_empty_limb  ( int mNumServos )
{
	OneLimbInterpolator one;
	one.redimension( m_num_points, mNumServos );
		
	m_limb.push_back( one );
}

