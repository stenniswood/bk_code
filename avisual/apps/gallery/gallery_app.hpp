#ifndef _GALLERY_APP_
#define _GALLERY_APP_


/**** These function handle the menu commands:   ****/
void draw_zoom_in			();
void draw_zoom_out			();


/***** END OF MENU COMMAND FUNCTIONS *****/

void init_gallery_app( );


class GalleryApp : public Application
{
public:
	GalleryApp ();
	GalleryApp ( Rectangle* mRect );
	virtual ~GalleryApp();


	virtual void 	Initialize		(	);	// create all the objects here.

	virtual void	setup_main_menu	(	);  // The horizontal menu. derived class adds these here
	virtual void	setup_app_menu  ( );
	
	void				About			(	);
	void				Preferences		(	);
	//int				Quit			(	);	

	virtual void	file_new		( );
	virtual void	file_open		( );	
	virtual void	file_open_recent( );	
	virtual void	file_save		( );
	virtual void	file_save_as	( );

	void 			show_grid		( );

	void 			zoom_in			( );
	void 			zoom_out		( );
	
	ImageGallery 	m_gallery;
};

extern GalleryApp* gallery_app;

#endif


