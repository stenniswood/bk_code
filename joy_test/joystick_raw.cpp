#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <string.h>

#include <fcntl.h>
#include <pthread.h>
#include "crc_32.h"
#include "joystick_raw.hpp"


#define PACKET_SIZE 83

unsigned char iBuffer[BUFFER_SIZE];

int 		quitState  = 0;
#define    	CUR_SIZ  16				// cursor size, pixels beyond centre dot

#define Debug 1   
pthread_t inputThread_id;
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#define Dprintf if (Debug) printf("%s ", __FILENAME__); if (Debug) printf

signed char mBuff[4];
char        dev_name[40];
extern void play_sound( int mIndex );


// eventThread reads from the JoystickRaw input file
void* eventThread(void *arg) 
{
	JoystickRaw* joy = (JoystickRaw*)arg;
	
	// Open JoystickRaw driver
	char path_name[40] = "/dev/";
	strcat( path_name, dev_name );
	
	//sprintf( path_name, "/dev/input/js0" );
	printf("opening JoystickRaw:  %s\n", path_name );
	joy->joystick_fd = open( path_name, O_RDWR );	// O_RDONLY 
	
	if (joy->joystick_fd >= 0) {
		printf( "Opened JoystickRaw:  %s\n", path_name );
		//system("aplay ./final-fantasy-v-music-victory-fanfare.wav");
	} else {
		system("aplay ./erro.wav");
		perror( "JoystickRaw not available!");
		printf("This app uses the Linux joydev API. ");
		printf("To use a PS4 controller, you need to install and run ds4drv ");
		printf("See:  https://github.com/chrippa/ds4drv  \n\n");
		quitState = 1;
		return &quitState;
	}
	
	//while (1) 
	{
		//joy->read_report     ( );
		//joy->print_event_info( );
	}
	return NULL;
}

JoystickRaw::JoystickRaw()
{
	Initialize();
}

JoystickRaw::~JoystickRaw()
{

}

void JoystickRaw::Initialize()
{ 
	//Dprintf("JoystickRaw::Initialize() ...\n");
	for (int i=0; i<MAX_AXIS; i++)
		m_axis_latest[i] = 0;

	// CREATE A THREAD TO READ FROM DEVICE : 
/*	for (int retries=0; retries<3; retries++) 
	{	
		int result = pthread_create( &inputThread_id, NULL, &eventThread, this );
		if (result != 0) {
			if (Debug) fprintf(stderr, "Unable to initialize the JoystickRaw\n");
			sleep(1);
			printf("Retrying... connection to JoystickRaw.");
			continue;
		}
		break;
	} */
} 	

void JoystickRaw::dump_i_buffer(  )
{
	printf( "INPUT BUFFER : " );
	for (int i=0; i<m_bytes_read; i++)
	{
		if ((i%16)==0) printf("\n%4d :  ", i);
		printf("%2x ", iBuffer[i] );		
	}
	printf("\n");
}


#ifdef PS4
std::string get_button_name( int mButtonIndex )
{
	switch (mButtonIndex)
	{
	case PS_PAD_PUSH 		: return "TOUCH PAD PUSH";	
	case PS_BUTTON 			: return "PS";
	case PS_L3 				: return "L3";
	case PS_R3			 	: return "R3";
	case PS_SHARE 			: return "SHARE";
	case PS_OPTIONS 		: return "OPTIONS";
	case PS_L1 				: return "L1";
	case PS_R1 				: return "R1";
	case PS_L2		 		: return "L2";
	case PS_R2		 		: return "R2";
		
	case PS_DPAD_UP 	    : return "UP";
	case PS_DPAD_DOWN 		: return "DOWN";
	case PS_DPAD_LEFT 		: return "LEFT";
	case PS_DPAD_RIGHT 		: return "RIGHT";

	case PS_TRIANGLE 		: return "TRIANGLE";
	case PS_CIRCLE 			: return "CIRCLE";
	case PS_X 				: return "X";
	case PS_SQUARE 			: return "SQUARE";
	default: return "Unknown";
	};
}
#else
std::string get_button_name( int mButtonIndex )
{
	switch (mButtonIndex)
	{
	case PS_SELECT 			: return "SELECT";
	case PS_L3 				: return "LEFT_JOY_PUSH";
	case PS_R3			 	: return "RIGHT_JOY_PUSH";
	case PS_START 			: return "START";
	case PS_UP 				: return "UP";
	case PS_RIGHT 			: return "RIGHT";
	case PS_DOWN 			: return "DOWN";
	case PS_LEFT 			: return "LEFT";
	case PS_LEFT_TOP 		: return "L1";
	case PS_RIGHT_BOTTOM 	: return "R2";
	case PS_LEFT_BOTTOM 	: return "L2"; 
	case PS_RIGHT_TOP 		: return "R1";
	case PS_TRIANGLE 		: return "TRIANGLE";
	case PS_CIRCLE 		: return "CIRCLE";
	case PS_X 				: return "X";
	case PS_SQUARE 		: return "SQUARE";
	default: return "Unknown";
	};
}
#endif


