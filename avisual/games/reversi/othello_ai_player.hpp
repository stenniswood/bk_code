


class AI_Player extends RadarMemory 
{
public:
	AI_Player();
	GridPt  Guess					( BoardMemory hbm 				);
	GridPt  RandomGuess				( BoardMemory hbm				);
	GridPt  GuessAfter1Hit			( GridPt mHit, BoardMemory hbm	);
	int 	countResultingFlips		( byte row, byte col			);
	float 	assessStrategicPosition	( byte row, byte col			);
	float	findHighestScore		( GridPt[] pts, float[] scores	);


private:
	Random generator = null;
	GridPt  LastHit;
	boolean LastGuessWasHit;
	boolean HitsAreHorizontal = false;
	boolean multipleHits  = false;
	boolean endPointFound = false;
	
	
}
