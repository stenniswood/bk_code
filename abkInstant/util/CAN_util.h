


byte extract_CAN_msg( struct sCAN* Msg, byte* DataPack			  );
int  pack_CAN_msg   ( struct sCAN* msg, byte* buffer, int bufSize );
void can_interface  ( );

void dump_buffer(byte* buffer, int bufSize);

