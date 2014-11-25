
enum card_names = {
"1_H.jpg",
"2_H.jpg",
"3_H.jpg",
"4_H.jpg",
"5_H.jpg",
"6_H.jpg",
"7_H.jpg",
"8_H.jpg",
"9_H.jpg",
"10_H.jpg",
"J_H.jpg",
"Q_H.jpg",
"K_H.jpg",
"A_H.jpg",

"1_C.jpg",
"2_C.jpg",
"3_C.jpg",
"4_C.jpg",
"5_C.jpg",
"6_C.jpg",
"7_C.jpg",
"8_C.jpg",
"9_C.jpg",
"10_C.jpg",
"J_C.jpg",
"Q_C.jpg",
"K_C.jpg",
"A_C.jpg",

"1_D.jpg",
"2_D.jpg",
"3_D.jpg",
"4_D.jpg",
"5_D.jpg",
"6_D.jpg",
"7_D.jpg",
"8_D.jpg",
"9_D.jpg",
"10_D.jpg",
"J_D.jpg",
"Q_D.jpg",
"K_D.jpg",
"A_D.jpg",

"1_S.jpg",
"2_S.jpg",
"3_S.jpg",
"4_S.jpg",
"5_S.jpg",
"6_S.jpg",
"7_S.jpg",
"8_S.jpg",
"9_S.jpg",
"10_S.jpg",
"J_S.jpg",
"Q_S.jpg",
"K_S.jpg",
"A_S.jpg"
}


class Card : public Control
{
public:
	/* Value:  [1..10,J,Q,K,A]
	   Suit :  [C,C,S,D]  { Hearts, Clubs, Spades, Diamonds }	*/
	Card	( string mFilename, char Value, char suit 	);
	Card	( int  mIndex	);
		
	char	get_card();
	char	get_suit();
	
	void	draw();
	
	
protected:
	void	load_resource();
	
private:
	char	value;
	char	suit;	
	
	Image	image;
	
};

