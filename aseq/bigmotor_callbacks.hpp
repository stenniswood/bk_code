#ifndef _BIGMOTOR_CALLBACKS_
#define _BIGMOTOR_CALLBACKS_


extern bool FirstIssued;

BOOL can_position_test_responder	   ( struct sCAN* mMsg 	  );  // moves motors
BOOL can_position_meas_responder	   ( struct sCAN* mMsg 	  );  

BOOL can_motor_position_responder	   ( struct sCAN* mMsg 	  );
BOOL can_vector_read_complete_responder( byte mAppendageIndex );




#endif
