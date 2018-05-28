# TiltPan Camera - PiCAMScan Board

This app is intended to work with Pi Cam Scanner from Beyond Kinetics, Inc.

## The Beyond Kinetics PiCAMScan Feature set:
* 2 Servos  (PWM signals for Pan and Tilt)
* 8 Analog Inputs
* 10 Lowside drivers
* GPS connector
* CAN transceiver

All inputs/outputs will be accessible via abkInstant from a Android smart phone (cell phone or other bkInstant device.

Since each RC servo is slightly different specs, the data is put into the config.ini file

Min Pulse width
Max Pulse width
Min Angle
Max Angle

etc.

The communication with the cell phone happens thru the bkInstant application.  So this must
also be started on the RPI.  It uses interprocess communications interact with this app.


*********************************************************************************************

