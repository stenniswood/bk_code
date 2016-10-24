#ifndef _BODY_POSE_H_
#define _BODY_POSE_H_

enum robot_servo_indices {
	l_hip_rotate,
	l_hip_swing,
	l_hip_fb_swing,
	l_knee,
	l_ankle,
	r_hip_rotate,
	r_hip_swing,
	r_hip_fb_swing,
	r_knee,
	r_ankle,
		// ARMS:
	l_shoulder_rotate,
	l_shoulder_swing,
	l_shoulder_fore_rotate,
	l_elbow,
	l_wrist_rotate,
	l_wrist_swing,
	r_shoulder_rotate,
	r_shoulder_swing,
	r_shoulder_fore_rotate,
	r_elbow,
	r_wrist_rotate,
	r_wrist_swing,
	head_turn,
	head_tilt
};
const int MAX_SERVOS = 24;
struct stBodyPositionVector
{
	int 	num_active_servos;
	float	servo_values[MAX_SERVOS];	// See "struct stBodyPosition" at bottom of this file for order definition:
};


#endif

