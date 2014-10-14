#ifndef _ProgressBar_
#define _ProgressBar_

#include "control.hpp"


class ProgressBar : public Control 
{
public:
	ProgressBar(										  );
	ProgressBar( int Left, int Right, int Top, int Bottom );

	void	setPercentage	( float Percentage )  
	{ 
			printf("Progress::setPercentage();\n");
			fraction = Percentage/100.0; 
	};	
	void	setMaxValue		( float Max		);
	void	setCurrentValue	( float current );

	virtual int   	draw 	(	);
	//virtual int	onClick();
	
private:
	float fraction;			// 1.0 is 100%
	
	// Alternate to percent
	float Max;
	float Current;
};

 
#endif
