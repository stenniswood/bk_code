/*
 *
 *
 *  
 *
 */
 
#include <math.h>
#include <stdlib.h>

#include "fft.h"

int n;
int nu;

int bitrev(int j) 
{
	int i;
    int j2;
    int j1 = j;
    int k = 0;
    for (i = 1; i <= nu; i++) {
        j2 = j1/2;
        k  = 2*k + j1 - 2*j2;
        j1 = j2;
    }
    return k;
}

void ShortToFloat(short* in, float* out, int length) 
{
	//float* out = (float*)malloc( sizeof(float)*length );
	int i;
	for (i=0; i<length; i++) {
		out[i] = (float) in[i];  
	}
}

void FloatToShort(float* in, short* out, int length) 
{
	//short* out = (short*)malloc( sizeof(short)*length );
	int i;	
	for (i=0; i<length; i++) {
		out[i] = (short) in[i];  // Good cast? or do we need to convert?
	}
}

void fftMag(float* x, int length, float* mag) 
{
    // assume n is a power of 2
    n = length;
    nu = (int)(log(n)/log(2));
    int n2 = n/2;
    int nu1 = nu - 1;
    float* xre = (float*)malloc( sizeof(float)*n  );
    float* xim = (float*)malloc( sizeof(float)*n  );
    //float* mag = malloc( sizeof(float)*n2 );
    float tr, ti, p, arg, c, s;
	int i;
    int r;
    int k = 0;
    int l;
    for (i = 0; i < n; i++) {
        xre[i] = x[i];
        xim[i] = 0.0f;
    }
    for (l = 1; l <= nu; l++) {
        while (k < n) {
            for (i = 1; i <= n2; i++) {
                p = bitrev (k >> nu1);
                arg = 2 * (float) M_PI * p / n;
                c = (float) cos (arg);
                s = (float) sin (arg);
                tr = xre[k+n2]*c + xim[k+n2]*s;
                ti = xim[k+n2]*c - xre[k+n2]*s;
                xre[k+n2] = xre[k] - tr;
                xim[k+n2] = xim[k] - ti;
                xre[k] += tr;
                xim[k] += ti;
                k++;
            }
            k += n2;
        }
        k = 0;
        nu1--;
        n2 = n2/2;
    }
    k = 0;

    while (k < n) {
        r = bitrev (k);
        if (r > k) {
            tr = xre[k];
            ti = xim[k];
            xre[k] = xre[r];
            xim[k] = xim[r];
            xre[r] = tr;
            xim[r] = ti;
        }
        k++;
    }

    mag[0] = (float) (sqrt(xre[0]*xre[0] + xim[0]*xim[0]))/n;
    for (i = 1; i < n/2; i++)
        mag[i]= 2 * (float) (sqrt(xre[i]*xre[i] + xim[i]*xim[i]))/n;
    return;
}


void KeepEvery2( float* mVector, int mLength )				  
{
	int i  =0;
	int bin=0;
	for ( bin=0, i=0; bin<mLength; i++, bin+=2 )
	{
		mVector[i] = mVector[bin];
	}
}

void KeepEvery8(  float* mVector, int mLength  )				
{
	int i=0;
	int bin=0;
	for ( bin=0, i=0; bin<mLength; i++, bin+=8 )
	{
		mVector[i] = mVector[bin];
	}
}

/* Each value will be the sum of 8 consequative values.
 * Bins will be dimensioned /8 of the mVector 
 * RETURN:  number of bins (mLength/8)
 */
int Bin8( float* mVector, int mLength )	
{
	int bin	=0;
	int j	=0;
	int k   =0;
	int tmpValue;

	for ( j=0; j<mLength; j+=8 )
	{
		tmpValue = 0;
		for ( k=j; k<j+8; k++ )			// Add up all 8
		{
			tmpValue += mVector[k];
		}
		mVector[bin] = tmpValue;		// Store it.
		bin++;
	}
 	return bin;
}

/*
 * Bins will be dimensioned /8 of the mVector 
 * RETURN:  number of bins.
 */
#define BIN4 4
int Bin4( float* mVector, int mLength )	
{
	int bin	=0;
	int j	=0;
	int k   =0;
	int tmpValue;

	for ( j=0; j<mLength; j+=BIN4 )
	{
		tmpValue = 0;
		for ( k=j; k<j+BIN4; k++ )
		{
			tmpValue += mVector[k];
		}
		mVector[bin] = tmpValue;
		bin++;
	}
 	return bin;
}
    
