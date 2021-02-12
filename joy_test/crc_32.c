#include <stdint.h>

#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "crc_32.h"
#include "tab_crc32.inc"


#define		CRC_START_32		0xFFFFFFFFul


unsigned char sample_data[] = {
0xA1, 0x11, 0xC0, 0x00, 0x83, 0x81, 0x7E, 0x7E, 
0x08, 0x00, 0x3C, 0x00, 0x00, 0x83, 0xA2, 0x07, 
0xF1, 0xFF, 0xF9, 0xFF, 0x04, 0x00, 0x21, 0x03, 
0x17, 0x1F, 0x29, 0xF9, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 
0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x80,
0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 
0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 
0x00, 0x80, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 
0x00, 0x00, 0x00
};


/*
 * uint32_t crc_32( const unsigned char *input_str, size_t num_bytes );
 *
 * The function crc_32() calculates in one pass the common 32 bit CRC value for
 * a byte string that is passed to the function together with a parameter
 * indicating the length.
 */
uint32_t crc_32( const unsigned char *input_str, size_t num_bytes ) 
{
	uint32_t crc;
	uint32_t rev;	
	const unsigned char *ptr;
	size_t a;

	crc = CRC_START_32;
	ptr = input_str;

	if ( ptr != NULL ) for (a=0; a<num_bytes; a++) 
	{
		//printf("%02x ", *ptr);		if ((a%32)==31) printf("\n");		
		int f_index = (crc ^ (uint32_t) *ptr) & 0x000000FFul;
		crc = (crc >> 8) ^ crc_tab32[ f_index ];
		// printf("%2d:f_index=%3d; crc=%8x\t", a, f_index, crc );  if ((a % 5)==0)  printf("\n");
        ptr++;        
	}
	//printf("\n\n");

	crc = crc ^ 0xFFFFFFFFul;
	//printf("CRC = %4x\n", crc);
	return (crc);
}  

/*
 * uint32_t update_crc_32( uint32_t crc, unsigned char c );
 *
 * The function update_crc_32() calculates a new CRC-32 value based on the
 * previous value of the CRC and the next byte of the data to be checked.
 */

uint32_t update_crc_32( uint32_t crc, unsigned char c ) {

	return (crc >> 8) ^ crc_tab32[ (crc ^ (uint32_t) c) & 0x000000FFul ];
}  



/*int main( int argc, char *argv[] ) 
{
	uint32_t result = crc_32( sample_data, 75 );
	printf("CRC 32 Checksum for sampledata = %lx\n", result );
	exit(1);
}  

/*unsigned long wantcrc = 0x0B5D0A7D;
	rev = CRC_START_32 ^ wantcrc;
	ptr = (input_str + num_bytes-1);
	if ( ptr != NULL ) for (long r=num_bytes-1; r>=0; r--) 
	{
		int r_index = rev >> (3 * 8);
        rev = (rev << 8) ^ reverse[r_index] ^ *ptr;

		printf("%2d:r_index=%3d; crc=%4x\t", r, r_index, rev );
		if ((r % 5)==0)	printf("\n");

        ptr--;        		
	} printf("REVERSE CRC = %4x\n", rev);	*/
	
