// Steve Tenniswood
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "VG/openvg.h"
#include "VG/vgu.h"
#include <shapes.h>
#include <fontinfo.h>
#include "tk_colors.hpp"


Colors::Colors(byte Red, byte Green, byte Blue)
{
	combo = 0xFF000000;
	combo |= (Red<<16);
	combo |= (Green<<8);
	combo |= (Blue);
	
}

long int	Colors::get_color(				)
{
	return combo;
}

byte		Colors::get_alpha(				)
{
	return (combo & 0xFF000000)>>24;
}

void		Colors::set_alpha( byte mAlpha )
{
	combo &= 0x00FFFFFF;
	combo |= (mAlpha<<24);
}
