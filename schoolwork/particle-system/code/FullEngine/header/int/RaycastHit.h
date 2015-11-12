#ifndef RAYCAST_HIT_H
#define RAYCAST_HIT_H

#pragma once

#include "../ext/Vector3.h"
#include "../ext/Vector2.h"

struct Collider;

struct RaycastHit{
	Vector3 worldPos;
	Vector3 normal;
	bool hit;
	Vector2 texCoords;
	float depth;

	Collider* col;

	RaycastHit();
};


#endif