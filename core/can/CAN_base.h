#ifndef _CAN_base_H_
#define _CAN_base_H_

#ifdef  __cplusplus
extern "C" {
#endif

// CAN 2.0B 29 bit Identifier
// Note: this is not a union!!  Should be, but can't because the byte boundaries are
// not even. 
struct idType
{
	//long int	full_id;
	byte		id_array[4];
	// be careful about these not overlapping with above.	
	struct {  
		byte	block;
		word	id;
		byte	instance;		
	} group;
	/*struct {  
		int	priority:3;
		int	ext_datapage:2;
		int	tos:2;
		int	SourceAddress:11;		
		int	DestAddress:11;
	} diag;	// 15765-3
	*/
};
#define tID struct idType


struct sCAN
{
  tID 	id;					// CAN uses 29 bits
  struct {
  	byte  rtr    : 1;		// Remote Transmission Request (0=DataFrame; 1=RemoteFrame)
							// a node can request a msg from another node.
	byte DLC : 4;			// 4 bits => [0..15]
  } header;
  byte data[8];				// 
  word time_stamp;          // 
};

extern struct sCAN 	msg1;
extern struct sCAN 	msg2;
extern struct sCAN 	message;          // One buffer filled by various routines
extern struct sCAN 	rxmessage;        // One buffer filled by various routines

#ifdef  __cplusplus
}
#endif


#endif
