#include "../header/int/AABB.h"
#include "../header/int/Collider.h"
#include "../header/int/Mat4.h" 
#include "../header/int/GameObject.h" 
#include <cfloat>

AABB::AABB(){
	minCorner = Vector3(0,0,0);
	maxCorner = Vector3(0,0,0);
}

AABB::AABB(const AABB& orig){
	minCorner = orig.minCorner;
	maxCorner = orig.maxCorner;
}

AABB::AABB(const Vector3& min, const Vector3& max){
	minCorner = min;
	maxCorner = max;
}

AABB::AABB(const AABB& orig, const Mat4x4& transform){
	Vector3 corners[8] = 
	{
	transform * Vector3(orig.minCorner.x, orig.minCorner.y, orig.minCorner.z),
	transform * Vector3(orig.minCorner.x, orig.minCorner.y, orig.maxCorner.z),
	transform * Vector3(orig.minCorner.x, orig.maxCorner.y, orig.minCorner.z),
	transform * Vector3(orig.minCorner.x, orig.maxCorner.y, orig.maxCorner.z),
	transform * Vector3(orig.maxCorner.x, orig.minCorner.y, orig.minCorner.z),
	transform * Vector3(orig.maxCorner.x, orig.minCorner.y, orig.maxCorner.z),
	transform * Vector3(orig.maxCorner.x, orig.maxCorner.y, orig.minCorner.z),
	transform * Vector3(orig.maxCorner.x, orig.maxCorner.y, orig.maxCorner.z)
	};

	float cornersX[8];
	float cornersY[8];
	float cornersZ[8];

	for(int i = 0; i < 8; i++){
		cornersX[i] = corners[i].x;
		cornersY[i] = corners[i].y;
		cornersZ[i] = corners[i].z;
	}

	minCorner = Vector3(min(cornersX, 8), min(cornersY, 8), min(cornersZ, 8));
	maxCorner = Vector3(max(cornersX, 8), max(cornersY, 8), max(cornersZ, 8));
}

float min(float* array, int size){
	float currentMin = FLT_MAX;
	for(int i = 0; i < size; i++){
		if(array[i] < currentMin){
			currentMin = array[i];
		}
	}

	return currentMin;
}

float max(float* array, int size){
	float currentMax = -FLT_MAX;
	for(int i = 0; i < size; i++){
		if(array[i] > currentMax){
			currentMax = array[i];
		}
	}

	return currentMax;
}

AABB::AABB(const BoxCollider* col, bool globalSpace){
	minCorner = col->position - col->size;
	maxCorner = col->position + col->size;

	if(col->gameObject != NULL && globalSpace){
		minCorner = col->gameObject->transform.LocalToGlobal(minCorner);
		maxCorner = col->gameObject->transform.LocalToGlobal(maxCorner);
	}
}

Collision AABB::CollideWith(const AABB& otherBox) const{
	Collision col;
	col.collide = true;

	col.collide &= minCorner.x <= otherBox.maxCorner.x;
	col.collide &= maxCorner.x >= otherBox.minCorner.x;

	col.collide &= minCorner.y <= otherBox.maxCorner.y;
	col.collide &= maxCorner.y >= otherBox.minCorner.y;

	col.collide &= minCorner.z <= otherBox.maxCorner.z;
	col.collide &= maxCorner.z >= otherBox.minCorner.z;

	return col;
}
