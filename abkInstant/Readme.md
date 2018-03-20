====================================================================
## abkInstant
### A server for Adrenaline networks.
#### Product of Beyond Kinetics, Inc.
====================================================================

This app transfers Audio, Video, CAN, text, messages, HMI, etc.

#### How to Start:
Start the server on one or more computers at once.
	cd ~/bk_code/abkInstant
	s ./instant & 
	
#### Then what?
A) From another computer, create a connection to the server :
		s ./instant 192.168.1.101
B) There are several "apps" that can now use this connection.
	1. A simple command line client:
		./aclient [list,host='', ip=''] [open audio,video,CAN,HMI,etc]
	2. "Avisual" - A graphical user interface package
		./avisual 	
	3. For remote control of the PiCAM board ( a PCB board which provide 2 servos, 
		8 analog inputs, 16 lowside drivers, etc)
		Start PiCAM Scan board app ( servo motor commands will move )
		./camscan blah blah
	4. Start Segway app
	5. Other apps may attach to this.
	
====================================================================
## INTERNALS:

### How it works:
	The server starts 3 threads:  UDP beacon and UPD Receive.
	server_thread which waits for a connection from a client.

NOTE:  instant allocates memory for shared apps.


Command Server Socket
Command Client Socket
Data Socket

    Contact GitHub API Training Shop Blog About 

## How does the Instant server work?

	Two Different ways of connecting to Instant:
		a) Client app (uses cli shared memory)
		b) TCP/IP client connect 
