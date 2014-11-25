package games.multiplayer.othello;

import java.util.Random;

public class AI_Player extends RadarMemory 
{
	Random generator = null;
	GridPt  LastHit;
	boolean LastGuessWasHit;
	boolean HitsAreHorizontal = false;
	boolean multipleHits  = false;
	boolean endPointFound = false;

	public AI_Player() {
		super();
		generator = new Random();
	}

	public GridPt Guess( BoardMemory hbm )
	{
		//byte result = 0;
		GridPt g = RandomGuess(hbm);
		if (multipleHits)
		{			
			// evaluate on human players board
			//result = hbm.EvaluateGuess( g.row, g.col );

		} else if (LastGuessWasHit) 
		{
		   g = GuessAfter1Hit(LastHit, hbm);
		   //result = hbm.EvaluateGuess( g.row, g.col );
		    
		} else {
			//result = hbm.EvaluateGuess( g.row, g.col );
			
		}
		return g;
	}
	/***
	 * 
	 * @param result
	 * @param mg
	 * @param hbm
	 * @return  -1 Not Sunk, ShipIndex if sunk.
	 */
	public GridPt RandomGuess(BoardMemory hbm)
	{
		GridPt  gp = new GridPt();
		gp.row = (byte) generator.nextInt(10);
		gp.col = (byte) generator.nextInt(10);
		if ((BoardMemory.board[gp.row][gp.col] & 0x0F)>0)		// if already guessed
		{
			gp.row = (byte)generator.nextInt(10);	
			gp.col = (byte)generator.nextInt(10);
		}
		// This guess has to go to the oponent view
		return gp;
	}
	/***
	 * 
	 * @param mHit
	 * @param hbm		human's board memory
	 * @return
	 */
	public GridPt GuessAfter1Hit(GridPt mHit, BoardMemory hbm)
	{
		byte possible = 0x00;
		GridPt g      = new GridPt();
		g.row = mHit.row;
		g.col = mHit.col;
		boolean valid = false;
		byte dir 	  ;
		while ((!valid) && (possible<0xFF))
		{
			valid = true;
			g.row = mHit.row;
			g.col = mHit.col;
			dir   = (byte)generator.nextInt(4);
			switch (dir)
			{
				case 0:	g.col = (byte) (mHit.col-1);	possible |= 0x01;	break;
				case 1: g.col = (byte) (mHit.col+1);	possible |= 0x02;	break;
				case 2:	g.row = (byte) (mHit.row-1);	possible |= 0x04;	break;
				case 3:	g.row = (byte) (mHit.row+1);	possible |= 0x08;	break;
			}
			if (g.col<0) valid = false;
			else if (g.row<0) valid = false;
			else if (g.col>9) valid = false;
			else if (g.row>9) valid = false;
			else if ((BoardMemory.board[g.row][g.col]&0x0F)>0) valid = false;
		}
		return g;
	}

	public int countResultingFlips(byte row, byte col)
	{
		return 1;
		
	}	
	/***
	 * Certain positions on the board are considered stategic.  ie. Corners worth most,
	 * Edges worth more,  1 row in from edges considered less.
	 * @param row
	 * @param col
	 * @return
	 */
	public float assessStrategicPosition(byte row, byte col)
	{
		if ((row==0) || (row==MAX_Y))
		{
			if ((col==0) || (col==MAX_X))
				return +2;		// corner
			else
				return +1;
		} 
		if ((col==0) || (col==MAX_X))
			return +1;		
		return 0;
	}	
	
	public float findHighestScore(GridPt[] pts, float[] scores)
	{
		float max_score = 0;
		int   max_score_index = -1;
		for (int i=0; i<pts.length; i++)
			if (scores[i]>max_score)
				max_score_index = i;
		
		return scores[max_score_index];
	}
	/***
	 *   Here the orientation of the ship is already known.
	 *   Further guesses will be along that direction until the ship is sunk.
	 *   Obviously the endpoints may not be exactly known.  So misses still 
	 *   are possible.
	 *   
	 *   Note ships could be placed side by side and therefore this would not
	 *   result in a sunk ship.  The method will work anyway however, getting
	 *   as many ships as are placed along that direction.
	 */
	// What's held in AI_Player bm?
	//   Computer Ship Locations
	//   Human players guesses
	// What's held in the human player bm?
	//	 Computer's guesses
	//	 Humans ships
}

