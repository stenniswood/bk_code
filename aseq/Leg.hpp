


class Leg
{
public:
	Leg  ();
	void timeslice();
	void setPositionVector();
	void ComputeDutyVector();
		
private:
	Pivot* Hip;
	Pivot* Knee;
	Pivot* Ankle;

}