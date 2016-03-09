


Legend::Legend( int Left, int Right, int Top, int Bottom )
:Control(Left, Right, Top, Bottom)
{ 
	Initialize();
}
Legend::Legend()
{
	Initialize(); 
}
Legend::~Legend()
{ 
}

void  Legend::Initialize			(					)
{ 
	
}
void Legend::set_data_series		( long mColor, char* mLabel )
{ 
	m_color.push_back( mColor );
	m_names.push_back( mLabel );
}
void  Legend::reset	  	(  )
{
	m_color.clear();
	m_names.clear();
}
// This links the Datasets together!
void  Legend::add_data_series	  	( DataSet* NewData )
{ 
	m_color.push_back( color );
	m_names.push_back( NewData.name );
}
void  Legend::remove_data_series	( int index /*DataSet* OldData*/ )
{ 
	m_color.erase( );
	m_names.erase( );
}
void  Legend::calc_metrics			(					  )
{ 
}
const int LINE_LENGTH = 20;

int   Legend::draw			(	)
{ 
	Control::draw();
	float div = m_names.size();
	if (div==0.0) div = 1.0;
	float y_inc = height / div;
	float y = bottom+height;
	for(int i=0; i<m_names.size(); i++)
	{

		Stroke_l ( m_color[i] );
		Line(left+margin, asf, left+margin+ LINE_LENGTH, bottom);		

		Fill_l   ( m_color[i]  );				
		Text(left+margin+LINE_LENGTH, m_names[i].c_str(), SerifTypeface, text_size );
		y -= y_inc;
	}
}		// Draw calls all the sub draw routines.

