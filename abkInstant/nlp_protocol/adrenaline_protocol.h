#ifndef _ADRENALINE_PROTOCOL_H_
#define _ADRENALINE_PROTOCOL_H_

/**
	Purpose of this:	access adrenaline 2D window objects via nlp.
			Place a list box with all blah in it.
			Bring up the gyro/accelerometer.
			show a waveform,
			etc.
	This will use the sql_global_knowledge_name database and thus 
		user specific items will not be handled here!
		
	Very priliminary work, not even close to completed.
	
	There is also the 3D object manipulation (see ThreeD_object_protocol.cpp)
**/

void Init_Adrenaline_Protocol( );

int Parse_adrenaline_statement( Sentence& mSentence, ServerHandler* mh );



#endif
