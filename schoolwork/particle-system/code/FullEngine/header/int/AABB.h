#ifndef AABB_H
#define AABB_H

#pragma once

#include "../ext/Vector3.h"

struct Mat4x4;
struct Collision;
struct BoxCollider;

struct AABB{
	Vector3 minCorner;
	Vector3 maxCorner;

	AABB();
	AABB(const AABB& orig);
	AABB(const Vector3& min, const Vector3& max);
	AABB(const AABB& orig, const Mat4x4& transform);
	explicit AABB(const BoxCollider* col, bool globalSpace = true);

	Collision CollideWith(const AABB& otherBox) const;
};

float min(float* array, int size);
float max(float* array, int size);

#endif