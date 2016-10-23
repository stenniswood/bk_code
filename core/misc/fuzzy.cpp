/*	
	Good going on this file Steve!  Will be very usefull!
*/
#include "fuzzy.hpp"



range_map ranges;


void print_range(float mLow, float mHigh)
{
	printf("Range Low: %6.3f		Range High=%6.3f\n", mLow, mHigh );
}
void print_range(struct stRange range)
{
	printf("Range Low: %6.3f		Range High=%6.3f\n", range.low, range.high );
}
void print_range(string mKey, range_map mRanges)
{
	printf("%s - Range Low: %6.3f	Range High=%6.3f\n", mKey.c_str(), mRanges[mKey].low, mRanges[mKey].high );
}


bool in_range( float mValue, float mLow, float mHigh )
{
	if ((mValue > mLow) && (mValue < mHigh))
		return true;
	return false;
}
bool above	 ( float mValue, float mLow, float mHigh )
{
	if (mValue > mHigh)
		return true;
	return false;
}
bool below	 ( float mValue, float mLow, float mHigh )
{
	if (mValue < mLow)
		return true;
	return false;
}


bool in_range( float mValue, struct stRange mRange )
{
	return in_range( mValue, mRange.low, mRange.high );
}
bool above	 ( float mValue, struct stRange mRange )
{
	return above( mValue, mRange.low, mRange.high );
}
bool below	 ( float mValue, struct stRange mRange )
{
	return below( mValue, mRange.low, mRange.high );
}


// Use above global ranges map of stored ranges for comparision:
bool in_range( float mValue, std::string mKey )
{
	return in_range( mValue, ranges[mKey].low, ranges[mKey].high );
}
bool above	 ( float mValue, std::string mKey )
{
	return above( mValue, ranges[mKey].low, ranges[mKey].high );
}
bool below	 ( float mValue, std::string mKey )
{
	return below( mValue, ranges[mKey].low, ranges[mKey].high );
}

string find_range( float mValue, range_map mRanges )
{
	static string not_found = "not found";
	bool result = false;
	range_map::iterator myiter = mRanges.begin();
	while (myiter != mRanges.end())
	{
		result = in_range( mValue, myiter->second );
		if (result)
			return myiter->first;
		myiter++;
	}
	return not_found;
}


void test_fuzzy()
{
	struct stRange r;
	r.low = -180; r.high=50;	ranges["cold"] = r;
	r.low = 50; r.high=76;	ranges["warm"] = r;	
	r.low = 76; r.high=UNBOUND_HIGH;	ranges["hot"] = r;		
	
	string key = "hot";
	bool result = in_range( 150000, key);
	printf("100 is %s = %d\n", key.c_str(), result);
	 result = in_range( 72, key);
	printf("72 is %s = %d\n", key.c_str(), result);
	 result = in_range( 32, key);
	printf("32 is %s = %d\n", key.c_str(), result);
	
	string tmp = find_range( 120, ranges );
	printf("120 is %s\n", tmp.c_str() );	

	 tmp = find_range( 73, ranges );
	printf("73 %s\n", tmp.c_str() );	

	 tmp = find_range( 32, ranges );
	printf("32 %s\n", tmp.c_str() );	
	
}

