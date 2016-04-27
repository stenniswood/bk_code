


class FuzzyFloat 
{
public:
	FuzzyFloat();
	FuzzyFloat(float mInitValue);
	
	FuzzyFloat operator+(FuzzyFloat mValue);
	FuzzyFloat operator-(FuzzyFloat mValue);
	FuzzyFloat operator*(FuzzyFloat mValue);
	FuzzyFloat operator/(FuzzyFloat mValue);
	
	FuzzyFloat operator<(FuzzyFloat mValue);
	FuzzyFloat operator>(FuzzyFloat mValue);
	FuzzyFloat operator==(FuzzyFloat mValue);	// fuzzy compare.
	
	
	float	Value;
	float 	Uncertainty;		// ie standard deviation.

};