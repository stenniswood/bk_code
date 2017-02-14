
This app will read from a connected PS3 controller via USB.
It uses the Linux Joystick API.

The left and right Analog positions are stored into an array.
These are converted into a motor speed and the main thread sends them to 
one of the RoboClaw boards.

Press LEFT TOP and move the left pod to control /dev/ttyACM1 board
Press RIGHT TOP and move the right pod to control /dev/ttyACM1 board

Move the left pod to control /dev/ttyACM0 board
Move the right pod to control /dev/ttyACM0 board

Press LEFT BOTTOM and move the left pod to control /dev/ttyACM2 board
Press RIGHT BOTTOM and move the right pod to control /dev/ttyACM2 board



