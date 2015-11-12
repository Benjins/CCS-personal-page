#include "../header/int/PhysicsSim.h"
#include "../header/int/GameObject.h"
#include "../header/int/SCTransform.h"
#include "../header/int/Mat4.h"
#include "../header/int/Vector4.h"
#include "../header/int/Collider.h"
#include "../header/int/Scene.h"
#include "../header/int/AABB.h"
#include "../header/int/RaycastHit.h"
#include "../header/ext/simple-xml.h"
#include "../header/int/LoadingUtilities.h"
#include <cfloat>


Collision BoxCollider::CollisionWith(const Collider* col) const{
	return col->CollisionWith(this);
}
Collision BoxCollider::CollisionWith(const BoxCollider* col) const{
	return DetectCollision(this, col);
}
Collision BoxCollider::CollisionWith(const SphereCollider* col) const{
	return DetectCollision(col, this);
}

RaycastHit BoxCollider::Raycast(const Vector3& origin, const Vector3& direction){
	return RaycastBox(this, origin, direction);
}

Collider::Collider(){
	gameObject = NULL;
}

BoxCollider::BoxCollider(Vector3 _position, Vector3 _size){
	gameObject = NULL;
	position = _position;
	size = _size;
}

AABB BoxCollider::GetBounds(bool globalSpace) const{
	return AABB(this, globalSpace);
}

void BoxCollider::AddToSim(PhysicsSim* sim){
	sim->staticBoxBodies.push_back(this);
}

void BoxCollider::OnAwake(){
	if(gameObject != NULL && gameObject->scene != NULL){
		AddToSim(gameObject->scene->physicsSim);
	}
}

Component* BoxCollider::Clone(){
	BoxCollider* newBox = new BoxCollider();
	newBox->position = position;
	newBox->size = size;

	return newBox;
}

XMLElement BoxCollider::Serialize(){
	XMLElement elem;
	elem.name = "BoxCollider";
	elem.AddAttribute("position", EncodeVector3(position));
	elem.AddAttribute("size", EncodeVector3(size));

	return elem;
}

void BoxCollider::Deserialize(const XMLElement& elem){
	for(auto iter = elem.attributes.begin(); iter != elem.attributes.end(); iter++){
		XMLAttribute attr = *iter;
		if(attr.name == "position"){
			position = ParseVector3(attr.data);
		}
		else if(attr.name == "size"){
			size = ParseVector3(attr.data);
		}
	}
}

BoxCollider::~BoxCollider(){
	if(gameObject != nullptr && gameObject->scene != nullptr){
		vector<BoxCollider*>& boxBodies = gameObject->scene->physicsSim->staticBoxBodies;
		for(auto iter = boxBodies.begin(); iter != boxBodies.end(); iter++){
			if(*iter == this){
				boxBodies.erase(iter);
				break;
			}
		}
	}
}

SphereCollider::SphereCollider(Vector3 _position, float _radius){
	gameObject = NULL;
	position = _position;
	radius = _radius;
}

Collision SphereCollider::CollisionWith(const Collider* col) const{
	return col->CollisionWith(this);
}
Collision SphereCollider::CollisionWith(const BoxCollider* col) const{
	return DetectCollision(this, col);
}
Collision SphereCollider::CollisionWith(const SphereCollider* col) const{
	return DetectCollision(this, col);
}

RaycastHit SphereCollider::Raycast(const Vector3& origin, const Vector3& direction){
	return RaycastSphere(this, origin, direction);
}

void SphereCollider::AddToSim(PhysicsSim* sim){
	sim->staticSphereBodies.push_back(this);
}

void SphereCollider::OnAwake(){
	if(gameObject->scene != NULL){
		AddToSim(gameObject->scene->physicsSim);
	}
}

Component* SphereCollider::Clone(){
	SphereCollider* newSphere = new SphereCollider();
	newSphere->position = position;
	newSphere->radius = radius;

	return newSphere;
}

