
short Volume = 3000;


void square_wave( short* mData, int mChannels, int mPeriodSamples, int mBuffSize )
{
	int i,last_edge=0;
	int value = Volume;
	
	for (i=0; i<mBuffSize; i++)
	{	
		if ((i % mPeriodSamples)==0)
		{
			last_edge = i;
			value = Volume;
		}		
		else if (i==(last_edge + mPeriodSamples/2.))
			value = -Volume;		
		
		mData[i] = value;
	}
}


float calc_period( float Herz, float sample_rate, float mchannels )
{
	float period_samples = (sample_rate/Herz)*mchannels;
	return period_samples;
}