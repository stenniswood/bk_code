#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "bk_system_defs.h"
#include "CAN_base.h"

#include "can_eid.h"
#include "can_id_list.h"


//=================== CHEVY KNOWN CODES: =======================
void code_description( tID mID, char* mDescription )
{
	switch (mID.group.id)
	{
	case 0x0C9:	strcpy(mDescription, "Accelerator   ");		break;
	case 0x0F1:	strcpy(mDescription, "Brake         ");		break;
		
	case 0x135:	strcpy(mDescription, "Drive Position");		break;		
	case 0x1A1:	strcpy(mDescription, "Accelerator   ");		break;
	case 0x1C3:	strcpy(mDescription, "Accelerator   ");		break;
	case 0x1EF:	strcpy(mDescription, "Gas Engine RPM");		break;
	case 0x1F5:	strcpy(mDescription, "Shift Position");		break;
	
	case 0x206:	strcpy(mDescription, "Battery SOC   ");		break;
	case 0x32A:	strcpy(mDescription, "GPS Latitude  ");		break;
	case 0x3E9:	strcpy(mDescription, "Speed         ");		break;
	
	default:  *mDescription=0; 					break;	
	}
}

float parse_0C9( struct sCAN* mMsg )
{
	float percent = 100.*(mMsg->data[5])/254.;
	printf("percent = %5.4f \t\t", percent );
	return percent; 
}
float parse_0F1( struct sCAN* mMsg )
{
	float percent = 100.*(mMsg->data[2])/254.;
	printf("percent = %5.4f \t\t", percent );
	return percent; 
}

void parse_135( struct sCAN* mMsg )
{
	switch(mMsg->data[5])
	{
	case 0:  printf(" Park    \t\t\t\t"); break;
	case 1:  printf(" Neutral \t\t\t\t"); break;
	case 2:  printf(" Drive/L \t\t\t\t"); break;
	case 3:  printf(" Reverse \t\t\t\t"); break;
	default: printf(" \t\t\t\t"); break;
	}
}
float parse_1A1( struct sCAN* mMsg )
{
	float percent = 100.*(mMsg->data[8])/254.;
	printf("percent = %5.4f \t\t", percent );
	return percent; 
}
float parse_1C3( struct sCAN* mMsg )
{
	float percent = 100.*(mMsg->data[8])/254.;
	printf("percent = %5.4f \t\t", percent );
	return percent; 
}
void parse_1EF( struct sCAN* mMsg )
{
	word rpm = ((mMsg->data[3]<<8) | mMsg->data[4]);
	printf("rpm = %6d\t\t\t", rpm);

}
void parse_1F5( struct sCAN* mMsg )
{
	switch(mMsg->data[5])
	{
	case 0:  printf(" Park    \t\t\t"); break;
	case 1:  printf(" Neutral \t\t\t"); break;
	case 2:  printf(" Drive/L \t\t\t"); break;
	case 3:  printf(" Reverse \t\t\t"); break;
	default: break;
	}
}

void parse_206( struct sCAN* mMsg )
{
	word soc = ((mMsg->data[1]<<8) | mMsg->data[2]);
	printf("Battery SOC = %5d\t\t", soc);
}

void parse_32A( struct sCAN* mMsg )
{
	long int  lat = ((mMsg->data[1]<<24) | (mMsg->data[2]<<16) |
					 (mMsg->data[3]<< 8) | (mMsg->data[4]));
	long int  lon = ((mMsg->data[5]<<24) | (mMsg->data[6]<<16) |
					 (mMsg->data[7]<< 8) | (mMsg->data[8]));
	printf("Latitude=%10d;  Longitude=%10d\t\t", lat, lon);
}
void parse_3E9( struct sCAN* mMsg )
{
	float speed = ((mMsg->data[1]<<8)|mMsg->data[2])/100.;
	printf("Speed = %6.3f\t\t\t", speed);
}

