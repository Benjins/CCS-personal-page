#include "../header/int/3DUtilities.h"
#include <stdio.h>

Quaternion RotateTowards(const Quaternion& from, const Quaternion& to, float maxDegreesDelta){
	
	maxDegreesDelta *= (3.14159265358979f/180);

	float distance = (from - to).Magnitude();
	float otherDistance = (from + to).Magnitude();
	if(distance < maxDegreesDelta){
		return to;
	}

	if(otherDistance < maxDegreesDelta){
		return to;
	}

	if(otherDistance < distance){
		float weight = maxDegreesDelta/otherDistance;
		return from * (1 - weight) + (to * -weight);
	}
	else{
		float weight = maxDegreesDelta/distance;
		return from * (1 - weight) + to * weight;
	}
}
