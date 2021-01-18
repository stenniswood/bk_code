import time
import serial

# configure the serial connections (the parameters differs on the device you are connecting to)
ser = serial.Serial(
    port='/dev/ttyACM0',
    baudrate=9600,
    parity=serial.PARITY_NONE,
    stopbits=serial.STOPBITS_ONE,
    bytesize=serial.EIGHTBITS
)


ser.isOpen()

ser.write( 'servo 1 120\r\n' )
time.sleep(1)
while ser.inWaiting() > 0:
	out += ser.read(1)

ser.close()



