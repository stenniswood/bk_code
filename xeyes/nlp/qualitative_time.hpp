//
//  qualitative_time.hpp
//  abkInstant
//
//  Created by Stephen Tenniswood on 9/6/16.
//  Copyright © 2016 Stephen Tenniswood. All rights reserved.
//

#ifndef qualitative_time_hpp
#define qualitative_time_hpp

#include <stdio.h>
#include <string>
#include <algorithm>
#include "nlp_sentence.hpp"

using namespace std;

extern bool parsed_qualitative_time;		// True if any qualitative time is stated : 
extern bool parsed_qualitative_duration;	// True if any qualitative time is stated : 

//void convert_to_time_t( m_row[1], &tmp );
void compose_time_qualitative		( time_t mTime, std::string& mTimeString );
void compose_time_period_qualitative( struct tm* sTime, struct tm* eTime, std::string& mTimeString );

int  parse_qualitative_1_time( Sentence& mSentence, time_t mTime   );
int  parse_qualitative_2_time( Sentence& mSentence, struct tm& sTime, struct tm& eTime );


#endif /* qualitative_time_hpp */
