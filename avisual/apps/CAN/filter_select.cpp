#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include "VG/openvg.h"
#include "VG/vgu.h"
#include <shapes.h>
#include <fontinfo.h>
#include "Graphbase.hpp"
#include "control.hpp"
#include "button.hpp"
#include "display.h"
#include "filter_select.hpp"


FilterSelector::FilterSelector()
{
	Initialize();
}
void FilterSelector::Initialize()
{
	printf("FILTER SELECTOR::Initialize() called from base constructor!\n\n");
	Window::Initialize();
	
}
int	FilterSelector::calc_metrics()
{
	return 0;
}
void FilterSelector::place_views()
{
	
}

