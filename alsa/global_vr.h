#ifndef _GLOBAL_VR_H_
#define _GLOBAL_VR_H_

#include <string>
#include <vector>


typedef unsigned long t_index;
typedef double t_real;
typedef long   t_signed;

typedef struct stFeatureVector {
	std::vector<double> d;
	std::string symbol;
} tFeatureVector;

//typedef vector<double> tFeatureVector;

#endif
