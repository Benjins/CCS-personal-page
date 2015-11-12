#ifndef RIGIDBODY_H
#define RIGIDBODY_H

#pragma once

#include "../ext/3dbasics.h"
#include "Component.h"

struct SC_Transform;
struct Collider;
struct XMLElement;

struct RBState{
	Vector3 position;
	Vector3 velocity;
	Vector3 force;
	float invMass;

	float t;
};

struct RBDeriv{
	Vector3 instantVelocity;
	Vector3 instantAcceleration;
};

struct RigidBody : Component{
	float mass;
	bool useGravity;
	bool isKinematic;
	float time;

	SC_Transform* transform;

	Collider* col;

	RBState state;
	RBDeriv deriv;

	RigidBody(){
		transform = nullptr;
		col = nullptr;
		time = 0;
		useGravity = false;
		isKinematic = false;
		mass = 1;
		state.invMass = 1/mass;
	}

	virtual Component* Clone() override{
		RigidBody* rb = new RigidBody();

		rb->isKinematic = isKinematic;
		rb->mass = mass;

		return rb;
	}

	virtual void OnAwake();
	virtual XMLElement Serialize();
	virtual void Deserialize(const XMLElement& elem);

	RigidBody(SC_Transform* _transform, Collider* _col, float m_ass = 1.0f);

	void AddForce(Vector3 force);

	void Translate(const Vector3& move);

	void StepForward(float deltaTime);

	~RigidBody();

};

RBDeriv Evaluate(const RBState& init, float dt, const RBDeriv& d);





#endif