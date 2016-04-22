#ifndef _CAN_DISPATHER_HPP_
#define _CAN_DISPATHER_HPP_



void CAN_add_rx_callback( BOOL (*mCallback)(struct sCAN*) );
void CAN_remove_rx_callback( BOOL (*mCallback)(struct sCAN*) );

void dispath_to_all_can_receivers( struct sCAN* mPtr );


#endif

