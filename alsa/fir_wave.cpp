#include "fir_wave.hpp"



FIRWave::FIRWave()
{

}

FIRWave::~FIRWave()
{

}

void FIRWave::apply_fir( float* mIR, int length)
{
	// Resize : 
	int num_samples   = m_bytes_recorded/m_block_align;
	int final_samples = num_samples + length*m_block_align;
	
	short *tmp        = new short[final_samples]();
	//m_bytes_recorded  = m_buffer_length = num_samples*2;	

	for (int s=0; s<m_bytes_recorded; s+=m_block_align )
	{
		int idex = s*m_number_channels+0;
		for (int ss=0; ss<m_bytes_recorded; ss+= m_block_align )
			tmp[s] = mIR[s] * m_data[ss];		
	}

	delete m_data;
	m_data = tmp;
}

void FIRWave::tremolo  ( float mRatio, float mFrequency )
{

}