std::string get_axis_name( int mAxisIndex )
{
	switch (mAxisIndex)
	{
	case PS_AXIS_LPOD_LR	: return "LEFT - LR";
	case PS_AXIS_LPOD_UD    : return "LEFT - UD";
	case PS_AXIS_RPOD_LR	: return "RIGHT - LR";
	case PS_AXIS_RPOD_UD 	: return "RIGHT - UD";
	case PS_AXIS_TILT_ROLL 	  : return "TILT - ROLL";
	case PS_AXIS_TILT_PITCH	  : return "TILT - PITCH";	
	case PS_AXIS_BATTERY 	  : return "BATTERY";		
	case PS_AXIS_ANGLEX_VEL   : return "Angular Velocity X";		
	case PS_AXIS_ANGLEY_VEL   : return "Angular Velocity Y";		
	case PS_AXIS_ANGLEZ_VEL   : return "Angular Velocity Z";		

	case PS_DPAD	 	    : return "DPAD";
	case PS_AXIS_LTRIGGER	: return "L TRIGGER";
	case PS_AXIS_RTRIGGER	: return "R TRIGGER";

	case PS_AXIS_ACCELX   	: return "Accel X (Roll )";
	case PS_AXIS_ACCELY   	: return "Accel Y (     )";		
	case PS_AXIS_ACCELZ   	: return "Accel Z (Pitch)";
	
	default: return "unknown"; break;
	}
}

/*void JoystickRaw::print_event_info()
{
	if (Debug==0) return;

	if (m_ev.type==JS_EVENT_BUTTON) 
	{
		//printf("time=%lu; Button #%d = %d\n", m_ev.time, m_ev.number, m_ev.value );
		std::string button_name = get_ps4_button_name( m_ev.number );
		if (m_ev.value==1)
			printf("time=%lu; BUTTON %d %s down\n", m_ev.time, m_ev.number, button_name.c_str() );
		else 
			printf("time=%lu; BUTTON %d %s up\n", m_ev.time, m_ev.number, button_name.c_str() );
	}
	else if (m_ev.type==JS_EVENT_AXIS) 
	{
		//if (m_ev.number==25)
		//		return;

		// PS4 pods use (0 & 1) and right (2 & 5) 
				
		// Only Print 0,1,2,5 which are the anlog pod positions.  Not the tilt angles.
		if ((m_ev.number==PS_AXIS_LPOD_LEFT_RIGHT)  || (m_ev.number==PS_AXIS_LPOD_UP_DOWN) ||
		    (m_ev.number==PS_AXIS_RPOD_LEFT_RIGHT) || (m_ev.number==PS_AXIS_RPOD_UP_DOWN) )		
		{
			std::string axis_name = get_axis_name( m_ev.number );
			printf("\ttime=%lu; Axis   %d %s = %d\n", m_ev.time, m_ev.number, axis_name.c_str(), m_ev.value );	
		}
		else 
			printf("\ttime=%lu; Axis   %d = %d\n", m_ev.time, m_ev.number,  m_ev.value );			
	} else if (m_ev.type & JS_EVENT_INIT) 
	{
		char str[10];
		sprintf(str,"unknown");
		if ((m_ev.type&0x7F)==JS_EVENT_AXIS)
			sprintf( str, "AXIS ");
		else if ((m_ev.type&0x7F)==JS_EVENT_BUTTON)
			sprintf (str, "Button");
		printf("time=%lu; INIT %s; number=%d; value=%8x; \n", m_ev.time, str, m_ev.number, m_ev.value );	
	} else 
		printf("time=%lu; type=0x%2x; number=%d; value=%8x; \n", m_ev.time, m_ev.type, m_ev.number, m_ev.value );	
}*/



