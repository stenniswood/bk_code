#ifdef  __cplusplus
extern "C" {
#endif


void update_variables		 ();
void ipc_print_mem			 ();

void ipc_read_servos		 ();
void ipc_write_analogs		 ();
void ipc_read_lowside_outputs();
void ipc_write_buttons		 ();
void ipc_write_leds			 ();


#ifdef  __cplusplus
}
#endif