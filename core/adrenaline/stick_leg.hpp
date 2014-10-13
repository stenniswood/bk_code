#ifndef _STICKLEG_H_
#define _STICKLEG_H_


#include "control.hpp"
#include "motor_pack.hpp"


struct sRect {
	float left;
	float right;
	float top;
	float bottom;
};

struct sRect_wh {
	float left;
	float bottom;
	float width;
	float height;
};

/* Abstract Base Class for all graphs */
class StickLeg : public Control
{
public:
	StickLeg( int Left, int Right, int Top, int Bottom );
	StickLeg();

	void 	print_ab(char* text) {
	 	//printf("================%s: width=%6.1f  height=%6.1f \n", text, AbdominBox.width, AbdominBox.height  );
	  };
	  
	virtual int	  draw(	);		// Must implement in derived class! Contains the graph body.

	void	set_motor_pack( MotorPack* mmp)  { mp = mmp; 		   };
	void 	set_leg_color( long int mColor)  { Leg_color = mColor; };
	void	set_ab   ( float angle )		 {	AbAngle   = angle; };

protected:

private:
	MotorPack*  mp;
	long  Leg_color;
	float AbAngle;
/*	float HipAngle;
	float KneeAngle;
	float AnkleAngle; */
	struct sRect_wh AbdominBox;	
};

 
#endif
