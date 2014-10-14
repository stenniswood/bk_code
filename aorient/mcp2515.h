/* Copyright (c) 2007 Fabian Greif
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
// ----------------------------------------------------------------------------

#ifndef	MCP2515_H
#define MCP2515_H

//#include <avr/io.h>
//#include <util/delay.h>
//#include "WConstants.h"
#include "global.h"
//#include "mcp2515.h"
#include "mcp2515_defs.h"
//#include "defaults.h"

/*typedef struct
{
	uint16_t id;				// CAN uses 11 bits
	struct {
		int8_t  rtr    : 1;		// Remote Transmission Request (0=DataFrame; 1=RemoteFrame)
								// a node can request a msg from another node.
		uint8_t length : 4;		// [0..15]
	} header;
	uint8_t data[8];			// 
} tCAN; */


// -------------------------------------------------------------------------
// Schreibt/liest ein Byte ueber den Hardware SPI Bus
uint8_t 	spi_putc( uint8_t data );
void 		mcp2515_write_register( uint8_t adress, uint8_t data );
uint8_t 	mcp2515_read_register(uint8_t adress);
void 		mcp2515_bit_modify(uint8_t adress, uint8_t mask, uint8_t data);
uint8_t 	mcp2515_read_status(uint8_t type);
uint8_t 	mcp2515_init(uint8_t speed);

// check if there are any new messages waiting
uint8_t 	mcp2515_check_message(void) ;

// check if there is a free buffer to send messages
uint8_t 	mcp2515_check_free_buffer(void);
//uint8_t 	mcp2515_get_message(tCAN *message);
//uint8_t 	mcp2515_send_message(tCAN *message);

#endif
