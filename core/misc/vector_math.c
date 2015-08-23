#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include "bk_system_defs.h"
//#include "mcp2515.h"
#include "pican_defines.h"
#include "CAN_base.h"
#include "packer_lcd.h"
#include "packer.h"
#include "packer_lcd.h"
#include "packer_motor.h"
#include "can_id_list.h"
#include "buttons.h"
#include "leds.h"
#include "vector_math.h"
#include "parser_tilt.h"

//#define _GNU_SOURCE 

/**********************************************
		GENERIC HELPER FUNCTIONS
 *********************************************/
void convert_to_real( struct tXYZ* mIn, struct fXYZ* fOut )
{
	fOut->x = (float)mIn->x;
	fOut->y = (float)mIn->y;
	fOut->z = (float)mIn->z;
}

struct fXYZ average( struct fXYZ mRead1, struct fXYZ mRead2 )
{
	static struct fXYZ avg;
	avg.x = (mRead1.x + mRead2.x) / 2;
	avg.y = (mRead1.y + mRead2.y) / 2;
	avg.z = (mRead1.z + mRead2.z) / 2;
	return avg;
}

/* 
	The weight vectors w1 & w2 together need to add up to 1.
*/
void weighted_average_comp( struct fXYZ *mRead1, struct fXYZ *mRead2, 
					   		struct fXYZ *w1,     struct fXYZ *w2, struct fXYZ *sumW12,
					   		struct fXYZ *mResult )
{
	static struct fXYZ avg;
	struct fXYZ tmp1;
	struct fXYZ tmp2;

	v_product( mRead1, w1,   	&tmp1 		);
	v_product( mRead2, w2,   	&tmp2 		);
	add  	 ( &tmp1,   &tmp2, 	&avg  		);
	v_divide ( &avg,    sumW12, mResult		);
}

struct fXYZ weighted_average( struct fXYZ *mRead1, struct fXYZ *mRead2, 
							  float w1, float w2 )
{
	float sumW = w1 + w2;
	static struct fXYZ avg;
	struct fXYZ tmp1;
	struct fXYZ tmp2;

	scale( mRead1, w1/sumW, &tmp1 );
	scale( mRead2, w2/sumW, &tmp2 );
	add  ( &tmp1,   &tmp2,   &avg  );
	return avg;
}

float magnitude( struct fXYZ* mReadings )
{
	float magnitude = ((mReadings->x * mReadings->x) +
	 	(mReadings->y * mReadings->y) +
	 	(mReadings->z * mReadings->z));
	return sqrt(magnitude);
}

void v_product( struct fXYZ* mA, struct fXYZ* mB, struct fXYZ* mResult )
{
	mResult->x = (mA->x * mB->x);
	mResult->y = (mA->y * mB->y);
	mResult->z = (mA->z * mB->z);		
}

void v_divide( struct fXYZ* mNumerator, struct fXYZ* mDenom, struct fXYZ* mResult )
{
	mResult->x = (mNumerator->x / mDenom->x);
	mResult->y = (mNumerator->y / mDenom->y);
	mResult->z = (mNumerator->z / mDenom->z);
}

void v_reciprocal( struct fXYZ* mInput, struct fXYZ* mResult )
{
	mResult->x = (1 / mInput->x);
	mResult->y = (1 / mInput->y);
	mResult->z = (1 / mInput->z);
}
void zero( struct fXYZ* mA  )
{
	mA->x = 0;
	mA->y = 0; 
	mA->z = 0;
}

void scale( struct fXYZ* mA, float mScaler, struct fXYZ* mResult )
{
	mResult->x = (mA->x * mScaler);
	mResult->y = (mA->y * mScaler);
	mResult->z = (mA->z * mScaler);
}

void mix( struct fXYZ* mA, struct fXYZ* mB, float mAlpha, struct fXYZ* mResult )
{
	struct fXYZ tmpA,tmpB;
	scale( mA,   mAlpha,   &tmpA );
	scale( mB,   1-mAlpha,  &tmpB );
	add  ( &tmpA, &tmpB,     mResult);	// moving average	
}

float dot_product( struct fXYZ* mA, struct fXYZ* mB )
{
	float ab =  (mA->x * mB->x);
	ab += (mA->y * mB->y);
	ab += (mA->z * mB->z);	
	return ab;
}

/*
RETURN:	Angle in radians */
float angle_between( struct fXYZ* mA, struct fXYZ* mB )
{
	float magA = magnitude(mA);
	float magB = magnitude(mB);
	float dp   = dot_product(mA, mB);

	float angle = acos( dp / (magA*magB) );
	return angle;
}

void subtract( struct fXYZ* mFirst, struct fXYZ* mSecond, struct fXYZ* mResult )
{
	mResult->x = (mFirst->x - mSecond->x);
	mResult->y = (mFirst->y - mSecond->y);
	mResult->z = (mFirst->z - mSecond->z);
}
void add	( struct fXYZ* mFirst, struct fXYZ* mSecond, struct fXYZ* mResult )
{
	mResult->x = (mFirst->x + mSecond->x);
	mResult->y = (mFirst->y + mSecond->y);
	mResult->z = (mFirst->z + mSecond->z);
}
float rad_to_deg( float radians )
{
	return radians * 180 / M_PI;
}
float deg_to_rad( float radians )
{
	return radians * M_PI/180;
}

void radians_to_degrees( struct frXYZ* mRad, struct frXYZ* mDeg )
{
	mDeg->rx = (mRad->rx *180 / M_PI);
	mDeg->ry = (mRad->ry *180 / M_PI);
	mDeg->rz = (mRad->rz *180 / M_PI);
}

void degrees_to_radians( struct frXYZ* mDeg, struct frXYZ* mRad )
{
	mRad->rx = (mDeg->rx * M_PI / 180.0 );
	mRad->ry = (mDeg->ry * M_PI / 180.0 );
	mRad->rz = (mDeg->rz * M_PI / 180.0 );
}

void print_ivector( struct tXYZ* mXYZ )
{
	printf("X=%d;  Y=%d;  Z=%d\t", mXYZ->x, mXYZ->y, mXYZ->z );		
}

void print_vector( struct fXYZ* mXYZ )
{
   printf("X=%12.4f;  Y=%12.4f;  Z=%12.4f\t", mXYZ->x, mXYZ->y, mXYZ->z );
}

void print_avector( struct frXYZ* mXYZ )
{
   printf("rX=%10.4f;  rY=%10.4f;  rZ=%10.4f\t", mXYZ->rx, mXYZ->ry, mXYZ->rz );
}

/*********************************************************************
 ******************   	SCALER ROUTINES       ************************
 *********************************************************************/

/* RESULT:
	-1			==>		 m1 < mStandard
	 0			==>		 m1 is within tolerance.
	 1			==>		 m1 > mStandard					*/
char compare_with_tolerance( float value, float mKnown, float mTolerance)
{
	float high	= mKnown + mTolerance;
	float low	= mKnown - mTolerance;

	if (value > high)
		return 1;
	else if (value < low)
		return -1;
	return 0;	
}

