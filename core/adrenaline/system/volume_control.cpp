/*************************************************************************
NAME		:  volume_control.cpp
DESCRIPTION	:  


DATE	: March 20, 2016
AUTHOR	: Steve Tenniswood
**************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "VG/openvg.h"
#include "VG/vgu.h"
#include <shapes.h>
#include <fontinfo.h>
#include "CAN_Interface.h"
#include "adrenaline_windows.h"
#include "adrenaline_graphs.h"
#include "display.h"
#include "display_manager.hpp"
#include "frame_window.hpp"
#include <vector>
#include "power_level.hpp"
#include "stereo_power.hpp"
//#include "visual_memory.h"
#include "audio_memory.h"
#include "fft.h"
#include "audio_app.hpp"
#include "can_app.hpp"
#include "card_games.h"

#include "slider.hpp"


static Window		VolumeControl(450, 1050, 500, 100);
StereoPowerLevels	spl;
SliderControl		volume; 
SliderControl		pan;

void init_volume_control()
{
	VolumeControl.pack_control		( &spl,    PACK_FILL_PARENT, PACKER_ALIGN_TOP  	);
	VolumeControl.pack_control      ( &volume, PACK_FILL_PARENT, PACK_RIGHT  		);
	VolumeControl.pack_control      ( &pan,    PACK_FILL_PARENT, PACKER_ALIGN_BOTTOM );

	MainDisplay.remove_all_objects(		);
	MainDisplay.add_object( &VolumeControl );
}

