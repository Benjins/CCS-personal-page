#include "../header/int/PhysicsSim.h"
#include "../header/int/Collider.h"
#include "../header/int/RigidBody.h"
#include "../header/int/GameObject.h"
#include "../header/int/SCTransform.h"
#include "../header/int/Interval.h"
#include "../header/int/RaycastHit.h"
#include "../header/int/Mat4.h"
#include "../header/int/Vector4.h"
#include <string>

PhysicsSim::PhysicsSim(float _deltaTime){
	timeStep = _deltaTime;
	accumulatedTime = 0.0f;
}

void PhysicsSim::AddStaticCollider(Collider* col){
	for(auto iter = dynamicBodies.begin(); iter != dynamicBodies.end(); iter++){
		if((*iter)->col == col){
			return;
		}
	}
	col->AddToSim(this);
}

void PhysicsSim::AddRigidBody(RigidBody* rb){
	dynamicBodies.push_back(rb);

	for(auto iter = staticBoxBodies.begin(); iter != staticBoxBodies.end(); iter++){
		if(*iter == rb->col){
			staticBoxBodies.erase(iter);
			return;
		}
	}

	for(auto iter = staticSphereBodies.begin(); iter != staticSphereBodies.end(); iter++){
		if(*iter == rb->col){
			staticSphereBodies.erase(iter);
			return;
		}
	}
}

void PhysicsSim::Advance(float dt){
	accumulatedTime += dt;

	while(accumulatedTime >= timeStep){
		StepForward();
		accumulatedTime -= timeStep;
	}
}

void PhysicsSim::StepForward(){
	for(auto iter = dynamicBodies.begin(); iter != dynamicBodies.end(); iter++){
		RigidBody* rb = *iter;
		if(rb->isKinematic){
			rb->StepForward(timeStep);
		}
		for(auto iter2 = iter; iter2 != dynamicBodies.end(); iter2++){
			RigidBody* rb2 = *iter2;
			if(rb == rb2){
				continue;
			}

			Collision collision = rb->col->CollisionWith(rb2->col);
			if(collision.collide){
				GameObject* obj1 = rb->col->gameObject;
				GameObject* obj2 = rb2->col->gameObject;

				Vector3 normalProj = VectorProject(rb->state.velocity * -1, collision.normal) * -1;
				Vector3 newVelocity = normalProj + (normalProj - rb->state.velocity);

				rb->state.velocity = newVelocity * -0.6f;
				rb->state.position = rb->state.position + collision.normal * collision.depth * 1.001f;

				if(rb->state.velocity.MagnitudeSquared() < 0.3f){
					rb->state.velocity = Vector3(0,0,0);
				}

				obj1->OnCollision(rb2->col);
				obj2->OnCollision(rb->col);
			}
		}
		for(auto iter2 = staticBoxBodies.begin(); iter2 != staticBoxBodies.end(); iter2++){
			if((*iter)->col == *iter2){
				continue;
			}

			Collision collision = (*iter)->col->CollisionWith(*iter2);
			if(collision.collide){
				GameObject* obj1 = rb->col->gameObject;
				GameObject* obj2 = (*iter2)->gameObject;

				Vector3 normalProj = VectorProject(rb->state.velocity * -1, collision.normal) * -1;
				Vector3 newVelocity = normalProj + (normalProj - rb->state.velocity);

				rb->state.velocity = newVelocity * -0.6f;
				rb->state.position = rb->state.position + collision.normal * collision.depth * 1.001f;
				//printf("Col normal: (%.3f, %.3f, %.3f)\n", collision.normal.x, collision.normal.y, collision.normal.z);

				if(rb->state.velocity.MagnitudeSquared() < 0.3f){
					rb->state.velocity = Vector3(0,0,0);
				}

				obj1->OnCollision(*iter2);
				obj2->OnCollision(rb->col);
			}
		}
	}
}

void PhysicsSim::ResetSelectionBoxColliders(){
	for(BoxCollider* col : staticBoxBodies){
		//HACK: This entire function.
		//We free it, because the destructor tries to remove it from the physicsSim.
		//Sometimes I hate my choices.
		free(col);
	}
	staticBoxBodies.clear();
}

RaycastHit PhysicsSim::Raycast(Vector3 origin, Vector3 direction){
	direction.Normalize();
	RaycastHit finalHit;
	finalHit.hit = false;
	finalHit.depth = FLT_MAX;

	for(auto iter = dynamicBodies.begin(); iter != dynamicBodies.end(); iter++){
		RaycastHit hit = (*iter)->col->Raycast(origin, direction);
		if(hit.hit && hit.depth < finalHit.depth){
			finalHit = hit;
		}
	}

	for(auto iter = staticBoxBodies.begin(); iter != staticBoxBodies.end(); iter++){
		RaycastHit hit = RaycastBox(*iter, origin, direction);
		if(hit.hit && hit.depth < finalHit.depth){
			finalHit = hit;
		}
	}

	for(auto iter = staticSphereBodies.begin(); iter != staticSphereBodies.end(); iter++){
		RaycastHit hit = RaycastSphere(*iter, origin, direction);
		if(hit.hit && hit.depth < finalHit.depth){
			finalHit = hit;
		}
	}

	return finalHit;
}

