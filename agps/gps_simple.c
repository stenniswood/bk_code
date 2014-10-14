/*
 * serialTest.c:
 *	Very simple program to test the serial port. Expects
 *	the port to be looped back to itself
 *
 */
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <wiringPi.h>
#include <wiringSerial.h>

#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>



#define TXD_PIN 15
#define RXD_PIN 16


int main ()
{
  int fd ;
  int count ;
  unsigned int nextTime ;


  if ((fd = serialOpen ("/dev/ttyAMA0", 9600)) < 0)
  {
    fprintf (stderr, "Unable to open serial device: %s\n", strerror (errno)) ;
    return 1 ;
  }
  printf("fd=%d", fd);

  if (wiringPiSetup () == -1)
  {
    fprintf (stdout, "Unable to start wiringPi: %s\n", strerror (errno)) ;
    return 1 ;
  }
  
  // THIS LIKELY IS ALREADY DONE IN serialOpen()
  pinMode( RXD_PIN, INPUT  );
  pinMode( TXD_PIN, OUTPUT );

  // We need to set the 2515 controller chip for RTS inputs
	// no they are either digital inputs or Ready-To-Send
	
  nextTime = millis () + 300 ;

  for (count = 0 ; count < 256 ; )
  {
    if (millis () > nextTime)
    {
      printf ("\nOut: %3d: ", count) ;
      fflush (stdout) ;
      serialPutchar (fd, count);
      serialFlush(fd) ;
      nextTime += 300 ;
      ++count ;
    }
    delay (3);

	int bytesAv=0;
	while (!bytesAv && ioctl(fd,FIONREAD,&bytesAv)>=0)
	    delay(10); 

   // while (serialDataAvail(fd))
    {
      printf (" -> %3d", serialGetchar (fd)) ;
      fflush (stdout) ;
    }

  }

  printf ("\n") ;
  return 0 ;
}