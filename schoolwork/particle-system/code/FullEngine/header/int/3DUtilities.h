#ifndef _3DUTILITIES_H
#define _3DUTILITIES_H

#pragma once

#include "../ext/Quaternion.h"

/*
These functions should probably be folded into 3dbasics at some point.  But they're here for now.
*/

Quaternion RotateTowards(const Quaternion& from, const Quaternion& to, float maxDegreesDelta);

#endif