RaycastHit RaycastSphere(SphereCollider* col, Vector3 origin, Vector3 direction){
	SC_Transform trans = col->gameObject->transform;
	Vector3 transformedOrigin = trans.GlobalToLocal(origin);
	Vector3 transformedDirection = trans.GlobalToLocal(direction) - trans.GlobalToLocal(Vector3(0,0,0));
	transformedDirection = transformedDirection.Normalized();
	
	Vector3 originToCenter = col->position - transformedOrigin;
	Vector3 originToCenterProjectedOntoDir = VectorProject(originToCenter, transformedDirection);
	
	Vector3 checkVec = (transformedOrigin + originToCenterProjectedOntoDir - col->position);
	float distance = checkVec.Magnitude();

	if(distance > col->radius){
		RaycastHit hit;
		hit.hit = false;
		return hit;
	}
	
	float backOutDist = sqrt((col->radius * col->radius) - (distance * distance));
	Vector3 hitLocation = transformedOrigin + originToCenterProjectedOntoDir - (transformedDirection * backOutDist);

	float depth = (hitLocation - transformedOrigin).Magnitude();

	RaycastHit hit;
	hit.hit = true;
	hit.depth = depth;
	hit.worldPos = origin + direction*hit.depth;
	hit.hit = true;
	hit.col = col;
	hit.normal = (hitLocation - col->position).Normalized();
	return hit;
}

RaycastHit RaycastBox(BoxCollider* col, Vector3 origin, Vector3 direction){
	Vector3 colMin = col->position - col->size;
	Vector3 colMax = col->position + col->size;

	SC_Transform trans = col->gameObject->transform;
	Vector3 transformedOrigin = trans.GlobalToLocal(origin);
	Vector3 transformedDirection = trans.GlobalToLocal(direction) - trans.GlobalToLocal(Vector3(0,0,0));

	Vector3 gameObjectScale = col->gameObject->transform.scale;
	Vector3 corner1 = colMin - transformedOrigin;
	Vector3 corner2 = colMax - transformedOrigin;

	Vector3 diffMin = Vector3(min(corner1.x, corner2.x),
							  min(corner1.y, corner2.y),
							  min(corner1.z, corner2.z));
	Vector3 diffMax = Vector3(max(corner1.x, corner2.x),
							  max(corner1.y, corner2.y),
							  max(corner1.z, corner2.z));

	Interval xInter = Interval(diffMin.x / transformedDirection.x, diffMax.x / transformedDirection.x, true);
	Interval yInter = Interval(diffMin.y / transformedDirection.y, diffMax.y / transformedDirection.y, true);
	
	Interval zInter = Interval(diffMin.z / transformedDirection.z, diffMax.z / transformedDirection.z, true);

	Interval yzInter = yInter.Intersection(zInter);

	Interval lineInterval = xInter.Intersection(yzInter);

	if(lineInterval.IsValid() && lineInterval.min > 0){
		RaycastHit x;
		x.depth = lineInterval.min;
		x.worldPos = origin + direction*x.depth;
		x.hit = true;
		x.col = col;

		Vector3 transformedHit = (transformedOrigin + transformedDirection * x.depth) - col->position;
		transformedHit = transformedHit.Scaled(Vector3(1 / col->size.x, 1 / col->size.y, 1 / col->size.z));

		Vector3 normalLocal = transformedHit;
		normalLocal.x = (1 - (abs(normalLocal.x)) <= 0.00001f ? 1 : 0) * (normalLocal.x < 0 ? -1 : 1);
		normalLocal.y = (1 - (abs(normalLocal.y)) <= 0.00001f ? 1 : 0) * (normalLocal.y < 0 ? -1 : 1);
		normalLocal.z = (1 - (abs(normalLocal.z)) <= 0.00001f ? 1 : 0) * (normalLocal.z < 0 ? -1 : 1);

		if(normalLocal.MagnitudeSquared() == 0){
			int xaa = 0;
		}

		x.normal = Rotate(normalLocal, trans.TotalRotation()).Normalized();
		return x;
	}

	RaycastHit x;
	x.hit = false;
	return x;
}

PhysicsSim::~PhysicsSim(){
	//We don't actually destroy the static colliders or rigidbodies, since they're components, 
	//and so the gameobject they're attached to will handle them.
}

