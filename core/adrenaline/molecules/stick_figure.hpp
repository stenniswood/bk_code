#ifndef _STICKLEG_H_
#define _STICKLEG_H_

#include "control.hpp"
#include "robot.hpp"

/* We want to make this work without the Robot or MotorPack classes
   because they depend on hardware presence.  
   
   Want to be able to do the visualization without hardware.  GUI separate 
   from hardware.
*/

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

/* This draws a stick figure of a humanoid robot 
	It is a 2 dimensional side view of the robot.
*/

class StickFigure : public Control
{
public:
	StickFigure( int Left, int Right, int Top, int Bottom );
	StickFigure();

	void 	print_ab(char* text) {
	 	//printf("================%s: width=%6.1f  height=%6.1f \n", text, AbdominBox.width, AbdominBox.height  );
	};

	void	init();
	float 	get_leg_length();
	float 	get_arm_length();
	
	void 	draw_arm(int mStartx, int mStarty, byte mIndex);
	void 	draw_leg(int mStartx, int mStarty, byte mIndex);
	void 	draw_chest(float x, float y, float half_chest_width, float half_chest_height);
  
	virtual int	  draw(	);		// Must implement in derived class! Contains the graph body.

//	void	set_robot( Robot* mRobot)  		 				{ robot = mRobot; 	   };
	void 	set_limb_color ( byte mIndex, long int mColor) 	{ Limb_color[mIndex] = mColor; };
	void	set_chest_angle( float angle )	 				{	AbAngle   = angle; };

protected:

private:
	//Robot*  robot;
	long	Limb_color[4];

	long	Chest_color;
	int		Chest_width;
	int		Chest_height;

	float	AbAngle;
};

 
#endif
