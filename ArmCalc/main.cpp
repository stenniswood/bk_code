/*********************************************************************
 *											   						 *
 *  DESCRIPTION: Main Test Bench Phase A Controller			 	 	 *
 *																	 *
 *	CATEGORY:   Application 									 	 *
 *											   						 *
 *  INTERFACE (Schematic Name:  GraphicLCD_back.sch):			 	 *
 *			  (Hardware Rev 1.0 - Boards Ordered July 31, 2008)		 *	
 *	=====================================================			 * *											   						 *
 *  AUTHOR	: Steve Tenniswood, Applications Engineering 	 		 *
 *	DATE	: August 2008						 				 	 *
 *********************************************************************/
#include <stdio.h>
#include <string.h>


#include "APP_InverseCoordinates.h"
#include "APP_CoordinateTransform.h"
#include "APP_Calibrations.h"
#include "drive_five.h"

DriveFive mots("/dev/ttyUSB0");


char result[50];


void help()
{
	printf("6 DOF Revolute Coordinate Arm Calculator.\n");
	printf(" BRH (Base angle, Radius, Height) coordinates are Cylindrical coordinates.\n");
	printf(" Units for XYZ are inches - (calculations will be based on real arm measurements).\n\n");

	printf("Upper Arm length=%6.3f\n", LENGTH_SHOULDER );
	printf("Fore Arm length =%6.3f\n", LENGTH_ELBOW );
	printf("Wrist length    =%6.3f\n", LENGTH_WRIST );
	printf("Base height     =%6.3f\n\n", BASE_HEIGHT );
	printf("Total Extension =%6.3f\n\n", LENGTH_SHOULDER + LENGTH_ELBOW + LENGTH_WRIST );
		
	printf("-xb 	Convert XYZ coordinate to BRH. \n");
	printf("-bx 	Convert BRH coordinate to XYZ. \n");

	printf("-xa 	Convert XYZ coordinate to servo angles [s1..s4]. \n");
	printf("-ax 	Convert servo angles to XYZ coordinate. \n\n");
}

#include <stdlib.h>

void process_args(int argc, char** argv, float* P1, float* P2, float* P3)
{
	if (argc>=4) {
		*P1 = atof(argv[2]);
		*P2 = atof(argv[3]);
		*P3 = atof(argv[4]);
	}
	printf("Input Parameters: %6.3f, %6.3f, %6.3f\n", *P1, *P2, *P3);
	
}

void init_drive_five()
{
	char msg[] = "use encoder v w x z ";
	mots.send_command(msg);
	mots.read_response();


}
void send_motor_positions(uint16_t mXpos, uint16_t mYpos, uint16_t mZpos )
{
	char msg[80];
	sprintf(msg, "POS x%d", mXpos );	
	mots.send_command(msg);
	mots.read_response();

	sprintf(msg, "POS y%d", mYpos );
	mots.send_command(msg);
	mots.read_response();

	sprintf(msg, "POS z%d", mZpos );
	mots.send_command(msg);
	mots.read_response();	
}

/*****************************************************************************
Name:    main()       
Parameters: none                     
Returns:  nothing      
Description: initializes variables, then goes into an infinite loop. A 
             simple delay loop is used to "slowly" increase the pulse widths
**************************************************************************** */
int main (int argc, char* *argv)
{ 
	uint8_t i=0;
	float X,Y,Z;
	float B,R,H;
	float P1,P2,P3,P4;
	struct stBRH_Coordinate BRH_result;
	struct stXYZ_Coordinate XYZ_result;
	struct stServoAnglesSet mSAResult;
	
	struct stFull_XYZ_Coordinate XYZ;		// Includes approach & wrist rotate angles.
	
	file_open();
	file_read();
	//exit(1);
	
	init_drive_five();
	
	
	if (argc)
	{
		int match = strcmp(argv[1],"help");
		if (match==0)
			help();

		process_args(argc, argv, &P1, &P2, &P3 );
		match = strcmp(argv[1],"-xb");
		if (match==0)
		{
			X = P1; 	Y=P2;	Z=P3;
			subtract_origin( &X,&Y,&Z );
			INV_Transform_XYZ_to_BRH ( X, Y, Z, &BRH_result );
			printf("Base,Radius,Height= < %6.3f, %6.3f, %6.3f >\n", BRH_result.B, BRH_result.R, BRH_result.H );
			printf("(radians,linear unit of input, linear unit of input)\n");
		}

		match = strcmp(argv[1],"-bx");
		if (match==0)
		{
			B=P1; 	R=P2;	H=P3;
			INV_Transform_BRH_to_XYZ( B, R, H, &XYZ_result );
			printf("X,Y,Z= < %6.3f, %6.3f, %6.3f >\n", XYZ_result.X, XYZ_result.Y, XYZ_result.Z );
		}

		match = strcmp(argv[1],"-xa");
		if (match==0)
		{
			XYZ.X=P1;		XYZ.Y=P2;		XYZ.Z=P3;
			subtract_origin( &(XYZ.X), &(XYZ.Y), &(XYZ.Z) );
			INV_XYZ_To_Angles( &XYZ, &mSAResult );
			printf("Servo Angles (in degrees):\n");
			printf("  s1= %6.3f\n", CT_Convert_Radians_to_Degrees(mSAResult.s1) );
			printf("  s2= %6.3f\n", CT_Convert_Radians_to_Degrees(mSAResult.s2) );
			printf("  s3= %6.3f\n", CT_Convert_Radians_to_Degrees(mSAResult.s3) );
			printf("  s4= %6.3f\n", CT_Convert_Radians_to_Degrees(mSAResult.s4) );
			printf("  s5= %6.3f\n", CT_Convert_Radians_to_Degrees(mSAResult.s5) );
		}
	
		match = strcmp(argv[8],"-ax");
		if (match==0)
		{
			P4 = atof(argv[5]);
			mSAResult.s1=P1;		mSAResult.s2=P2;		mSAResult.s3=P3;    mSAResult.s3=P4;
			
			CT_CalculateForwardEquations( &mSAResult, &X, &Y, &Z );			
			add_origin( &X,&Y,&Z );
			printf("XYZ Coordinates :\n");
			printf("  X= %6.3f\n", X );
			printf("  Y= %6.3f\n", Y );
			printf("  Z= %6.3f\n", Z );
		}
	}
}


