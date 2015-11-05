#ifndef _RESOURCE_MANAGER_HPP_
#define _RESOURCE_MANAGER_HPP_


#include "button.hpp"
#include "control.hpp"


/*
An icon bar:
	Icon names added programatically or - 
		specifying a resource group name.
	
	responsible for loading all icons.
	
	
Need to figure out how we'll define the images per application basis.
But for now we'll do all images in the resource/icons/ directory.
*/

class ResourceManager : public Control
{
public: 
	ResourceManager();

	void		load_icon_directory( string mPath );
	void		load_sound_clips( string mPath );
	//void		load_video_clips();
	
	
	// ICONS :
	vector<string>				m_image_names;		// file name only. (no path)
	vector<struct image_info>	m_images_info;
	vector<VGImage> 			m_images;
	vector< int >				m_icon_id;

};


#endif