void  JoystickRaw::mark_event_acknowledged()
{
	m_new_axis = false;
	m_new_button = false;
	m_new_event = true;
}


void  JoystickRaw::extract_buttons()
{
	m_buttons_latest[PS_R3	   ].set_value( iBuffer[PS_AXIS_OPTSHARE_LR_DATAPOS] & 0x80 );
	m_buttons_latest[PS_L3	   ].set_value( iBuffer[PS_AXIS_OPTSHARE_LR_DATAPOS] & 0x40 );
	m_buttons_latest[PS_OPTIONS].set_value( iBuffer[PS_AXIS_OPTSHARE_LR_DATAPOS] & 0x20 );
	m_buttons_latest[PS_SHARE  ].set_value( iBuffer[PS_AXIS_OPTSHARE_LR_DATAPOS] & 0x10 );

	m_buttons_latest[PS_R2].set_value	( iBuffer[PS_AXIS_OPTSHARE_LR_DATAPOS] & 0x08 );
	m_buttons_latest[PS_L2].set_value	( iBuffer[PS_AXIS_OPTSHARE_LR_DATAPOS] & 0x04 );
	m_buttons_latest[PS_R1].set_value	( iBuffer[PS_AXIS_OPTSHARE_LR_DATAPOS] & 0x02 );
	m_buttons_latest[PS_L1].set_value	( iBuffer[PS_AXIS_OPTSHARE_LR_DATAPOS] & 0x01 );

	m_buttons_latest[PS_BUTTON  ].set_value( iBuffer[PS_COUNTER_DATAPOS]&0x01 );	
	m_buttons_latest[PS_PAD_PUSH].set_value( iBuffer[PS_COUNTER_DATAPOS]&0x02 );	

	// 0x08 is released, 0=N, 1=NE, 2=E, 3=SE, 4=S, 5=SW, 6=W, 7=NW
	unsigned char dpad = (iBuffer[PS_AXIS_NSEW_DPAD_DATAPOS] & 0x0F);
	m_buttons_latest[PS_DPAD	  ].set_value( dpad 					   );
	m_buttons_latest[PS_DPAD_UP	  ].set_value( (dpad == 0)?(true):(false)  );
	m_buttons_latest[PS_DPAD_RIGHT].set_value( (dpad == 2)?(true):(false)  );
	m_buttons_latest[PS_DPAD_DOWN ].set_value( (dpad == 4)?(true):(false)  );
	m_buttons_latest[PS_DPAD_LEFT ].set_value( (dpad == 6)?(true):(false)  );
	
	m_buttons_latest[PS_TRIANGLE].set_value( (iBuffer[PS_AXIS_NSEW_DPAD_DATAPOS] & 0x80)>0 );
	m_buttons_latest[PS_CIRCLE 	].set_value( (iBuffer[PS_AXIS_NSEW_DPAD_DATAPOS] & 0x40)>0 );
	m_buttons_latest[PS_X 		].set_value( (iBuffer[PS_AXIS_NSEW_DPAD_DATAPOS] & 0x20)>0 );
	m_buttons_latest[PS_SQUARE 	].set_value( (iBuffer[PS_AXIS_NSEW_DPAD_DATAPOS] & 0x10)>0 );
}

