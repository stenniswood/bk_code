How APPS Work.

1) Derive a class from Application.  
2) Add a call to an  init routine in "avisual_menu.cpp" function 'handle_apps_screens()'
	Put the init routine in the same file you derived the application (step 1)
	That functin should create an instance of the derived class and if okay, call register_with_display_manager()

To activate a window, 
	m_main_window = m_board_view;

This class creates all the menus, windows.  Create handlers for each menu item.




