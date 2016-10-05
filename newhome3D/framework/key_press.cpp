//
//  key_press.cpp
//  home3D
//
//  Created by Stephen Tenniswood on 10/4/16.
//  Copyright © 2016 Stephen Tenniswood. All rights reserved.
//

#include "key_press.hpp"
#include <string>
#include "init_samples.hpp"
#include "simulator_memory.h"


extern GLdouble centerAngle ;
extern GLdouble angle_increment;
void rotate( );
extern glArm*  selected_arm ;          // 0=left; 1=right;
extern glLeg*  selected_leg ;          // 0=left; 1=right;
extern bool  last_select_leg;
extern GLdouble centerX,centerY,centerZ;
extern GLdouble eyeX, eyeY, eyeZ;
void assume_robot_view();
extern union uGlobalVars theGlobals;
#define eye_follows_robots theGlobals.raw_array[1]
extern GLdouble centerX,centerY,centerZ;
void move_forward( float mAmount );
void move_sideways( float mAmount );
extern bool pauser;


void handleKeypress(unsigned char key, int x, int y)
{
    static int  sequence_number = 0;
    string seq_filename;
    static bool result = false;
    static float lift_height=0.0;
    float tmp;
    printf("handleKeypress(%c)\n",key);
    
    switch (key) {
        case 'v':   //show_tree = !show_tree;
            robot.morph_demos();
            robot.reset_limbs_path();
            robot.m_interpolated_index = 0;
            seq_filename = "sequence_"+std::to_string(sequence_number)+".txt";
            robot.save_sequence_file( seq_filename );
            break;
        case 'b':   result = robot.play_next();
            if (result) {
                robot.sample_limbs_path();
                robot.gl_unregister_limbs_path();
                robot.gl_register_limbs_path();
            }
            else robot.m_interpolated_index = 0;
            break;
        case 'z':   select_route();
            break;
        case '1': tmp = KitchenSinkCounter.m_cabinets[0].m_door.m_fraction_open += 0.05;
            KitchenSinkCounter.m_cabinets[0].m_door.open( tmp );
            break;
        case '2': tmp = KitchenSinkCounter.m_cabinets[1].m_door.m_fraction_open += 0.05;
            KitchenSinkCounter.m_cabinets[1].m_door.open( tmp );
            break;
        case '3': tmp = KitchenSinkCounter.m_cabinets[2].m_door.m_fraction_open += 0.05;
            KitchenSinkCounter.m_cabinets[2].m_door.open( tmp );
            break;
        case '4': tmp = KitchenSinkCounter.m_cabinets[3].m_door.m_fraction_open += 0.05;
            KitchenSinkCounter.m_cabinets[3].m_door.open( tmp );
            break;
        case '!': tmp = KitchenSinkCounter.m_cabinets[0].m_door.m_fraction_open -= 0.05;
            KitchenSinkCounter.m_cabinets[0].m_door.open( tmp );
            break;
        case '@': tmp = KitchenSinkCounter.m_cabinets[1].m_door.m_fraction_open -= 0.05;
            KitchenSinkCounter.m_cabinets[1].m_door.open( tmp );
            break;
        case '#': tmp = KitchenSinkCounter.m_cabinets[2].m_door.m_fraction_open -= 0.05;
            KitchenSinkCounter.m_cabinets[2].m_door.open( tmp );
            break;
        case '$': tmp = KitchenSinkCounter.m_cabinets[3].m_door.m_fraction_open -= 0.05;
            KitchenSinkCounter.m_cabinets[3].m_door.open( tmp );
            break;
            
        case '5': tmp = KitchenSinkCounter.m_cabinets[0].m_drawer.m_fraction_open += 0.05;
            KitchenSinkCounter.m_cabinets[0].m_drawer.open( tmp );
            break;
        case '6': tmp = KitchenSinkCounter.m_cabinets[1].m_drawer.m_fraction_open += 0.05;
            KitchenSinkCounter.m_cabinets[1].m_drawer.open( tmp );
            break;
        case '7': tmp = KitchenSinkCounter.m_cabinets[2].m_drawer.m_fraction_open += 0.05;
            KitchenSinkCounter.m_cabinets[2].m_drawer.open( tmp );
            break;
        case '8': tmp = KitchenSinkCounter.m_cabinets[3].m_drawer.m_fraction_open += 0.05;
            KitchenSinkCounter.m_cabinets[3].m_drawer.open( tmp );
            break;
        case '%': tmp = KitchenSinkCounter.m_cabinets[0].m_drawer.m_fraction_open -= 0.05;
            KitchenSinkCounter.m_cabinets[0].m_drawer.open( tmp );
            break;
        case '^': tmp = KitchenSinkCounter.m_cabinets[1].m_drawer.m_fraction_open -= 0.05;
            KitchenSinkCounter.m_cabinets[1].m_drawer.open( tmp );
            break;
        case '&': tmp = KitchenSinkCounter.m_cabinets[2].m_drawer.m_fraction_open -= 0.05;
            KitchenSinkCounter.m_cabinets[2].m_drawer.open( tmp );
            break;
        case '*': tmp = KitchenSinkCounter.m_cabinets[3].m_drawer.m_fraction_open -= 0.05;
            KitchenSinkCounter.m_cabinets[3].m_drawer.open( tmp );
            break;
        case 'd': centerAngle -= angle_increment; rotate(); printf("got d\n"); break;
        case 'a': centerAngle += angle_increment; rotate(); break;
        case ' ': robot.start_walking();
            printf("Angle=%6.2f; centerX=%6.2f; centerY=%6.2f; centerZ=%6.2f;\n", centerAngle,
                   centerX, centerY, centerZ );
            printf("eyeX=%6.2f; eyeY=%6.2f; eyeZ=%6.2f;\n", eyeX, eyeY, eyeZ );
            pauser = !pauser;
            //create_nose_line      ();
            break;
            
            /*   Next 4 - Select a limb for direct control   */
        case '{':   selected_arm = &(robot.m_right_arm);
            last_select_leg = false;
            break;
        case '}':   selected_arm = &(robot.m_left_arm);
            last_select_leg = false;
            break;
            
        case '[':   selected_leg = &(robot.m_right_leg);
            last_select_leg = true;
            break;
        case ']':   selected_leg = &(robot.m_left_leg);
            last_select_leg = true;
            break;
            
            // Standing Turn of Robot:
        case 'j': robot.standing_turn_sequence(180, true);
            break;
        case 'J': robot.standing_turn_sequence(-180, false);
            break;
            
            /*  Direct Limb Control:
             i=hip angle;    o=hip_rotate;   p=hip_side_swing;
             k=knee angle;   l=lift leg;     ,=ankle anlge;
             
             */
        case 'i': if (last_select_leg)
            selected_leg->increase_hip_angle            ( +2.);
        else
            selected_arm->increase_shoulder_rotate_angle( +2.);
            break;
        case 'I': if (last_select_leg)
            selected_leg->increase_hip_angle            ( -2.);
        else
            selected_arm->increase_shoulder_rotate_angle( -2.);
            break;
            
        case 'o': if (last_select_leg)
            selected_leg->increase_hip_rotate_angle( +2.);
        else
            selected_arm->increase_shoulder_angle  ( +2.);
            break;
        case 'O': if (last_select_leg)
            selected_leg->increase_hip_rotate_angle( -2.);
        else
            selected_arm->increase_shoulder_angle ( -2.);
            break;
            
        case 'p': if (last_select_leg)
            selected_leg->increase_hip_swing_angle( +2.);
        else
            selected_arm->increase_upper_arm_rotate_angle( +2.);
            break;
        case 'P': if (last_select_leg)
            selected_leg->increase_hip_swing_angle( -2.);
        else
            selected_arm->increase_upper_arm_rotate_angle( -2.);
            break;
            
        case 'k': if (last_select_leg)
            selected_leg->increase_knee_angle  (+2.);
        else
            selected_arm->increase_elbow_angle ( +2.); 	  break;
        case 'K': if (last_select_leg)
            selected_leg->increase_knee_angle  (-2.);
        else
            selected_arm->increase_elbow_angle ( -2.);    break;
            
        case 'l': if (last_select_leg) {
            lift_height += 1;
            float max_squat = robot.compute_max_squat_distance();
            printf("Max Squat delta = %6.2f; now=%6.2f\n", max_squat, lift_height);
            if (lift_height>max_squat)
                lift_height = max_squat;
            robot.squat_distance( lift_height );
            //bool left = (selected_leg==&(robot.m_left_leg));
            //robot.lift_leg_from_standing  (lift_height, left);
        }
            break;
        case 'L': if (last_select_leg) {
            lift_height -= 1;       if (lift_height<0) lift_height =0;
            robot.squat_distance( lift_height );
            //bool left = (selected_leg==&(robot.m_left_leg));
            //robot.lift_leg_from_standing  (lift_height, left);
        }
            break;
            
        case ',': if (last_select_leg)
            selected_leg->increase_ankle_angle (+2.);
        else
            selected_arm->increase_wrist_angle     ( +2.);       break;
        case '<': if (last_select_leg)
            selected_leg->increase_ankle_angle (-2.);
        else
            selected_arm->increase_wrist_angle     ( -2.);       break;
            
        case '.': if (last_select_leg==false)
            selected_arm->increase_wrist_rotate_angle  (+2.);    break;
        case '>': if (last_select_leg==false)
            selected_arm->increase_wrist_rotate_angle  (-2.);    break;
            /* END OF DIRECT LIMB CONTROL */
            
        case '-': robot.take_step( false);                           break;
        case '=': robot.take_step( true );   robot.start_walking();  break;
            
        case 'R':   if (eye_follows_robots == 0)
        {
            assume_robot_view();
            eye_follows_robots = 1;
        } else
            eye_follows_robots = 0;
            break;
            
            // Leg Movements:
        case 'g': selected_leg->increase_knee_angle  (+2.); 		 break;
        case 'G': selected_leg->increase_knee_angle  (-2.); 		 break;
        case 'B': selected_leg->increase_ankle_angle (-2.); 		 break;
            
        case 'w': centerY += 5.; printf("w"); break;		// look up.
        case 'x': centerY -= 5.; printf("w"); break;
        case 'q': eyeY -= 5.; centerY -= 5.; break;
        case 'r': eyeY += 5.; centerY += 5.; break;
            
        case 'c': move_forward (  5.);  break;
        case 'e': move_forward ( -5.); 	break;
        case 's': move_sideways(  2.);	break;
        case 'f': move_sideways( -2.); 	break;
            
        case 27: //Escape key
            exit(0);
    }
    robot.m_right_leg.make_foot_level();
    robot.m_left_leg.make_foot_level();
    
    glutPostRedisplay();
}