void  JoystickRaw::extract_analogs()
{
	m_axis_latest[PS_AXIS_LPOD_LR] = iBuffer[PS_AXIS_LPOD_LR_DATAPOS];
	m_axis_latest[PS_AXIS_LPOD_UD] = iBuffer[PS_AXIS_LPOD_UD_DATAPOS];
	m_axis_latest[PS_AXIS_RPOD_LR] = iBuffer[PS_AXIS_RPOD_LR_DATAPOS];
	m_axis_latest[PS_AXIS_RPOD_UD] = iBuffer[PS_AXIS_RPOD_UD_DATAPOS];

	m_report_counter = (iBuffer[PS_COUNTER_DATAPOS] & 0xFC)>>2;

	m_axis_latest[PS_AXIS_LTRIGGER] = iBuffer[PS_AXIS_LTRIGGER_DATAPOS];
	m_axis_latest[PS_AXIS_RTRIGGER] = iBuffer[PS_AXIS_RTRIGGER_DATAPOS];

	m_axis_latest[PS_AXIS_BATTERY]  = iBuffer[PS_AXIS_BATTERY_DATAPOS] & 0x0F;

	m_axis_latest[PS_AXIS_ANGLEX_VEL] = (iBuffer[PS_AXIS_ANGLEX_VELOCITY_DATAPOS]<<8) + iBuffer[PS_AXIS_ANGLEX_VELOCITY_DATAPOS+1];
	m_axis_latest[PS_AXIS_ANGLEY_VEL] = (iBuffer[PS_AXIS_ANGLEY_VELOCITY_DATAPOS]<<8) + iBuffer[PS_AXIS_ANGLEY_VELOCITY_DATAPOS+1];
	m_axis_latest[PS_AXIS_ANGLEZ_VEL] = (iBuffer[PS_AXIS_ANGLEZ_VELOCITY_DATAPOS]<<8) + iBuffer[PS_AXIS_ANGLEZ_VELOCITY_DATAPOS+1];

	m_axis_latest[PS_AXIS_ACCELX] = (iBuffer[PS_AXIS_ACCELX_VELOCITY_DATAPOS]<<8) + iBuffer[PS_AXIS_ACCELX_VELOCITY_DATAPOS+1];
	m_axis_latest[PS_AXIS_ACCELY] = (iBuffer[PS_AXIS_ACCELY_VELOCITY_DATAPOS]<<8) + iBuffer[PS_AXIS_ACCELY_VELOCITY_DATAPOS+1];
	m_axis_latest[PS_AXIS_ACCELZ] = (iBuffer[PS_AXIS_ACCELZ_VELOCITY_DATAPOS]<<8) + iBuffer[PS_AXIS_ACCELZ_VELOCITY_DATAPOS+1];

	unsigned char dpad = (iBuffer[PS_AXIS_NSEW_DPAD_DATAPOS] & 0x0F);
	m_axis_latest[PS_DPAD] =  dpad;
}

void JoystickRaw::extract_touchpad()
{
	m_num_trackpad_packets = iBuffer[PS_TP_NUM_PACKETS_DATAPOS] & 0x0F;

	if ((iBuffer[PS_TP_FINGERID_1_DATAPOS] & 0x80)==0)
	{		
		m_num_fingerid_1 = iBuffer[PS_TP_FINGERID_1_DATAPOS]& 0x7F;
		printf("Finger1 active - ID=%x\n", m_num_fingerid_1 );
	}	
}

void JoystickRaw::open( const char* mPathname )
{
	 printf("Opening hidraw device: %s\t", mPathname );
     joystick_fd = ::open( mPathname, O_RDWR );
     if (joystick_fd <= 0) {
     	char str[80];
		sprintf ( str, "ERROR:  cannot open device %s\n", mPathname );
		perror  ( str );
	 }
	 printf("opened!\n");
}

long int found_crc;

long int JoystickRaw::read_report()
{	
	m_bytes_read = read( joystick_fd, (void*)iBuffer, PACKET_SIZE );
	//printf("read_report() : bytes_read = %d\n", m_bytes_read );

	if (m_bytes_read<0)
	{	perror(" error reading from device.\n");	return -1;	}
	else if (m_bytes_read==0)
	{  printf("End of file reached.\n"); }

	uint32_t c1 = iBuffer[m_bytes_read-1]<<(3*8);
	uint32_t c2 = iBuffer[m_bytes_read-2]<<(2*8);
	uint32_t c3 = iBuffer[m_bytes_read-3]<<(1*8);
	uint32_t c4 = iBuffer[m_bytes_read-4]<<(0*8);
	//printf("%db :  c1,2,3,4 : %lx %lx %lx %lx \n", bytes_read, c1, c2, c3, c4 );
	found_crc = ((c1) | (c2) | (c3) | (c4));

	return m_bytes_read;
}