XMLElement SphereCollider::Serialize(){
	XMLElement elem;
	elem.name = "SphereCollider";
	elem.AddAttribute("position", EncodeVector3(position));
	elem.AddAttribute("radius", to_string(radius));

	return elem;
}

void SphereCollider::Deserialize(const XMLElement& elem){
	for(auto iter = elem.attributes.begin(); iter != elem.attributes.end(); iter++){
		XMLAttribute attr = *iter;
		if(attr.name == "position"){
			position = ParseVector3(attr.data);
		}
		else if(attr.name == "radius"){
			radius = atof(attr.data.c_str());
		}
	}
}

SphereCollider::~SphereCollider(){
	if(gameObject != nullptr && gameObject->scene != nullptr){
		vector<SphereCollider*>& sphereBodies = gameObject->scene->physicsSim->staticSphereBodies;
		for(auto iter = sphereBodies.begin(); iter != sphereBodies.end(); iter++){
			if(*iter == this){
				sphereBodies.erase(iter);
				break;
			}
		}
	}
}

Collision DetectCollision(const SphereCollider* col1, const SphereCollider* col2){
	float maxDistance = (col1->radius + col2->radius);
	float maxDistanceSqr = maxDistance*maxDistance;

	Vector3 col1TransformedPos = col1->position;
	if(col2 != NULL && col2->gameObject != NULL){
		col1TransformedPos = col2->gameObject->transform.GlobalToLocal(col1->gameObject->transform.LocalToGlobal(col1->position));
	}
	float distanceSqr = (col1TransformedPos - col2->position).MagnitudeSquared();

	Collision collision;
	collision.collide = distanceSqr < maxDistanceSqr;

	return collision;
}

Collision DetectCollision(const SphereCollider* col1, const BoxCollider* col2){
	//Assumptions: No local transforms, and collider's size has no components less than 0.
	//Also, it's really complicated, and not terribly efficient.  Clean up will occur at not 1:30 AM.

	float radiusSqr = col1->radius * col1->radius;

	Vector3 spherePos = col1->position;

	if(col1->gameObject != NULL && col2->gameObject != NULL){
		spherePos = col2->gameObject->transform.GlobalToLocal(col1->gameObject->transform.LocalToGlobal(col1->position));
	}
	
	bool xBoxInter = RangeCheck(col2->position.x - col2->size.x, spherePos.x, col2->position.x + col2->size.x);
	bool yBoxInter = RangeCheck(col2->position.y - col2->size.y, spherePos.y, col2->position.y + col2->size.y);
	bool zBoxInter = RangeCheck(col2->position.z - col2->size.z, spherePos.z, col2->position.z + col2->size.z);

	
	bool xExInter = RangeCheck(col2->position.x - col2->size.x - col1->radius, spherePos.x, col2->position.x + col2->size.x + col1->radius);
	bool yExInter = RangeCheck(col2->position.y - col2->size.y - col1->radius, spherePos.y, col2->position.y + col2->size.y + col1->radius);
	bool zExInter = RangeCheck(col2->position.z - col2->size.z - col1->radius, spherePos.z, col2->position.z + col2->size.z + col1->radius);

	bool withinExtendedX = xExInter && yBoxInter && zBoxInter;
	bool withinExtendedY = xBoxInter && yExInter && zBoxInter;
	bool withinExtendedZ = xBoxInter && yBoxInter && zExInter;

	if(withinExtendedX || withinExtendedY || withinExtendedZ){
		Collision col;
		col.collide = true;
		return col;
	}

	float xEdgeDist = min<float>(abs(col2->position.x - col2->size.x - spherePos.x), abs(col2->position.x + col2->size.x - spherePos.x));
	float yEdgeDist = min<float>(abs(col2->position.y - col2->size.y - spherePos.y), abs(col2->position.y + col2->size.y - spherePos.y));
	float zEdgeDist = min<float>(abs(col2->position.z - col2->size.z - spherePos.z), abs(col2->position.z + col2->size.z - spherePos.z));

	float xEdgeDistSqr = xEdgeDist*xEdgeDist;
	float yEdgeDistSqr = yEdgeDist*yEdgeDist;
	float zEdgeDistSqr = zEdgeDist*zEdgeDist;

	bool withinEdgeXY = xExInter && yExInter && zBoxInter && (xEdgeDistSqr + yEdgeDistSqr < radiusSqr);
	bool withinEdgeXZ = xExInter && yBoxInter && zExInter && (xEdgeDistSqr + zEdgeDistSqr < radiusSqr);
	bool withinEdgeYZ = xBoxInter && yExInter && zExInter && (yEdgeDistSqr + zEdgeDistSqr < radiusSqr);

	if(withinEdgeXY || withinEdgeXZ || withinEdgeYZ){
		Collision col;
		col.collide = true;
		return col;
	}

	bool withinCorner = xExInter && yExInter && zExInter && (xEdgeDistSqr + yEdgeDistSqr + zEdgeDistSqr < radiusSqr);

	if(withinCorner){
		Collision col;
		col.collide = true;
		return col;
	}
	
	Collision col;
	col.collide = false;

	return col;
}

