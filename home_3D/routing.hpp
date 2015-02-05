

struct Location 
{
	float x;
	float y;
};


// The problem to be solved:  (how to get from x to y)
extern struct Location Start;
extern struct Location End;



void find_next_way_point(struct Location mLatest);