void JoystickRaw::update()
{
	size_t bytes_read = read_report();
	if (bytes_read > 8)
	{
		printf("Found CRC: %x;  ", found_crc );	
		byte*   tmpBuff = new byte[BUFFER_SIZE];
		tmpBuff[0] = 0xa1;	
		memcpy( &(tmpBuff[1]), iBuffer, bytes_read );
		long int crc = crc_32( tmpBuff, 75 /*bytes_read-5+1*/ );
		if (found_crc==crc)
			printf("CRCs Okay\n");
		else 
			printf("CRC ERROR!\n");
		//printf("Calculated CRC: %x;\n", crc );
		delete tmpBuff;
	}
	
	extract_buttons();
	extract_analogs();
}

void JoystickRaw::close()
{ 
	::close(joystick_fd);
}

void JoystickRaw::print_button( int mIndex )
{
	std::string bn = get_button_name( mIndex );
	printf( "%20s : %s\t", bn.c_str(), m_buttons_latest[mIndex].value ? "true" : "false" );
}

void JoystickRaw::print_axis( int mAxisIndex )
{
	std::string an = get_axis_name( mAxisIndex );
	printf( "%20s : %d\t", an.c_str(), m_axis_latest[mAxisIndex] );
}

void JoystickRaw::print_button_info( )
{
	printf( "BUTTONS: \n" );
	print_button( PS_L1 );			print_button( PS_R1 );			printf("\n");
	print_button( PS_L2 );			print_button( PS_R2 );			printf("\n");
	print_button( PS_L3 );			print_button( PS_R3 );			printf("\n");

	print_button( PS_SHARE 		);	print_button( PS_OPTIONS 	);	printf("\n");
	print_button( PS_BUTTON   	);	print_button( PS_PAD_PUSH 	);	printf("\n");

	print_button( PS_DPAD_UP 	);	print_button( PS_DPAD_LEFT  );	printf("\n");
	print_button( PS_DPAD_DOWN  );	print_button( PS_DPAD_RIGHT );	printf("\n");

	print_button( PS_TRIANGLE 	);	print_button( PS_CIRCLE  	);	printf("\n");
	print_button( PS_SQUARE 	);	print_button( PS_X  		);	printf("\n");
	printf("\n");
}

void  JoystickRaw::print_analogs_info( )
{
	printf("ANALOG AXIS:\tCounter=%d\n", m_report_counter);
	print_axis( PS_AXIS_LPOD_LR );		print_axis( PS_AXIS_RPOD_LR );	 printf("\n");
	print_axis( PS_AXIS_LPOD_UD );		print_axis( PS_AXIS_RPOD_UD );	 printf("\n");

	//print_axis( PS_AXIS_TILT_ROLL  );	print_axis( PS_AXIS_TILT_PITCH ); 
	printf("\n");
 
	print_axis( PS_AXIS_ANGLEX_VEL );	print_axis( PS_AXIS_ACCELX );	 printf("\n");
	print_axis( PS_AXIS_ANGLEY_VEL );	print_axis( PS_AXIS_ACCELY );	 printf("\n");
	print_axis( PS_AXIS_ANGLEZ_VEL );	print_axis( PS_AXIS_ACCELZ );	 printf("\n");

	print_axis( PS_AXIS_LTRIGGER );		print_axis( PS_AXIS_RTRIGGER );	 printf("\n");
	print_axis( PS_AXIS_BATTERY  );
	
	// 0x08 is released, 0=N, 1=NE, 2=E, 3=SE, 4=S, 5=SW, 6=W, 7=NW
	switch ( m_axis_latest[PS_DPAD] )
	{
		case 0 : printf("DPAD : North");		break;
		case 1 : printf("DPAD : North East");	break;
		case 2 : printf("DPAD : East");			break;
		case 3 : printf("DPAD : South East");	break;
		case 4 : printf("DPAD : South");		break;
		case 5 : printf("DPAD : South West");	break;
		case 6 : printf("DPAD : West");			break;
		case 7 : printf("DPAD : North West");	break;
		case 8 : printf("DPAD : Released");		break;
		default : break;
	}
	printf("\n");	
}

void	JoystickRaw::print_touchpad_info ( )
{

}
