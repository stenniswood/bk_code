

class DraggerGesture 
{
public: 
	DraggerGesture();

	void	recognize_enlarge ( );			// 2 fingers distance between
	void	recognize_rotation( );			// 2 fingers change angles
	void	recognize_three_up( );			// 2 finger left or right
	void	recognize_next_screen( );		// 2 finger left or right
	void	recognize_gesture( );

	void	add_event();
	
	vector<touch_event>	m_events;
};

