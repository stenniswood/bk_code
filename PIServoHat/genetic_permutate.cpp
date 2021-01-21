#include "genetic_permutate.hpp"


void init_genetics()
{
	srand();
}

std::vector<struct stOneVector> permutate_vector( Servo* mServ, struct stOneVector mSeed, int mNumMutations, int mJoint )
{
	float base_angle = mSeed.m_angle[mJoint];
	
	// Do we do small limit around the base_angle or full range?	
	float variance = (mServ->get_cal_max() - mServ->get_cal_min());
	
	std::vector<struct stOneVector> mutations;
	
	for (int M=0; M<mNumMutations; M++)
	{
		float new_angle = base_angle + variance * rand()/RAND_MAX;
		mutations.push_back(new_angle);
	}
	
	return mutations;
}

