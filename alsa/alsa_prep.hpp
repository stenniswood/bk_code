#ifndef _ALSA_PREP_HPP_
#define _ALSA_PREP_HPP_



snd_pcm_t* prepare_alsa( int m_number_channels, unsigned int msamples_per_second, 
						char** buff, snd_pcm_uframes_t* frames );



#endif
