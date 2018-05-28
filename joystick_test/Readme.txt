
This app will read from a connected PS3 controller via USB.
It uses the Linux Joystick API.

The left and right Analog positions are stored into an array.
These are converted into a motor speed and the main thread sends them to 
one of the RoboClaw boards.

===== Compiled for Roboclaw ==========
Move the left pod to control /dev/ttyACM0 board
Move the right pod to control /dev/ttyACM0 board

Press and hold L1 (LEFT Top button) and move the left pod to control /dev/ttyACM1 board
Press and hold R1 (RIGHT Top button) and move the right pod to control /dev/ttyACM1 board

Press LEFT BOTTOM and move the left pod to control /dev/ttyACM2 board
Press RIGHT BOTTOM and move the right pod to control /dev/ttyACM2 board



=========== Compiled for DriveFive board (beyond kinetics) ===========
For all, only up and down of the pod does anything.  (Left-Right no action)

Move left pod to control motor V
Move right pod to control motor W

Press & Hold L1 - Move left pod to control motor X
Press & Hold R1 - Move right pod to control motor Y

Press & Hold L2 - Move left pod to control motor Z


================== Tank Mode =========================
Controls same as World of Tanks on PS4.

Left Pod:
	UD - forward backward motion
	LR - Sets speed ratio of Left and Right treads

RIGHT Pod : 
	LR - Turns the Tank top and Turret.
	UD - Lifts or lowers the Turret.
	
Press X to fire!
========================================================