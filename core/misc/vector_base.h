#ifndef _VECTOR_BASE_H_
#define _VECTOR_BASE_H_

#ifdef  __cplusplus
extern "C" {
#endif

struct tXYZ
{
	short x;
	short y;
	short z;
};

struct fXYZ
{
	float x;
	float y;
	float z;
};

struct frXYZ
{
	float rx;		// Rotation about X axis
	float ry;		// Rotation about Y axis
	float rz;		// Rotation about Z axis
};


union uXYZ 
{
	uint8_t  array[6];	// byte access
	struct tXYZ	 coords;	// structure
	uint16_t value[3];	// xyz as an array
};

#ifdef  __cplusplus
}
#endif

#endif
