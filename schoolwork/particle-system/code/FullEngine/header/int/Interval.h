#ifndef INTERVAL_H
#define INTERVAL_H

#pragma once

#include <cfloat>

struct Interval{
	float min;
	float max;

	Interval(float _min = -(FLT_MAX), float _max = FLT_MAX, bool enforceConstraint = false);
	Interval(const Interval& orig);

	Interval Intersection(const Interval& other) const;

	bool IsValid() const;
};

#endif