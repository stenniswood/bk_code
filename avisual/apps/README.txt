How APPS Work.

1) Derive a class from Application.  
2) Register with the Adrenaline Framework :
	a) Put an init routine in the file created in step 1 Application derivative
	b) Add a call to an  init routine in "avisual_menu.cpp" function 'handle_apps_screens()'

	That function should create an instance of the derived Application class and call 
		MainDisplay.start_app( app );
3) Create a class derived (at least) from Control (better Window). (see draw Drawing2D)  
	in place_views() add each child control with register_child()
	
	

To activate a window, 
	m_main_window = m_board_view;

	The derived class should create the main window in Initialize() and assign it to 
		Applicatiopn::m_main_window
		
		creates all the menus, windows.  Create handlers for each menu item.




