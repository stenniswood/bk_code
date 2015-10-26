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
#include "text_view.hpp"
#include "edit_box.hpp"
#include "label_edit_box.hpp"




LabelEditBox::LabelEditBox()
{ 
	Initialize();
}
void LabelEditBox::Initialize(	)
{ 
	TextView::Initialize();
	
	m_label.set_text("Label", true);
	m_edit.set_text("text...", true);
	
}
int	LabelEditBox::calc_metrics()
{ 
	return 1;
}
int	LabelEditBox::onKey	  ( char mKey )
{ 

	return 1;
}
