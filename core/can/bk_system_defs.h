#ifndef _BK_SYSTEM_H_
#define _BK_SYSTEM_H_

/*#ifdef  __cplusplus
extern "C" {
#endif */

#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#define dprintf if (Debug) printf("%s ", __FILENAME__); if (Debug) printf

#define radians(arg) (3.1415*arg/180.0)
#define degrees(arg) (arg*180.0/3.1415)

// USE THESE FOR COLOR CODING THE PRINTF's!
#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"
// printf("%sred\n", KRED);
// printf("%snormal\n", KNRM);


//#include <avr/sfr_defs.h>
//#include <avr/common.h>
//#include <inttypes.h>
#include "global.h"

#define BOARD_MODEL 11
#define SerialNumber 0x12345678

//#define byte unsigned char
//#define uint32_t unsigned long
#define word unsigned short
#ifndef BOOL
#define BOOL byte
#endif
#define TRUE  1
#define FALSE 0

#ifndef NULL
#define NULL  0
#endif

//#define cli()  __asm__ __volatile__ ("cli" ::)
//#define sei()  __asm__ __volatile__ ("sei" ::)

#define min(a,b) (((a)<(b))?(a):(b))
#define max(a,b) (((a)>(b))?(a):(b))

#define CRYSTAL_FREQ 16000000
#define F_CPU CRYSTAL_FREQ

#define lo_word(one_long)	(one_long & 0x0000FFFF)
#define hi_word(one_long)	((one_long & 0xFFFF0000) >> 16)
#define lo(one_word) 		(one_word & 0x00FF)
#define hi(one_word) 		((one_word & 0xFF00) >> 8)

union float_array
{
	float value;
	uint8_t array[sizeof(float)];
};

#define one_second 400000
#define STROBE_SPEED 10

//void delay( long int mdelay );
void chip_init(void);

/*#ifdef  __cplusplus
}
#endif */

#endif
