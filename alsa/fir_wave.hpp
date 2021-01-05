#ifndef _FIRWAVE_
#define _FIRWAVE_

#include "wave.hpp"


class FIRWave : public Wave
{
public:
	FIRWave();
	~FIRWave();
	void 	apply_fir( float* mIR, int length);
	void 	tremolo  ( float mRatio, float mFrequency );
	

private:


};


#endif