int is_powertrain_msg( struct sCAN* mMsg )
{
	switch (mMsg->id.group.id)
	{
case 0x096 : return 1;	case 0x097 : return 1;	case 0x098 : return 1;	case 0x0AA : return 1;
case 0x0B1 : return 1;	case 0x0B9 : return 1;	case 0x0BA : return 1;  case 0x0BB : return 1;
case 0x0BC : return 1;  case 0x0BD : return 1;	case 0x0C1  : return 1; case 0x0C5  : return 1;
case 0x0C7  : return 1; case 0x0C9  : return 1;	case 0x0D1  : return 1; case 0x0D3  : return 1;
case 0x0F1  : return 1; case 0x120  : return 1;	case 0x12A  : return 1; case 0x130  : return 1;
case 0x135  : return 1; case 0x137  : return 1;	case 0x139  : return 1; case 0x140  : return 1;
case 0x148  : return 1; case 0x160  : return 1;	case 0x17D  : return 1; case 0x182  : return 1;
case 0x185  : return 1; case 0x186  : return 1;	case 0x1A1  : return 1; case 0x1A3  : return 1;
case 0x1C3  : return 1; case 0x1C4  : return 1;	case 0x1C5  : return 1; case 0x1C6  : return 1;
case 0x1DF  : return 1; case 0x1E1  : return 1;	case 0x1E5  : return 1; case 0x1E9  : return 1;
case 0x1EB  : return 1; case 0x1ED  : return 1;	case 0x1EF  : return 1; case 0x1F1  : return 1;
case 0x1F3  : return 1; case 0x1F4  : return 1;	case 0x1F5  : return 1; case 0x1FB  : return 1;

case 0x200 : return 1;	case 0x202 : return 1;	case 0x204 : return 1;	case 0x206 : return 1;
case 0x214 : return 1;	case 0x222 : return 1;	case 0x224 : return 1;	case 0x226 : return 1;
case 0x228 : return 1;	case 0x230 : return 1;	case 0x236 : return 1;	case 0x238 : return 1;
case 0x287 : return 1;	case 0x2C3 : return 1;	case 0x2C7 : return 1;	case 0x2F1 : return 1;
case 0x2F9 : return 1;  case 0x32A : return 1;	case 0x348 : return 1;  case 0x34A : return 1;	
case 0x3C1 : return 1;  case 0x3C9 : return 1;	
case 0x3CB : return 1;  case 0x3D1 : return 1;	case 0x3DC : return 1;	case 0x3DD : return 1;  
case 0x3E3 : return 1;  case 0x3E9 : return 1;  case 0x3ED : return 1;  case 0x3F1 : return 1;  
case 0x3F9 : return 1;
/*case 0x3FB : return 1;  case 0x3FC 
case 0x451 case 0x4C1 case 0x4C5 case 0x4C7 case 0x4C9 case 0x4CB case 0x4D1   case 0x4D7 case 0x4D9 case 0x4E1 case 0x4E9 case 0x4F1 case 0x500 case 0x514 case 0x52A
case 0x530 case 0x589 case 0x770 case 0x772 case 0x773 case 0x778 case 0x77D case 0x77F   case 0x787 
*/	
default : return 0; break;
	}
}

byte parse_chevy_id( struct sCAN* mMsg ) 
{
	//printf( "parse_chevy %4X\n", mMsg->id.group.id );
	char name[50];
	code_description( mMsg->id, name );
	if (*name==0) return 0;
	printf("%s :", name);	
		
	switch( mMsg->id.group.id )
	{
	case 0x0C9: parse_0C9 (mMsg);	break;
	case 0x0F1: parse_0F1 (mMsg);	break;
	
	case 0x135: parse_135 (mMsg);	break;
	case 0x1A1: parse_1A1 (mMsg);	break;	
	case 0x1C3: parse_1C3 (mMsg);	break;
	case 0x1EF: parse_1EF (mMsg);	break;
	case 0x1F5: parse_1F5 (mMsg);	break;	
	
	case 0x206: parse_206 (mMsg);	break;
	case 0x32A: parse_32A (mMsg);	break;
	case 0x3E9: parse_3E9 (mMsg);	break;
	default:  return 0; break;
	}
	return 1;	
}