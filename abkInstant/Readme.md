====================================================================
## abkInstant
### A server for Adrenaline networks.
#### Product of Beyond Kinetics, Inc.
====================================================================

This app transfer Audio, Video, CAN, text, messages, HMI, etc.

#### How to Start:
	cd ~/bk_code/abkInstant
	s ./instant & 

Start the server on several computers at once.
====================================================================
Then what?
A) Connect to another instant :
		s ./instant 192.168.1.101

		aclient [list,host='', ip=''] [open audio,video,CAN,HMI,etc]

B) Start avisual (make sure instant is running already)
C) Start PiCAM Scan board app ( servo motor commands will move )
D) Start Segway app
Other apps may attach to this.
====================================================================
INTERNALS:

How it works:
	The server starts 3 threads:  UDP beacon and UPD Receive.
	server_thread which waits for a connection from a client.

NOTE:  instant allocates memory for shared apps.


Command Server Socket
Command Client Socket
Data Socket

    Contact GitHub API Training Shop Blog About 

How does the Instant server work?

	Two Different ways of connecting to Instant:
		a) Client app (uses cli shared memory)
		b) TCP/IP client connect 
