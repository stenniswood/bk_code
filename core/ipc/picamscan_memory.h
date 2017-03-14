#ifdef  __cplusplus
extern "C" {
#endif

#define 		IPC_KEY_PICAM 0x04D4
#define 		NUM_ANALOG_CHANNELS 	8	

extern char* 	picam_shared_memory;
extern int 		picam_segment_id;
extern struct   picam_ipc_memory_map* ipc_memory_picam;

/******************** PICAM MEMORY MAP *****************/
struct picam_ipc_memory_map
{
	long int StatusCounter;
	char	 ConnectionStatus[64];
		
	float	 pan_angle;
	float	 tilt_angle;
	word	 Analog[NUM_ANALOG_CHANNELS];
	
	byte	 Lowside_driver1;	
	byte	 Lowside_driver2;
			
	word	 buttons;
	byte	 leds_byte;	
	
	long int RequestCount;
	long int AcknowledgedCount;
	
};


void picam_fill_memory		( );

int  picam_allocate_memory	( );
void picam_save_segment_id	( );
int  picam_attach_memory	( );
void picam_reattach_memory	( );
void picam_detach_memory	( );

void picam_deallocate_memory( );
int  picam_get_segment_size ( );

void update_variables		( );
void ipc_write_analogs		( word* Analog_Values );
void ipc_read_lowside_outputs();
void ipc_write_buttons		( );
void ipc_write_leds			( );


#ifdef  __cplusplus
}
#endif
