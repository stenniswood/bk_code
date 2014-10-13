#ifndef _CAN_INSTANCE_H_
#define _CAN_INSTANCE_H_

#ifdef  __cplusplus
extern "C" {
#endif

#define INSTANCE_TX_MOB 3

////////////////////////////////////
#define NOT_CLAIMED	  0x00
#define CLAIM_PENDING 0x20
#define DIRTY 		  0x80
#define CLEAR_REQUEST 0x40
#define CLAIMED	  	  0x01
#define NOT_USING_INSTANCES 0x02
////////////////////////////////////
#define INSTANCE_EEPROM_ADDRESS 0

extern byte MyInstance;
extern byte Confirmed;

void random_number_adc_init ();
word pick_random_number		();
void can_prep_instance_request( struct sCAN* mMsg, byte tmpInstance );
void can_prep_instance_query  ( struct sCAN* mMsg );

void can_process_instance_request( struct sCAN* mMsg );
void instance_adc_init();

void save_instance_number();
void read_instance_number();
void save_confirmed_status();
void read_confirmed_status();
void ObtainInstanceNumber();

void can_instance_init();
void can_instance_timeslice();

#ifdef  __cplusplus
}
#endif

#endif


