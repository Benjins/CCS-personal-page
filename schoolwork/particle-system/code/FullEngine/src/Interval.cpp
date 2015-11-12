#include "../header/int/Interval.h"

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))


Interval::Interval(float _min, float _max, bool enforceConstraint){
	if(enforceConstraint){
		min = MIN(_min, _max);
		max = MAX(_min, _max);
	}
	else{
		min = _min;
		max = _max;
	}
}

Interval::Interval(const Interval& orig){
	min = orig.min;
	max = orig.max;
}

Interval Interval::Intersection(const Interval& other) const{
	return Interval(MAX(min, other.min), MIN(max, other.max));
}

bool Interval::IsValid() const{
	return min <= max;
}
