#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include <math.h>
#include <poll.h>
#include <linux/serial.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <ctime>
#include "global.h"

#include "sserial.hpp"
#include "load_cell.hpp"

#define min(a, b) (((a) < (b)) ? (a) : (b))
#define max(a, b) (((a) > (b)) ? (a) : (b))


SSerial loadcell_left;
SSerial loadcell_right;


struct stOneFoot LeftSensors;
struct stOneFoot RightSensors;
struct stOneFoot LeftNominals;		// When properly balanced.
struct stOneFoot RightNominals;

float TooForwardRatio = 3333/4444;	// Threshold hold for detecting leaning forward (just starting)
float TooBackwardRatio = 3333/4444;	// Threshold hold for detecting falling backward (just starting)



/*		With 10+ lbs
+7.5	17.5
-7.5	2.5

Excess weight can be found then by:
	17.5-2.5  = 15.0
	half_diff = 15.0 / 2 = 7.5		The difference divide by 2
	
	The larger 17.5 - half_diff = 10.
*/
float calc_excess_weight( float mLarger, float mSmaller )
{
	float half_diff = (mLarger - mSmaller) / 2.0;
	float added_weight = mLarger - half_diff;
	return added_weight;
}

float calc_forward_pressure( float mFront, float mBack )
{
	float maxp = max(mFront, mBack);
	float minp = min(mFront, mBack);
	float added_weight = calc_excess_weight( maxp,minp );

	float  excess = (mFront - added_weight);
	return excess;
}

float calc_backward_pressure( float mFront, float mBack )
{
	float maxp = max(mFront, mBack);
	float minp = min(mFront, mBack);
	float added_weight = calc_excess_weight( maxp,minp );
	
	float  excess = (mBack - added_weight);
	return excess;
}

bool is_balanced( float mRatio)
{
	if ((mRatio<TooForwardRatio) && (mRatio > TooBackwardRatio))
		return true;
	return false;
}


void setup_load_cell( const char* mDeviceNameLeft, const char* mDeviceNameRight)
{
	loadcell_left.open ( mDeviceNameLeft  );
	loadcell_right.open( mDeviceNameRight );

	char msg[] = "stop\r\n";
	loadcell_left.writeStr ( msg );
	loadcell_right.writeStr( msg );
	
	// EMPTY RECEIVE BUFFERS:
	char flushB[2000];
	int rx_bytes = 0;
	while( loadcell_left.available() )	
		rx_bytes = ::read(loadcell_left.fd, flushB, 100);	// if this blocks, we are okay.	
	while( loadcell_right.available() )	
		rx_bytes = ::read(loadcell_right.fd, flushB, 100);	// if this blocks, we are okay.	
	
	// START MEASUREMENTSP
	char msg2[] = "streamD\r\n";
	loadcell_left.writeStr ( msg2 );
	loadcell_right.writeStr( msg2 );
}

void load_cell_stop( )
{	
	char msg[] = "stop\r\n";
	loadcell_left.writeStr ( msg );
	loadcell_right.writeStr( msg );
}

int read_data_left( )
{
	int rx_bytes=0;
	int avail = loadcell_left.available();
	if (avail)
	{
	//printf("L data avail!\n");
		rx_bytes = loadcell_left.readLine();
		char* ptr = strchr( loadcell_left.rx_buffer, '\r' );
		if (ptr)
			*ptr = ' ';
		return rx_bytes;
	} else return 0;
}

/* If data is available,
	Read 1 line, change \r to a space.
	return number of bytes read.
*/
int read_data_right( )
{
	int rx_bytes;
	int avail = loadcell_right.available();
	if (avail)
	{
		rx_bytes = loadcell_right.readLine();
		char* delim = strchr( loadcell_right.rx_buffer, '\r' );
		if (delim)
			*delim = ' ';
		return rx_bytes;
	} else return 0;
}


/* Given a line of 4 numbers comma separated.
Such as:  "   0.0,   0.0,   0.0,   0.0,"
Put into float array form 

Warning:  mResults must be allocated prior to this call!!

*/
void parse_loadcell_data( char* mLine, float* mResults )
{
	char* ptr = mLine;
	char* delim = strchr( mLine, ',' );
	if (delim) {
		*delim = 0;
		mResults[0] = atof( mLine );
	}

	ptr = delim+1;
	delim = strchr( ptr, ',' );
	if (delim) {
		*delim = 0;
		mResults[1] = atof( ptr );
	}

	ptr = delim+1;
	delim = strchr( ptr, ',' );
	if (delim) {
		*delim = 0;
		mResults[2] = atof( ptr );
	}

	ptr = delim+1;
	delim = strchr( ptr, ',' );
	if (delim) {
		*delim = 0;
		mResults[3] = atof( ptr );
	}
		
}

/* Parse the complete LoadCell line.
For example:
"LEFT:   0.0,   0.0,   0.0,   0.0,"
*/

void parse_loadcell_line( char* mLine, float* mResults )
{
	char* ptr = mLine;
	char* delim = strchr( mLine, ':' );
	
	parse_loadcell_data( mLine, mResults );
	
}

