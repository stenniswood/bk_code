



class AI_Player //: public RadarMemory 
{
public:
	AI_Player();
	GridPt  Guess					( BoardMemory* hbm 				);
	GridPt  RandomGuess				( BoardMemory* hbm				);
	GridPt  GuessAfter1Hit			( GridPt mHit, BoardMemory* hbm	);
	int 	countResultingFlips		( byte row, byte col			);
	float 	assessStrategicPosition	( byte row, byte col			);
	float	findHighestScore		( GridPt pts[], int num_grids, float scores[]	);

private:
	GridPt  LastHit;
	bool LastGuessWasHit;
	bool HitsAreHorizontal;
	bool multipleHits  	  ;
	bool endPointFound 	  ;
		
};

