#ifndef _AUDIO_PROTOCOL_H_
#define _AUDIO_PROTOCOL_H_

#include "nlp_sentence.hpp"
#include "server_handler.hpp"
#include "serverthread.hpp"

/* Note - Sometimes the Raspberry Pi doesn't play over HDMI anyway.
		  and doing "raspi-config" doesn't help.
		  Answer is to uncomment the line in "/boot/config.txt" which says
		  hdmi_drive=2.  Then reboot.  This solved it.
*/

extern BOOL  AUDIO_tcpip_ListeningOn;
extern BOOL  AUDIO_tcpip_SendingOn;
extern BOOL  AUDIO_tcpip_SendingMuted;			// we send zerod out audio
extern BOOL  AUDIO_tcpip_ListeningSilenced;		// we do not play any incoming audio.
extern BOOL  AUDIO_save_requested;

extern FILE* sending_audio_file_fd;


/* Action Initiators */
void send_audio_file( char* mFilename );
void send_audio		( ServerHandler* mh);
//void audio_listen	(BOOL Save=FALSE);
void audio_listen	( ServerHandler* mh);     // c doesn't support audio_listen

void audio_two_way	( ServerHandler* mh);
void audio_cancel	( ServerHandler* mh);


void 	DumpBuffer( BYTE* mbuff, int mlength);
void 	Init_Audio_Protocol();

int 	Parse_Audio_Statement( Sentence& msentence, ServerHandler* mh );
//int 	Parse_Audio_Statement( const char* mSentence, ServerHandler* mh );



#endif
