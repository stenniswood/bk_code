
#include <shapes.h>
#include <fontinfo.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <vector>
#include "VG/openvg.h"
#include "VG/vgu.h"
#include <shapes.h>
#include <fontinfo.h>
#include "CAN_Interface.h"

#include "dataset.hpp"
#include "display.h"
#include "adrenaline_windows.h"
#include "adrenaline_graphs.h"
#include "display_manager.hpp"
#include "client_memory.hpp"
#include "client_list_control.hpp"
#include "robot_performance.hpp"


#define Debug 1

static DataSet ds1;
static DataSet ds2;
static DataSet ds3;
const char chart1_title[] = "Hip Accuracy Graph";
const char chart2_title[] = "Knee Accuracy Graph";
void fill_fake_data_()
{
	ds3.add( 10. );	ds3.add( 10. );	ds3.add( 10. );		ds3.add( 10. );
	ds3.add( 11. );	ds3.add( 11. );	ds3.add( 11. );		ds3.add( 10. );
	ds3.add( 9. );	ds3.add( 9. );	ds3.add( 9. );		ds3.add( 9.5 );
	ds3.add( 8. );	ds3.add( 8. );	ds3.add( 8.75 );	ds3.add( 8.5 );
	ds3.add( 12. );	ds3.add( 12. );	ds3.add( 11.5 );	ds3.add( 11.25 );	
	ds3.add( 14. );	ds3.add( 15.5 ); ds3.add( 13.5 );	ds3.add( 12.2 );
	ds3.add( 6. );	ds3.add( 6.1 );	ds3.add( 6.9 );		ds3.add( 9.5 );
	ds3.add( 5. );	ds3.add( 16.2 ); ds3.add( 8.78 );	ds3.add( 8.88 );
}


RobotPerformancePanel::RobotPerformancePanel()
:Window()
{
	Initialize();
}
void 	RobotPerformancePanel::Initialize  ( )
{ 
	Window::Initialize();
	fill_fake_data_();
}
void	RobotPerformancePanel::calc_metrics( )
{
	Window::calc_metrics(); 
}

int		RobotPerformancePanel::place_views ( )
{ 
	float l_third = width / 3;
	float l_two_third = 2* width / 3;
	float height_third = height / 3;
	float height_two_third = 2 * height / 3;
	
	m_hg_1.set_position ( l_third, height_third+100, l_third+100, height_third );
	m_hg_2.set_position ( l_two_third, height_third+100, l_two_third+100, height_third );
	
//	register_child( &m_hg_1 );
//	register_child( &m_hg_2 );	
	return 1;
}

void	RobotPerformancePanel::setup_headings( )
{ 
}

int		RobotPerformancePanel::onCreate	(  )
{
	place_views();
	
	m_hg_1.set_title 			( (char*) chart1_title );
	m_hg_1.set_xLabel			( (char*) "Angle" 		);
	m_hg_1.set_yLabel			( (char*) "Samples" 	);	
//	m_hg_1.add_data_series		( &ds3 );

	m_hg_2.set_title 			( (char*) chart1_title );
	m_hg_2.set_xLabel			( (char*) "Angle" 		);
	m_hg_2.set_yLabel			( (char*) "Samples" 	);	
//	m_hg_2.add_data_series		( &ds3 );
 
	return Control::onCreate();
	
}	// chance to load resources, call functions which use fonts
int   	RobotPerformancePanel::draw		(  )
{
	return Window::draw(); 
}


