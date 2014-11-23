
#ifndef _FFT_H_
#define _FFT_H_


int 	bitrev		( int j							);

void	ShortToFloat( short* in, float* out, int length);
void	FloatToShort( float* in, short* out, int length);

void 	fftMag		( float* x,  int length, float* mag);
float 	KeepEvery2	( float* mVector, int mLength   );
float 	KeepEvery8	( float* mVector, int mLength   );
int   	Bin8	  	( float* mVector, int mLength   );
int		Bin4		( float* mVector, int mLength 	);


#endif 
