

struct stServoAnglesSet pick( float mApproach, float mX, float mY, float mZ )
{
	struct stServoAnglesSet as;
	
	uint8_t result = INV_XYZ_To_Angles(struct stFull_XYZ_Coordinate* XYZ,    
						&as );
	return as;
}
void pick_gripped( float mApproach, float mX, float mY, float mZ )
{

}

void pick_raised( float mApproach, float mX, float mY, float mZ )
{

}

/* mX,mY,mZ are the placed point */
void pick_moved( float mApproach, float mX, float mY, float mZ )
{

}

void place( float mApproach, float mX, float mY, float mZ )
{

}

void place_released( float mApproach, float mX, float mY, float mZ )
{

}


 void pick_n_place_state( stXYZ_Coordinate Pick, stXYZ_Coordinate Place )
{
	switch( current_state )
	{
	case 0: pick(); break;
	case 1: pick_gripped(); break;
	case 2: pick_raised();  break;
	case 3: pick_moved();   break;
	case 4: place();            break;
	case 5: place_released();   break;		
	default: break;
	}
	
}

