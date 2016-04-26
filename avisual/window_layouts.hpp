#ifndef _WINDOW_LAYOUT_H_
#define _WINDOW_LAYOUT_H_

/*extern DataSet ds1;
extern DataSet ds2;
extern DataSet ds3;
extern DataSet ds_tiltx;
extern DataSet ds_tilty; */

// L, R, T, B
//extern StickLeg  sf1;

extern Histogram hg;
extern LineGraph lg1;
extern LineGraph lg2;
extern LineGraph lg3;

extern Leveler l1;
extern Leveler l2;
extern Leveler l3;
extern Leveler l4;
extern Leveler l5;
extern Leveler l6;

void update_available_client_list();

void init_avisual		();
void init_pot_objs		();
void draw_pot_objs		();
void init_tilt_objs		();
void draw_tilt_objs		();
void init_control_test	();
void draw_control_test	();
void update_to_controls ();


#include "text_view.hpp"
#include "button.hpp"
#include "listbox.hpp"
#include "progressbar.hpp"
#include "checkbox.hpp"
#include "button_array_gui.hpp"


extern TextView 		ConnectionStatus;
extern TextView 		CmdText;
extern Button   		MyButt;
extern ListBox  		MyBoardList;
extern ProgressBar  	MyProgress;
extern CheckBox 		MyCheck;

//extern ButtonArrayMot	MyArray;

#endif
