#include <map>
#include <string>
#include <cfloat>
//#include <stdlib>
#include <cstdio>


#ifndef _FUZZY_HPP_
#define _FUZZY_HPP_


using namespace std;

#define UNBOUND_HIGH FLT_MAX
#define UNBOUND_LOW FLT_MIN

struct stRange
{
	float low;
	float high;
};
typedef map<string,struct stRange> range_map;
extern range_map ranges;


void print_range(float mLow, float mHigh);
void print_range(struct stRange);
void print_range(string mKey, range_map mRanges );

bool in_range( float mValue, float mLow, float mHigh );
bool above	 ( float mValue, float mLow, float mHigh );
bool below	 ( float mValue, float mLow, float mHigh );

bool in_range( float mValue, struct stRange mRange );
bool above	 ( float mValue, struct stRange mRange );
bool below	 ( float mValue, struct stRange mRange );

// Use above global ranges map of stored ranges for comparision:
bool in_range( float mValue, std::string mKey );
bool above	 ( float mValue, std::string mKey );
bool below	 ( float mValue, std::string mKey );

string find_range( float mValue, range_map mRanges );

void test_fuzzy();

#endif
