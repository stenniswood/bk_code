#ifndef _ADREN_NETWORK_VIEW_H_
#define _ADREN_NETWORK_VIEW_H_

#include "icon.hpp"
#include "tabular_listbox.hpp"
#include "board_list_oop.hpp"


using namespace std;
//extern char const* e_card_names[];

struct stBoardInfo2 {
	byte model;
	byte instance;
	byte status;			// failure, working okay, etc.
	byte Software_Major;	// Firmware version
	byte Software_Minor;
	byte Hardware_Revision;
	word Manufacturer;
	long int BKSerialNumber;
	char Description[80];
};

struct stBoardInfoText {
	char model   [10];
	char instance[10];
	char status  [10];			// failure, working okay, etc.
	char Software_Version[10];	// Firmware version
	char Hardware_Revision[10];
	char Manufacturer[30];
	char BKSerialNumber[8];
	char Description[128];
};

const int VIEW_MODE_LIST 		= 1;
const int VIEW_MODE_GRAPHICAL 	= 2;

/*
	Graphical View will be in a different class.
	This is tab list only!	
	
*/

class NetworkView : public TabularListBox
{
public:
	NetworkView (  );
	NetworkView ( int Left, int Right, int Top, int Bottom );
	NetworkView ( int Width, int Height 				   );
	~NetworkView(  );

	virtual void 	Initialize  (	);
	int				calc_metrics(	);

	//int			set_view_mode ( int mViewMode ); 
 	int				setup_headers	( );
	int				add_board 		( struct stBoardInfo2* mBInfo );
	int				handle_incoming_msg		( struct sCAN* msg );
	
	int				formulate_line	( struct stBoardInfoText* mTxt );

	struct stBoardInfoText* convert_to_text( struct stBoardInfo2* mInfo );

	void		reset();
	int		populate_with_boards(	);	
	
	
	int	draw_board		( );
	int	draw_board_info	( );
	virtual int	draw  	( );

protected:
	vector<struct stBoardInfo2>		m_board_info;
	vector<struct stBoardInfoText>	m_board_info_text;
	int				m_view_mode;
	
};


void can_parse_board_description_msg( sCAN* mMsg, struct stBoardInfo* mOut );
void can_parse_serial_number_msg ( sCAN* mMsg, struct stBoardInfo* mOut );
void can_parse_board_revision_msg( sCAN* mMsg, struct stBoardInfo* mOut );
void can_parse_board_presence_msg( sCAN* mMsg, struct stBoardInfo* mOut );


#endif
