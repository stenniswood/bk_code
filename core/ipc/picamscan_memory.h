#ifdef  __cplusplus
extern "C" {
#endif


#define 	NUM_ANALOG_CHANNELS 	8	
extern word Analog_Values[NUM_ANALOG_CHANNELS];


#define LOWSIDE_OFFSET 		 1
#define ANALOG_OFFSET 		 LOWSIDE_OFFSET+8
#define SERVO_ANGLE_OFFSET 	 ANALOG_OFFSET +2



void picam_save_segment_id();

int  picam_attach_memory();
void picam_reattach_memory();
void picam_detach_memory();

void picam_deallocate_memory();

void update_variables();
void ipc_write_analogs();
void ipc_read_lowside_outputs();
void ipc_write_buttons();
void ipc_write_leds();


#ifdef  __cplusplus
}
#endif
