
#include "horizontal_menu.hpp"
#include "vertical_menu.hpp"


extern HorizontalMenu system_hmenu;
void init_system_hmenu( );

int  show_atom_screens (void* menuPtr, int mMenuIndex );
void init_atom_menu();

int  show_molecule_screens( void* menuPtr, int mMenuIndex );
void init_molecule_menu();

int  show_game_screens( void* menuPtr, int mMenuIndex );
void init_game_menu();

int  show_graph_screens( void* menuPtr, int mMenuIndex );
void init_graph_menu();

void init_default_sidebar( SideBar* mSB );