Collision DetectCollision(const BoxCollider* col1, const BoxCollider* col2){
	Mat4x4 col2ToCol1Matrix;
	if(col1->gameObject != NULL){
		col2ToCol1Matrix = col1->gameObject->transform.GlobalToLocalMatrix();
	}

	bool col2HasGameObject = (col2->gameObject != NULL);
	Vector3 right=X_AXIS,up=Y_AXIS,forward=Z_AXIS;
	if(col2HasGameObject){
		SC_Transform col2Trans = col2->gameObject->transform;
		right = col2Trans.Right();
		up = col2Trans.Up();
		forward = col2Trans.Forward();
	}
	 
	Vector3 transformedCol2Axes[3] = { col2ToCol1Matrix * right,
									   col2ToCol1Matrix * up,
									   col2ToCol1Matrix * forward};

	Vector3 testAxes[15];
	//0-8
	for(int i = 0; i < 3; i++){
		Vector3 transformedAxis = transformedCol2Axes[i];
		//Crossed with X, Y, and Z axes
		testAxes[3*i]   = Vector3(0,                   transformedAxis.y, -transformedAxis.x).Normalized();
		testAxes[3*i+1] = Vector3(transformedAxis.x,  -transformedAxis.z,                  0).Normalized();
		testAxes[3*i+2] = Vector3(-transformedAxis.y, 0,                   transformedAxis.z).Normalized();
	}

	//9-11
	for(int i = 0; i < 3; i++){
		testAxes[9+i] = transformedCol2Axes[i].Normalized();
	}

	//12-14
	testAxes[12] = X_AXIS;
	testAxes[13] = Y_AXIS;
	testAxes[14] = Z_AXIS;

	Vector3 col1Min = col1->position - col1->size;
	Vector3 col1Max = col1->position + col1->size;
	Vector3 col2Min = col2->position - col2->size;
	Vector3 col2Max = col2->position + col2->size;

	Mat4x4 col2LocToCol1LocMatrix = col2ToCol1Matrix;
	if(col2HasGameObject){
		col2LocToCol1LocMatrix = col2ToCol1Matrix * col2->gameObject->transform.LocalToGlobalMatrix();
	}

	Vector3 col1Corners[8] = {  Vector3(col1Min.x, col1Min.y, col1Min.z),
								Vector3(col1Min.x, col1Min.y, col1Max.z),
								Vector3(col1Min.x, col1Max.y, col1Min.z),
								Vector3(col1Min.x, col1Max.y, col1Max.z),
								Vector3(col1Max.x, col1Min.y, col1Min.z),
								Vector3(col1Max.x, col1Min.y, col1Max.z),
								Vector3(col1Max.x, col1Max.y, col1Min.z),
								Vector3(col1Max.x, col1Max.y, col1Max.z)};

	Vector3 col2Corners[8] = {  col2LocToCol1LocMatrix * Vector3(col2Min.x, col2Min.y, col2Min.z),
								col2LocToCol1LocMatrix * Vector3(col2Min.x, col2Min.y, col2Max.z),
								col2LocToCol1LocMatrix * Vector3(col2Min.x, col2Max.y, col2Min.z),
								col2LocToCol1LocMatrix * Vector3(col2Min.x, col2Max.y, col2Max.z),
								col2LocToCol1LocMatrix * Vector3(col2Max.x, col2Min.y, col2Min.z),
								col2LocToCol1LocMatrix * Vector3(col2Max.x, col2Min.y, col2Max.z),
								col2LocToCol1LocMatrix * Vector3(col2Max.x, col2Max.y, col2Min.z),
								col2LocToCol1LocMatrix * Vector3(col2Max.x, col2Max.y, col2Max.z)};

	Collision checkDepthCollision;
	checkDepthCollision.collide = true;
	checkDepthCollision.depth = FLT_MAX;

	for(int i = 0; i < 15; i++){
		Vector3 testAxis = testAxes[i];
		Collision potentialCollision = SeparateAxisTheorem(testAxis, col1Corners, col2Corners);
		if(!potentialCollision.collide){
			//cout << "Not collide.\n";
			Collision x;
			x.collide = false;
			return x;
		}
		else{
			//Find the MVT by finding the axis with the smallest intersection
			if(potentialCollision.depth < checkDepthCollision.depth){
				//cout << "Changed depth from: " << checkDepthCollision.depth << " to: " << potentialCollision.depth << endl;
				checkDepthCollision.depth = potentialCollision.depth;
				Mat4x4 col1ToGlobal;
				if(col1->gameObject != nullptr){
					col1ToGlobal = col1->gameObject->transform.LocalToGlobalMatrix();
					col1ToGlobal.SetColumn(3, Vector4(0,0,0,1));
				}
				checkDepthCollision.normal = col1ToGlobal * potentialCollision.normal;
			}
		}
	}

	return checkDepthCollision;
}

