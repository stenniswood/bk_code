#ifndef _SEQ_INIT_HPP_
#define _SEQ_INIT_HPP_

#include "teach_pendant.hpp"
extern TeachPendant 	teach_pendant;
extern struct sCAN msg;

// Taken from "AdrenalineFirmware\BigMotorEn\motor.h"
#define MODE_SEND_UPDATES_NONE	0x00	// 
#define MODE_SEND_UPDATES_10ms	0x10	// 
#define MODE_SEND_UPDATES_20ms	0x20	// 
#define MODE_SEND_UPDATES_50ms	0x40	// 
#define MODE_SEND_UPDATES_100ms	0x80	// 
#define MODE_SEND_POSITION_RAW	0x01 	// Measured Pot/Encoder, CurrentLeft, CurrentRight, Speed
#define MODE_SEND_POSITION_CALC 0x02 	// Calculated quantities: Angle (deg*100), Current (Amps*100)
#define MODE_SEND_STATUS		0x04 	// 

// Wiring PI pin number (gpio pin 15)
#define CAN_INT_PIN 3
#define BUTTON1_INT_PIN 4
#define BUTTON2_INT_PIN 5
#define BUTTON3_INT_PIN 6



void Button1r_isr();
void Button2r_isr();
void Button3r_isr();

void init();
void init_hardware();
void help();
void print_args(int argc, char *argv[]);

void* scheduler_thread(void* n);
void create_threads();


void read_cnfs();
void jog();

#endif
