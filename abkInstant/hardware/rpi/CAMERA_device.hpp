/* Following are sentences generated by this object:
 * 
 * "what adrenaline boards do you have?"
 * "how many "+mBoardType+" boards do you have?"
 * "call board instance "+mInstance+" by "+mName+"."
 * "select board instance "+mInstance+"." 
 * "set camera tilt to "+mDegrees+"."  
 * "set camera pan to "+mDegrees+"." 
 * "set camera tilt to "+mTiltDegrees+" and pan to "+mPanDegrees+"." 
 * "set lowside driver pin "+mPin+" to "+mValue+"."
 * "read analog pin "+mPin+"." 
 * "read all analog pins."
 * "set GPIO pin "+mPin+" to "+mVal+"." 
 * "set GPIO pin "+mPin+" as "+direction+"." 
 * "read GPIO pin "+mPin+"." 
 * "send all CAN traffic to me on port "+mPort+"."
 * "listen to my CAN traffic." 
 * "listen to CAN message: "+mMessage+"."
 *   
 */
/*
 * "what adrenaline boards do you have?"
 * "set camera tilt to "+mDegrees+"."
 * "set camera tilt to "+mDegrees+"."
 * "set camera pan to "+mDegrees+"." 
 * "set camera tilt to "+mTiltDegrees+" and pan to "+mPanDegrees+"." 
 * "set lowside driver pin "+mPin+" to "+mValue+"." 
 * "read analog pin "+mPin+"."
 * "read all analog pins." 
 * "set GPIO pin "+mPin+" to "+mValue+"."
 * "set GPIO pin "+mPin+" as "+direction+"."
 * "read GPIO pin "+mPin+"." 
 * "send all CAN traffic to me on port "+mPort+"."  
 * "listen to my CAN traffic." 
 * "listen to CAN message: "+mMessage+"." 
 */

#ifdef  __cplusplus
extern "C" {
#endif


void set_camera_tilt_to( float mDegrees );
void set_camera_pan_to ( float mDegrees );

void lower_camera_by   ( float mDegrees );
void raise_camera_by   ( float mDegrees );

void move_camera_right_by( float mDegrees );
void move_camera_left_by ( float mDegrees );

float get_camera_tilt  ( float mDegrees );
float get_camera_pan   ( float mDegrees );


#ifdef  __cplusplus
}
#endif