Collision SeparateAxisTheorem(Vector3 axis, Vector3* points1, Vector3* points2){
	float point1Min = FLT_MAX;
	float point1Max = -FLT_MAX;

	float point2Min = FLT_MAX;
	float point2Max = -FLT_MAX;

	for(int i = 0; i < 8; i++){
		float projection1 = DotProduct(axis, points1[i]);
		point1Max = max(point1Max,projection1);
		point1Min = min(point1Min,projection1);
		
		float projection2 = DotProduct(axis, points2[i]);
		point2Max = max(point2Max,projection2);
		point2Min = min(point2Min,projection2);	
	}

	if(point2Min < point1Max && point2Max > point1Min){
		float maxMin = max(point1Min, point2Min);
		float minMax = min(point1Max, point2Max);

		Collision x;
		x.collide = true;
		x.normal = axis; //TODO: Convert to global coords
		x.normal = x.normal * (point2Min > point1Min ? -1 : 1);
		x.depth = minMax - maxMin;
		return x;
	}

	Collision x;
	x.collide = false;
	return x;
}

Collision DetectCollision(const BoxCollider* col1, const SphereCollider* col2){
	return DetectCollision(col2, col1);
}

//Check that mid is less than min and greater than max
bool RangeCheck(float min, float mid, float max){
	return min <= mid && mid <= max;
}

//Don't use these; pure virtual declaration caused errors in MSVC.
Collision Collider::CollisionWith(const Collider* col) const{Collision x; return x;}
Collision Collider::CollisionWith(const BoxCollider* col) const{Collision x; return x;}
Collision Collider::CollisionWith(const SphereCollider* col) const{Collision x; return x;}
void Collider::AddToSim(PhysicsSim* sim){}
RaycastHit Collider::Raycast(const Vector3& origin, const Vector3& direction){RaycastHit x; return x;}
void Collider::OnAwake(){}
Collider::~Collider(){}
