#include <stdio.h>
#include <math.h>
#include <stdlib.h>
//#include <string>

#include "dsp_funcs.h"


// num should be a power of two - ie. 256, 512, etc. not 255!
t_index Order_Log2(t_index num)
{
	t_index test_window_width,order;
	order=0;

	do 	{
		order++;
		test_window_width = 1L<<order;
		if (test_window_width > num) {
			printf("FFT not a power of 2 dimension required");
			return -test_window_width;
		}
	} while(test_window_width != num);

	return order;
}


// looks like a binary sort...
void shuffle(t_real* vr, t_real* vi, t_signed order)
{
	int i,j,k,n2,i_or;
	t_real t;

	i_or = (int)order;
	n2 = (1<<i_or)>>1;				// ie 128
	j=1;
	for (i=1; i<(1<<i_or); i++) 	// sample 1 to 256 (ie scan the window)
	{
		if (i<j)
			{
			t     =  vr[i];		// swap  vr[i] and vr[j] 
			vr[i] =  vr[j];
			vr[j] =  t;
			t     =  vi[i];		// swap  vi[i] and vi[j]
			vi[i] =  vi[j];
			vi[j] =  t;
			}
		k = n2;					// ie 128
		while (k<j)
			{
			j -= k;
			k >>= 1;			// ie 64, 32, 16, 8, 4, 2, 1
			}
		j += k;  				// 
	}
	return;
}


/*
 *****************************************************************************
 * given the complex vector with real part vr[1..n] and imaginary part,      *
 * vi[1..n], computes the FFT on NN=2^order points and puts results in the   *
 * vr[1..NN], vi[1..NN] vector position, if SIGN>=0                          *
 * computes the reverse FT if SIGN is negative                               *
 * the routine SHUFFLE is referenced.                                        *
 *****************************************************************************
*/
void xft(t_real* vr, t_real* vi, t_signed order, t_signed s)
{
	//register 
	int i,j,n,n2;
	int z,k,i_or;
	t_real wr,wi,arg,dwr,dwi,tr,ti,theta;

	if (s >= 0)   theta =  (t_real)acos( -1.0 );
	else          theta=   (t_real)(-1.0*acos( -1.0));

	shuffle(vr,vi,order);
	n2=1;
	i_or = (int)order;
	for (i=1; i<=i_or; i++)
		{
		n=n2;
		n2 += n2;
		wr= (t_real)1.0;
		wi= (t_real)0.0;
		arg=theta/(t_real)n;
		dwr= (t_real)cos( (t_real)arg);
		dwi= (t_real)sin( (t_real)arg);
		for (j=1; j<=n; j++)
			{
			for (k=j; k<=(1<<i_or); k+=n2) 
				{
				z = k + n;
				tr = vr[z]*wr + vi[z]*wi;
				ti = vi[z]*wr - vr[z]*wi;
				vr[z] = vr[k] - tr;
				vi[z] = vi[k] - ti;
				vr[k] += tr;
				vi[k] += ti;
				}
			tr = wr*dwr - wi*dwi;
			wi = wi*dwr + wr*dwi;
			wr = tr;
			}
		}
	return;
}


void Magnitude_And_Energy_Of_Real_Vectors( t_real* v_mag, 
		t_real* v_imm,  t_index size,
		t_real &energy )
{
	t_index len = size / 2;		// SJT Why only half? symetrical fft?
	t_index j;
	energy = 0;

	for(j=0;j< len;j++)
		{
		v_mag[j] = v_mag[j]*v_mag[j] + v_imm[j]*v_imm[j];
		energy += v_mag[j];
		v_mag[j] = sqrt( v_mag[j] );
		}

	energy = sqrt( energy )*2.0;
	return;
}

t_real compute_triangular_step( t_signed delta )
{
	/* compute the decrement step unit for triangular filters */
	t_real b = 1.0 + (t_real)(delta);
	b = (b-0.5);
	b = 1.0/b;
	// ie.  		b = 1/(0.5+delta)
	return b;
}

