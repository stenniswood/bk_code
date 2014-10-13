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
//#include <functional>
#include <string>
//#include "udp_transponder.h"
#include "serverthread.h"
#include "GENERAL_protocol.h"
#include "CAMERA_protocol.h"



void init_nlp_pwm()
{
}

void Parse_pwm_Statement( char* mSentence, sObject* mSubject, 
		 					 std::string* mVerb, sObject* mObject )
{
}



