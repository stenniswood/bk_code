#ifndef _CALIBRATIONS_HPP_
#define _CALIBRATIONS_HPP_

#include <string>
#include <string.h>


/********* Abstract Base Class Only!  **********
  
   Actual implementations are in rcServo and dfServo ! 

Strategy for calibration:
	We set the end points (keyboard presses)
	Then set the center point (keyboard press)
	
	Zero will tentatively be called "0.00" but is not neccessarily.
			There may be a time when the center is not zero.
			The ankle for example - 
			
	But the counts per degree is the same for all servos right now.
	We are going to keep it simple!
			So center will always be zero.
			
************************************************/
   
class Servo
{
public:
	Servo();
	~Servo();

	//void    load_cal_info();
	virtual void	setup_hawrdware()	{ };	
	void 			parse_cal_line  ( std::string mStr );
	virtual void 	print_parameters( );
	
	unsigned short  convert_deg_to_counts ( float mDegrees );
	float 		    convert_counts_to_degs( float mCounts  );

	virtual unsigned short	get_counts( 				) = 0;	
	virtual float	get_angle (  						) = 0;	
	virtual	void    angle_to  ( float mDegree 		 	) = 0;
	virtual	void    count_to  ( unsigned short mCounts 	) = 0;
	virtual	void    to_center ( ) {  count_to( cal_center ); };

	virtual void    load_current_position( std::ifstream& fd ) = 0;
	virtual void    save_current_position( FILE* fd ) = 0;	

	virtual void   save_cal_file_line( FILE* fd );
	void		set_cal_min   ( unsigned short mMinCounts )		;
	void		set_cal_max   ( unsigned short mMaxCounts )		;
	void		set_cal_center( unsigned short mCenterCounts )	;
	void		set_deg_min   ( float mMinDegs   		)		{ Deg_min    = mMinDegs;	 	};
	void		set_deg_max   ( float mMaxDegs   		)		{ Deg_max    = mMaxDegs;	 	};
	void		set_deg_center( float mCenterDegs   	)		{ Deg_center = mCenterDegs;  	};

	unsigned short get_stop_counts( int mStopNum );

	unsigned short get_cal_min() { return cal_min; };
	unsigned short get_cal_max() { return cal_max; };
	unsigned short get_cal_center() { return cal_center; };
	

private:
	unsigned short cal_min;			// Counts
	unsigned short cal_max;			// Counts
	unsigned short cal_center;		// Zero Offset Counts

	float		Deg_min;			// Corresponding degrees.
	float		Deg_max;
	float		Deg_center;

};


extern Servo* servs[16];
extern int    servs_used;

int read_cal_file( std::string mFilename );
void print_calfile_info();



#endif

