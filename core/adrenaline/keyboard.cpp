// line graph OpenVG program
// Steve Tenniswood
#include <shapes.h>
#include <fontinfo.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string>
#include <vector>

#include "VG/openvg.h"
#include "VG/vgu.h"
#include <shapes.h>
#include <fontinfo.h>
#include "CAN_Interface.h"

#include "dataset.hpp"
#include "display.h"
#include "adrenaline_windows.h"
#include "adrenaline_graphs.h"
#include "display_manager.hpp"
#include "client_memory.hpp"
#include "client_list_control.hpp"
#include "keyboard.hpp"


Keyboard::Keyboard()
{

}

Keyboard::~Keyboard()
{

}
#define KEY_WIDTH  50
#define KEY_HEIGHT 75
#define KEY_SPACING_COL 64
#define KEY_SPACING_ROW 110

void	Keyboard::initialize_keys()
{
	struct stKey key;
	key.width  = KEY_WIDTH;
	key.height = KEY_HEIGHT;

	// ROW #1 : ^ ZXCVBNM 
	char array1[] = { 'Q','W','E','R','T','Y','U','I','O','P' };	
	key.x = 5;
	key.y= 110;
	for (int col=0; col<10; col++)
	{
		key.x += KEY_SPACING_COL;
		key.text[0] = array1[col];
		m_keys.push_back(key);
	}

	// ROW #2 : asdfghjkl
	char array2[] = { 'A','S','D','F','G','H','J','K','L' };	
	key.x = 5+KEY_SPACING_COL/2;
	key.y += KEY_SPACING_ROW;
	for (int col=0; col<9; col++)
	{
		key.x += KEY_SPACING_COL;
		key.text[0] = array1[col];
		m_keys.push_back(key);
	}

	// ROW #3 : qwertyuiop
	char array3[] = { 'Z','X','C','V','B','N','M' };	
	key.x  = 5+KEY_SPACING_COL/2+KEY_SPACING_COL;
	key.y += KEY_SPACING_ROW;
	for (int col=0; col<7; col++)
	{
		key.x += KEY_SPACING_COL;
		key.text[0] = array1[col];
		m_keys.push_back(key);
	}	
}

void Keyboard::Initialize()
{
	initialize_keys();
}

int Keyboard::draw()
{
	int size = m_keys.size();
	
	for (int i=0; i<size; i++)
	{
		// Draw a filled rectangle:
		
	//	m_keys[i]
		// Print the letter:
	}
}


Control* Keyboard::HitTest( int x, int y )
{

}

int Keyboard::onClick(int x, int y, bool mouse_is_down)
{
	// Disperse to affected child:

	return FALSE;
}
