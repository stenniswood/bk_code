/*********************************************************************
 *											   						 *
 *    DESCRIPTION:  Calibrations 		 			 	 			 *
 *											  						 *
 *	  CATEGORY	:  Low level APP									 *
 *											  						 *
 *    AUTHOR	:  Stephen Tenniswood, Embedded Engineer 	 		 *
 *											   						 *
 *    PURPOSE	:  Routines to convert Servo positions into Angles.	 *
 *			   	   and to convert Angles into an <X,Y,Z> coordinate. *
 *																	 *
 *	  DATE		:  Octt 27,2008										 *
 *********************************************************************/
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "APP_InverseCoordinates.h"
#include "APP_CoordinateTransform.h"
#include "APP_Servos.h"
#include "APP_Calibrations.h"


struct stXYZ_Coordinate ArmOrigin;

float LENGTH_SHOULDER 	=4.75;
float LENGTH_ELBOW		=4.75;
float LENGTH_WRIST		=6.0;
float BASE_HEIGHT		=2.5;


FILE* fd=NULL;
char* line=NULL;		// One line from the config file.


void file_open()
{
	fd = fopen("parameters.ini", "r");
	if (fd==NULL)
	{ 	perror("Error open calibration file!");	}	
	
	line = (char*) malloc(255);
}

void read_origin()
{
	uint16_t x,y,z;	
	size_t len = 255;

	// XYZ for the arm
	size_t bytes_in = getline( &line, &len, fd );
	char* ptr = strchr(line, ' ');
	x = atoi( ptr+1 );
	
	// Y
	bytes_in = getline( &line, &len, fd );
	ptr = strchr(line, ' ');
	y = atoi( ptr+1 );

	// Z
	bytes_in = getline( &line, &len, fd );
	ptr = strchr(line, ' ');
	z = atoi( ptr+1 );
	
	ArmOrigin.X = x;
	ArmOrigin.Y = y;
	ArmOrigin.Z = z;			
	printf("Arm Origin at (%d,%d,%d)\n", x,y,z );
}

void read_arm_sizes()
{
	// lengths of joints + base height
	float hbase, lshoulder,lelbow, lwrist;
	size_t len = 255;

	size_t bytes_in = getline( &line, &len, fd );
	char* ptr = strchr(line, ' ');
	lshoulder = atof( ptr+1 );

	bytes_in = getline( &line, &len, fd );
	ptr = strchr(line, ' ');
	lelbow = atof( ptr+1 );

	bytes_in = getline( &line, &len, fd );
	ptr = strchr(line, ' ');
	lwrist = atof( ptr+1 );

	bytes_in = getline( &line, &len, fd );
	ptr = strchr(line, ' ');
	hbase = atof( ptr+1 );
	
	printf("ARM DIMENSIONS:\n  SHOULDER=%6.2f;\n  Elbow=%6.2f;\n  Wrist=%6.2f\n",
			lshoulder, lelbow, lwrist );
	printf("  BaseHeight=%6.2f\n", hbase );
			
}

void read_zero_points()
{
	//  PID value
	size_t len=0;
	size_t bytes_in = getline( &line, &len, fd );
	char* ptr = strchr(line, ' ');
	ServoInfo[0].zero_value = atoi( ptr+1 );
	//ServoInfo[i].zero_angle = 0.0;

	 bytes_in = getline( &line, &len, fd );
	ptr = strchr(line, ' ');
	ServoInfo[1].zero_value = atoi( ptr+1 );

	 bytes_in = getline( &line, &len, fd );
	ptr = strchr(line, ' ');
	ServoInfo[2].zero_value = atoi( ptr+1 );

	 bytes_in = getline( &line, &len, fd );
	ptr = strchr(line, ' ');
	ServoInfo[3].zero_value = atoi( ptr+1 );

	 bytes_in = getline( &line, &len, fd );
	ptr = strchr(line, ' ');
	ServoInfo[4].zero_value = atoi( ptr+1 );

}

void read_min_points()
{
	// Angle & PID value
	size_t len=0;
		
	size_t bytes_in = getline( &line, &len, fd );
	char* ptr = strchr(line, ' ');
	ServoInfo[0].min_angle = atoi( ptr+1 );
	ServoInfo[0].min_value = atoi( ptr+1 );

	 bytes_in = getline( &line, &len, fd );
	ptr = strchr(line, ' ');
	ServoInfo[1].min_angle = atoi( ptr+1 );
	ServoInfo[1].min_value = atoi( ptr+1 );

	 bytes_in = getline( &line, &len, fd );
	ptr = strchr(line, ' ');
	ServoInfo[2].min_angle = atoi( ptr+1 );
	ServoInfo[2].min_value = atoi( ptr+1 );

	 bytes_in = getline( &line, &len, fd );
	ptr = strchr(line, ' ');
	ServoInfo[3].min_angle = atoi( ptr+1 );
	ServoInfo[3].min_value = atoi( ptr+1 );
		
	 bytes_in = getline( &line, &len, fd );
	ptr = strchr(line, ' ');
	ServoInfo[4].min_angle = atoi( ptr+1 );
	ServoInfo[4].min_value = atoi( ptr+1 );

}

void read_max_points()
{
	// Angle & PID value
	size_t len=0;	
	size_t bytes_in = getline( &line, &len, fd );
	char* ptr = strchr(line, ' ');
	ServoInfo[0].max_angle = atoi( ptr+1 );
	ServoInfo[0].max_value = atoi( ptr+1 );

	 bytes_in = getline( &line, &len, fd );
	ptr = strchr(line, ' ');
	ServoInfo[1].max_angle = atoi( ptr+1 );
	ServoInfo[1].max_value = atoi( ptr+1 );

	 bytes_in = getline( &line, &len, fd );
	ptr = strchr(line, ' ');
	ServoInfo[2].max_angle = atoi( ptr+1 );
	ServoInfo[2].max_value = atoi( ptr+1 );

	 bytes_in = getline( &line, &len, fd );
	ptr = strchr(line, ' ');
	ServoInfo[3].max_angle = atoi( ptr+1 );
	ServoInfo[3].max_value = atoi( ptr+1 );
		
	 bytes_in = getline( &line, &len, fd );
	ptr = strchr(line, ' ');
	ServoInfo[4].max_angle = atoi( ptr+1 );
	ServoInfo[4].max_value = atoi( ptr+1 );
}

void file_read()
{
	file_open();
	read_origin();
	read_arm_sizes();
	read_zero_points();
	read_min_points();
	read_max_points();
	fclose (fd);
						
}


void subtract_origin( float* mX, float* mY, float* mZ )
{
	*mX -= ArmOrigin.X;
	*mY -= ArmOrigin.Y;
	*mZ -= ArmOrigin.Z;
}
void add_origin( float* mX, float* mY, float* mZ )
{
	*mX += ArmOrigin.X;
	*mY += ArmOrigin.Y;
	*mZ += ArmOrigin.Z;
}
