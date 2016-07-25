#ifndef _VIDEO_APP_
#define _VIDEO_APP_


/**** These function handle the menu commands:   ****/

/***** END OF MENU COMMAND FUNCTIONS *****/

void init_Video_app( );


class VideoApp : public Application
{
public:
	VideoApp ();
	VideoApp ( Rectangle* mRect );
	virtual ~VideoApp();

	virtual void 	Initialize		(	);	// create all the objects here.

	virtual void	setup_sidebar	(	);  // derived class adds these here
	virtual void	setup_main_menu ( );
	virtual void	setup_app_menu  ( );
	
	void			About			(	);
	void			Preferences		(	);

	virtual void	file_new		( );
	virtual void	file_open		( );	
	virtual void	file_open_recent( );	
	virtual void	file_save		( );
	virtual void	file_save_as	( );

	void 			zoom_in			();
	void 			zoom_out		();

	// Edit Menu commands - (here for now)
	void			cut					();
	void			copy				();
	void			paste				();
	void			snap_shot			();
	
	// View Menu commands - (here for now)
	void 			show_camera		( );	// CAN Messages view.
	void 			show_video		( );	// Adrenaline Network.
	void 			show_incoming	( );	// Robot sequencer

	void			camera_list			();
	void			local_camera		();
	void			remote_camera		();
	void			movie				();
	void			Incoming			();	
	void			YouTube				();				

	// Settings Menu commands - (here for now)
	void			view_size			();		
	void			video_quality		();	
	void			camera_settings		();
	
	
};

extern VideoApp* video_app;

#endif


