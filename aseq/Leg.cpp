

#include "joint.hpp"

/* 82666 Parameters */
#define mu_static_82666	 	 1.0	// 
#define mu_kinetic_82666 	 1.0	//
#define RatedTorque_82666	 1536	// ounce*inch
#define Stall_current_82666  19.0	// amps
#define Noload_current_82666 1.5	// amps
#define Max_speed_rps_82666  1.5833	// rps

/* 815xxx Parameters */
#define mu_static_71646		 1.0
#define mu_kinetic_71646 	 1.0
#define RatedTorque_71646 	 1066   // lbs*inch
#define Stall_current_71646  19.0	// amps
#define Noload_current_71646 1.5	// amps
#define Max_speed_rps_71646  1.5833	// rps


Leg::Leg()
{
	/* Consists of XX Pivots.
		A) Hip & upper leg
		B) Knee & lower leg
		C) Ankle & foot		
		Add hip rotation later!		
	*/
	// Construct Hip Rotate Pivot:
	/*Joint*   tmpJ = new Joint	( mu_static_82666, mu_kinetic_82666 	);
	Segment* tmpS = new Segment	( 8.0, 12.0, 7 							);	
	Motor*   tmpM = new Motor   ( RatedTorque_82666, Stall_current_82666);
	HipRotate	  = new Pivot	( tmpJ, tmpS, tmpM 						); */
	
	// Construct Hip Pivot:
	Joint*   tmpJ = new Joint	( mu_static_82666, mu_kinetic_82666 	);
	Segment* tmpS = new Segment	( 8.0, 12.0, 7 							);	
	Motor*   tmpM = new Motor   ( RatedTorque_82666, Stall_current_82666);
	Hip  = new Pivot			( tmpJ, tmpS, tmpM 						);
	
	// Construct Knee Pivot:
	tmpJ = new Joint	 ( mu_static_82666, mu_kinetic_82666 );
	tmpS = new Segment	 ( 8.0, 12.0, 7 					 );	
	tmpM = new Motor	 ( RatedTorque_82666, Stall_current_82666 );
	Knee = new Pivot	 ( tmpJ, tmpS, tmpM 				);

	// Construct Ankle Pivot:
	tmpJ = new Joint	( mu_static_71646, mu_kinetic_71646 );
	tmpS = new Segment	( 8.0, 12.0, 7 );	
	tmpM = new Motor	( RatedTorque_71646, Stall_current_71646 );
	Knee = new Pivot	( tmpJ, tmpS, tmpM );
}



