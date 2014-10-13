#include <iostream>
#include <list>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <time.h> 
#include "protocol.h"
#include "devices.h"
#include "package_commands.h"
#include <termios.h>
#include <unistd.h>
#include <pthread.h>

#include <string>
//#include "udp_transponder.h"
#include "serverthread.h"
#include "GENERAL_protocol.h"
#include "CAMERA_protocol.h"

#include "Adren_analog_talk.h"
#include "Adren_bigmotor_talk.h"
#include "Adren_button_talk.h"
#include "Adren_lcd_talk.h"
#include "Adren_picamscan_talk.h"
#include "Adren_pwm_talk.h"
#include "Adren_tiltsensor_talk.h"



void init_nlp_adrenaline()
{

}
void Parse_adrenaline_Statement( char* mSentence, sObject* mSubject, 
		 					 std::string* mVerb, sObject* mObject )
{

}		 					 
		 					 




