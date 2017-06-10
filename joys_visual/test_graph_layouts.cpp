/*************************************************************************
NAME	:	test_layouts.cpp
DESCRIPTION:

	This file is used to test the primitive objects in the Adrenaline
	windowing package.

	For combinations of objects see:  test_combo_layouts.cpp
	
DATE	: November 20, 2014
AUTHOR	: Steve Tenniswood
**************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include <vector>
#include "VG/openvg.h"
#include "VG/vgu.h"
#include <shapes.h>
#include <fontinfo.h>

#include "GyroViewI.hpp"
#include "CAN_Interface.h"
#include "adrenaline_windows.h"
#include "adrenaline_graphs.h"
#include "display_manager.hpp"
#include "frame_window.hpp"
#include "test_layouts.hpp"

#include "visual_memory.h"
#include "audio_memory.h"
#include "fft.h"
#include "test_combo_layouts.hpp"
#include "audio_app.hpp"

#include "card_games.h"



static DataSet ds1;
static DataSet ds2;
static DataSet ds3;
static Histogram 		hg ( 100, 300, 600, 400 );		// was ( 100, 300, 300, 100 );
static LineGraph 		lg1( 100, 300, 600, 400 );
static LineGraph 		lg2( 400, 600, 600, 400 );
static LineGraph 		lg3( 700, 900, 600, 400 );

static BarGraph  		bg ( 100, 500, 600, 400 );		// was ( 100, 300, 300, 100 );
static StackedBarGraph  sbg( 100, 300, 600, 400 );		// was ( 100, 300, 300, 100 );
static ScatterGraph  	sg ( 100, 300, 600, 400 );		// was ( 100, 300, 300, 100 );


void fill_fake_data1()
{
	ds1.add( 1. );	ds1.add( 2. );	ds1.add( 3. );	ds1.add( 4. );
	ds1.add( 5. );	ds1.add( 6. );	ds1.add( 7. );	ds1.add( 8. );
	ds1.add( 9. );	ds1.add( 10. );	ds1.add( 11. );	ds1.add( 12. );
	ds1.add( 13. );	ds1.add( 14. );	ds1.add( 15. );	ds1.add( 16. );
	ds1.add( 17. );	ds1.add( 18. );	ds1.add( 19. );	ds1.add( 20. );	
	ds1.add( 21. );	ds1.add( 22. );	ds1.add( 23. );	ds1.add( 24. );
	ds1.add( 25. );	ds1.add( 26. );	ds1.add( 27. );	ds1.add( 28. );
	ds1.add( 29. );	ds1.add( 30. );	ds1.add( 31. );	ds1.add( 32. );						
}
void fill_fake_data2()
{
	ds2.add( 1. );	ds2.add( 3. );	ds2.add( 5. );	ds2.add( 7. );
	ds2.add( 9. );	ds2.add( 11. );	ds2.add( 13. );	ds2.add( 15. );
	ds2.add( 17. );	ds2.add( 19. );	ds2.add( 21. );	ds2.add( 23. );
	ds2.add( 25. );	ds2.add( 27. );	ds2.add( 29. );	ds2.add( 31. );
	ds2.add( 33. );	ds2.add( 35. );	ds2.add( 37. );	ds2.add( 39. );	
	ds2.add( 41. );	ds2.add( 43. );	ds2.add( 45. );	ds2.add( 47. );
	ds2.add( 49. );	ds2.add( 51. );	ds2.add( 53. );	ds2.add( 55. );
	ds2.add( 57. );	ds2.add( 59. );	ds2.add( 61. );	ds2.add( 63. );
}
void fill_fake_data3()
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

const char chart1_title[] = "Sample X Line Graph";
const char chart2_title[] = "Sample Y Line Graph";

void init_line_graph()
{
	printf("Line Graph Init\n");
	fill_fake_data1			(	);
	fill_fake_data2			(	);
	fill_fake_data3			(	);	

	lg1.set_title 			( (char*) chart1_title );
	lg1.set_xLabel			( (char*) "Time" 				);
	lg1.set_yLabel			( (char*) "X Degree/second" 	);	
	//lg1.draw_title			(      );
	lg1.set_max				( 64.0 );
	lg1.set_min				( 0.0  );
	
	lg1.add_data_series		( &ds1 							);
	lg1.add_data_series		( &ds3 							);	
	lg1.calc_scale			( 								);
	lg1.set_horizontal_lines( 5 							);
	lg1.set_vertical_lines	( 5 							);

	lg2.set_title 			( (char*) chart2_title 			);
	lg2.set_xLabel			( (char*) "Time" 				);
	lg2.set_yLabel			( (char*) "Y Degree/second" 	);
	lg2.add_data_series		( &ds2 							);
	lg2.set_max				( 64.0 );
	lg2.set_min				( 0.0 );
	lg2.calc_scale			( );
	lg2.set_horizontal_lines( 5		);
	lg2.set_vertical_lines  ( 5 	); 
	
	MainDisplay.remove_all_objects(	);
	MainDisplay.add_object( &lg1 );
	MainDisplay.add_object( &lg2 );
}
void init_histogram_graph()
{
	printf("Histogram Graph Init\n");
	fill_fake_data3			(	);
	
	hg.set_title 			( (char*) chart1_title );
	hg.set_xLabel			( (char*) "Angle" 		);
	hg.set_yLabel			( (char*) "Samples" 	);	
	hg.add_data_series		( &ds3 );

	MainDisplay.remove_all_objects(	);
	MainDisplay.add_object	( &hg );
}
void init_bar_graph	 	()
{
	fill_fake_data3();
	bg.set_title 			( (char*) "Bar Chart" );
	bg.set_xLabel			( (char*) "Month" 		);
	bg.set_yLabel			( (char*) "Apples Sold" 	);	
	bg.add_data_series		( &ds3 );

	MainDisplay.remove_all_objects(	);
	MainDisplay.add_object	( &bg );
}

void init_scatter_graph( )
{
}

void init_combined_graph()
{
	printf("Line Graph Init\n");
	fill_fake_data1			(	);
	fill_fake_data2			(	);
	fill_fake_data3			(	);

	lg1.set_title 			( (char*) chart1_title 			);
	lg1.set_xLabel			( (char*) "Time" 				);
	lg1.set_yLabel			( (char*) "X Degree/second" 	);	
	lg1.set_max				( 64.0 );
	lg1.set_min				( 0.0  );	
	lg1.add_data_series		( &ds1 							);
	lg1.add_data_series		( &ds3 							);	
	lg1.calc_scale			( 								);
	lg1.set_horizontal_lines( 5 							);
	lg1.set_vertical_lines	( 5 							);

	lg2.set_title 			( (char*) chart2_title 			);
	lg2.set_xLabel			( (char*) "Time" 				);
	lg2.set_yLabel			( (char*) "Y Degree/second" 	);
	lg2.add_data_series		( &ds2 							);
	lg2.set_max				( 64.0 );
	lg2.set_min				( 0.0 );
	lg2.calc_scale			( );
	lg2.set_horizontal_lines( 5		);
	lg2.set_vertical_lines  ( 5 	); 
	
	bg.move_to				( 675, 400 );
	bg.set_title 			( (char*) "Bar Chart" );
	bg.set_xLabel			( (char*) "Month" 		);
	bg.set_yLabel			( (char*) "Apples Sold" 	);	
	bg.add_data_series		( &ds3 );

	hg.move_to				( 100,100 );
	hg.set_background_color ( 0xFF202020 );
	hg.set_title 			( (char*) chart1_title );
	hg.set_xLabel			( (char*) "Angle" 		);
	hg.set_yLabel			( (char*) "Samples" 	);	
	hg.add_data_series		( &ds3 );

	MainDisplay.remove_all_objects(	);
	MainDisplay.add_object( &lg1 );
	MainDisplay.add_object( &lg2 );
	MainDisplay.add_object( &bg );
	MainDisplay.add_object( &hg );
}




