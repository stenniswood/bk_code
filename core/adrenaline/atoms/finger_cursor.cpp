
#include "finger_cursor.hpp"




FingerCursor::FingerCursor()
{
}

FingerCursor::~FingerCursor()
{
}
	
int FingerCursor::draw				(	)
{
	//vgDrawPath		(path, VG_STROKE_PATH			);	
	return 0;
}	

void FingerCursor::compose_path()
{
/*
	// To Answer the question - is there any fill in a font:
	StrokeWidth	( 1.0 		 );
	Stroke_l	( 0xFFFF00FF );
	Fill_l		( 0xFF0000FF );
	//Text( x, y, "Sample Large thick", SerifTypeface, 42 );
	VGubyte * commands = new VGubyte[10];
	VGfloat * coords   = new VGfloat[20];
	VGint numCmds		= 10;
	VGint numCoords 	= 10;
	VGPath path = vgCreatePath(VG_PATH_FORMAT_STANDARD,
								VG_PATH_DATATYPE_F,
								1.0f, 0.0f, 		// scale,bias
								numCmds, numCoords,
								VG_PATH_CAPABILITY_ALL);
	commands[0] = VG_MOVE_TO_ABS;
	commands[1] = VG_LINE_TO_ABS;
	commands[2] = VG_LINE_TO_ABS;
	commands[3] = VG_LINE_TO_ABS;
	commands[4] = VG_LINE_TO_ABS;
	commands[5] = VG_LINE_TO_ABS;
	commands[6] = VG_LINE_TO_ABS;
	commands[7] = VG_LINE_TO_ABS;
	commands[8] = VG_LINE_TO_ABS;
	commands[9] = VG_LINE_TO_ABS;
	commands[10]= VG_LINE_TO_ABS;
	
	coords[0]   = 12.0;		coords[1] = 24.0;
	coords[2]   = 100.0;	coords[3] = 350.0;
	coords[4]   = 150.0;	coords[5] = 400.0;
	coords[6]   = 200.0;	coords[7] = 500.0;
	coords[8]   = 300.0;	coords[9] = 350.0;
	coords[10]  = 125.0;	coords[11] =200.0;
	coords[12]  = 900.0;	coords[13] =250.0;
	coords[14]  = 1080.0;	coords[15] =200.0;
	coords[16]  = 500.0;	coords[17] =100.0;
	coords[18]  = 12.0;		coords[19] =24.0;

	vgAppendPathData(path, numCmds, commands, coords);
	vgDrawPath		(path, VG_STROKE_PATH			);
*/
}
