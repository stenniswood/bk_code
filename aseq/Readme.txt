******************* SEQUENCER ********************
This directory is a sequencer for motor commands.
Intended for the Raspberry Pi platform.

Intention is for a simple programmatic walking sequence.

Movements will be defined in a /moves/*.csv file.  Unlimited files can be stored there.
The command line argument names the file followed by the number of repetitions.
	sudo ./seq simple_walk.csv 10
	
Will name which sequence to playback

This app will contain the CAN code.

