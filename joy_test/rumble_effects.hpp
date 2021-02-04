#ifndef _RUMBLE_EFFECTS_HPP_
#define _RUMBLE_EFFECTS_HPP_


#include "ps4_hid_outputs.hpp"


struct stEffect {
	std::vector<byte> strongs;
	std::vector<byte> weaks;
	int	ms_delay;
};


void init_rumble_effects(  );

int  add_effect	    (  struct stEffect& mEnvelope );
void select_effect	(  int mEffectIndex 		  );

void start_effects  ( 					);
void stop_effects   ( 					);




#endif
