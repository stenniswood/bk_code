


Legend::Legend( int Left, int Right, int Top, int Bottom )
:Control(Left, Right, Top, Bottom)
{ 
}
Legend::Legend()
{ 
}
Legend::~Legend()
{ 
}

void  Legend::Initialize			(					)
{ 
}
void Legend::set_data_series		( long mColor, char* mLabel )
{ 
}

void  Legend::calc_metrics			(					  )
{ 
}

// This links the Datasets together!
void  Legend::add_data_series	  	( DataSet* NewData )
{ 
}
void  Legend::remove_data_series	( int index /*DataSet* OldData*/ )
{ 
}

int   Legend::draw			(	)
{ 

}		// Draw calls all the sub draw routines.
