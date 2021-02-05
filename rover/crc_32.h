#include <stdint.h>


typedef uint32_t size_t;


uint32_t		crc_32			( const unsigned char *input_str, size_t num_bytes );
uint32_t		update_crc_32	(      uint32_t crc, unsigned char c        );