/*
 *****************************************************************************
 * ------------------------------------------------------------------------- *
 * REFERENCES                                                                *
 * Davis, Mermelstein "Comparison of parametric representation ..."          *
 * ASSP Vol.28, No.4, August 80, pp.357                                      *
 *****************************************************************************
*/
//  clock_t MFCCTime;
/*  
INPUT:
	dp_fft	- log magnitude of fft
	l_nfft	- size of the fft window (in samples)
	l_nmfcc	- size of the requested mfcc
	d_sam	- band * 2 	
OUTPUT:
	dp_mfcc[0..l_nmfcc] - the output
*/
void mfcc( t_real  *dp_fft, t_real * dp_mfcc, t_real d_sam,
	  	   t_signed l_nfft, t_signed l_nmfcc )
{
	static int BUFFER_SIZE = 1024;
	static char   *buffer = (char*)malloc(BUFFER_SIZE);
	t_real   band,rowamp,onek;
	t_real  *filterbank;
	t_signed i,j,n,k,nfilterbank;
	t_signed delta,left,center,right;
	t_real   a,b,pigr;

	t_real zero = 0.0;
	t_real fdelta;
	t_real factor=1.2;

	band   = d_sam / 2.0;			    // like the number of samples from zero to zero
	rowamp = band  / (t_real)l_nfft;	// 

	// Warn of non Power of 2 window size :
	if( l_nfft!=64L && l_nfft!=128L && l_nfft!=256L && l_nfft!=512L && l_nfft!=1024)
		{
		printf("\nIN MFCC <l_nfft> IS %ld overflow in mfcc coefficient", l_nfft );
		}

	// Filter bank is size of WINDOW + 4
	size_t bsize = (sizeof(t_real)*(size_t)( l_nfft+4L ));
	if (bsize != BUFFER_SIZE) {
		free (buffer);
		buffer = (char*)malloc(bsize);
	}
	filterbank=(t_real *)&(buffer[0]);

	// Zero out the filter bank : 
	for(i=0L;i<=l_nfft;i++)
			filterbank[i]=zero;
/*
	********************************************************
	*                                                      *
	*   INITIALIZE FILTER DEFINITION IN LINEAR PART        *
	*                                                      *
	********************************************************
*/
	i       = 1L;
	onek    = 1000.0/ rowamp;      /* rows in 1kHz */
	fdelta  = onek  / 10.0;        /* we want about 10 cep uder 1kHz */
	delta   = (t_signed)fdelta;
	n       = 1L;
	left    = n;
	center  = n + delta;
	right   = n + 2L*delta;
	//printf("n=1; delta=%ld; onek=%6.2f\n", delta, onek );
/*
	********************************************************
	*                                                      *
	*   NOW COMPUTE  THE LINEAR SCALE BANKFILTER  f<1kHz   *
	*                                                      *
	********************************************************
*/
	/* until the central boundary is lower than 1kHz */
	while(n < (t_signed)onek )
	{
		filterbank[i] = 0.0;
		/* b is the step unit */
		b = compute_triangular_step( delta );
		
		// Only go from left side of triangle to right.  Filter is zero otherwise.
		for(j=left; j<=right; j++ ) 
		{
			/* a is the point distance from the centre of the
			   triangular filter	*/
			a = fabs( (t_real)center - (t_real)j);
			a = 1.0 - a*b;
			/* a is the factor<1 to amplify row(j) */
			/* update filter i in filterbank computation */
			filterbank[i] += dp_fft[j]*a;
			/* Yeah, filterbank[] is just 1 number - the output of each filter.
			   the a is the triangular shape and zero elsewhere which is multiplying by the FFT.
			   so this plus is doing the integrating of the spectrum.
			*/ 
		}
			
		/* NOW
		   1) compute next filter i -> i+1
		   2) delta is still the same (linear grows)
		   3) filter center moves up of delta points
		   4) left and right boundary are updated		*/
		n      = n + delta;
		i      = i +1L;
		left   = n;
		center = n + delta;
		right  = n + 2L*delta;
		}
		//printf("mfcc lower 1khz done; i=%ld; \n", i-1);
		
//	********************************************************
//	*                                                      *
//	*   NOW COMPUTES THE BANKFILTER  f>1kHz      		   *
//	*                                                      *
//  * Gives num filters needed to reach end of window for  *
//  * the fft.											   *
//	********************************************************

	// until the right boundary is inside the FFT 
	while(right < l_nfft ) 
	{
		filterbank[i] = 0.0; // i continues from previous loop.  Does not start over at filterbank 0!

		/* compute the decrement step unit for triangular filters */
		b = compute_triangular_step( delta );
		// Only go from left side of triangle to right.  Filter is zero otherwise.
		for(j=left; j<=right; j++ ) 
			{
			/* a is the point distance from the centre of the
			   triangular filter
			*/
			a = fabs( (t_real)center - (t_real)j);
			a = 1.0 - a*b;
			// a is the factor<1 to amplify row(j) 
			// update filter i in filterbank computation
			filterbank[i] += dp_fft[j]*a;
			}
		/* NOW
		   1) compute next filter i -> i+1
		   2) delta grows exponentially delta -> delta*factor
		   3) filter center moves up of delta points
		   4) left and right boundary are updated		*/
		fdelta = factor*fdelta;
		delta  = (t_signed) fdelta;

		i      = i +1L;		
		n      = n + delta;
		left   = n;
		center = n + delta;
		right  = n + 2L*delta;
	}
	//printf("mfcc above 1khz done; i=%ld; \n", i-1 );

//	********************************************************
//	*                                                      *
//	*   LOG ENERGY FILTERBANK AND CEPSTRUM COMPUTATION     *
//	*                                                      *
//	********************************************************
	nfilterbank = i-1;                /* the number of filter */
	if( nfilterbank < l_nmfcc )
		printf("\nIN MFCC <nfilter> IS %ld AND <nmfcc> IS %ld", nfilterbank, l_nmfcc );

	// LOG computation of all filterbank outputs,
	//   no negative values are allowed, if E<1 => E=1.
	for(i=1;i<=nfilterbank;i++)
		{
		if( filterbank[i] >= 1.0 ) 
			{
			filterbank[i] = 20.0*log10( filterbank[i] );
			} else  {
					filterbank[i] = 0.0;
					}
		}
	
	/* the cosine transformation to obtain MFCC */
	pigr = acos(-1.0);
	for(i=0;i<l_nmfcc;i++)
		{
		dp_mfcc[i] = 0.0;
		for(k=1;k<=nfilterbank;k++) 
			{
			a = (t_real)k - 0.5;
			a = a * pigr * (t_real)(i+1);
			a = a/(t_real)nfilterbank;
			a = cos(a);
			dp_mfcc[i] += filterbank[k] * a;
			}
		}

//  free( (char *)filterbank);
//	clock_t FinalT = clock();
//	MFCCTime      += (FinalT - InitT);
	return ;
}
