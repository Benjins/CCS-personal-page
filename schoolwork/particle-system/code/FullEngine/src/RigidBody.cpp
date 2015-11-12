#include "../header/int/RigidBody.h"
#include "../header/int/SCTransform.h"
#include "../header/int/Collider.h"
#include "../header/int/GameObject.h"
#include "../header/int/Scene.h"
#include "../header/int/PhysicsSim.h"
#include "../header/ext/simple-xml.h"


RigidBody::RigidBody(SC_Transform* _transform, Collider* _col, float _mass){
	transform = _transform;
	col = _col;
	col->gameObject  = transform->gameObject;
	transform->gameObject->AddComponent<Collider>(col);
	transform->gameObject->scene->physicsSim->AddRigidBody(this);
	state.position = transform->position;

	isKinematic = false;

	mass = _mass;
	state.invMass = 1/_mass;
}

void RigidBody::AddForce(Vector3 force){
	state.force = state.force + force;
}

void RigidBody::Translate(const Vector3& move){
	state.position = state.position + move;
}

void RigidBody::OnAwake(){
	transform = &gameObject->transform;
	col = gameObject->GetComponent<Collider>();
	gameObject->scene->physicsSim->AddRigidBody(this);

	state.position = transform->position;
}

XMLElement RigidBody::Serialize(){
	XMLElement elem;
	elem.name = "RigidBody";
	elem.attributes.emplace_back("mass", to_string(mass));
	elem.attributes.emplace_back("isKinematic", (isKinematic ? "T" : "F"));

	return elem;
}

void RigidBody::Deserialize(const XMLElement& elem){
	for(const XMLAttribute& attr : elem.attributes){
		if(attr.name == "mass"){
			mass = atof(attr.data.c_str());
			state.invMass = 1/mass;
		}
		else if(attr.name == "isKinematic"){
			isKinematic = attr.data == "T";
		}
	}

	cout << "Rigidbody " << (isKinematic ? "is" : " isn't") << " kinematic.\n";
}

void RigidBody::StepForward(float deltaTime){
	RBDeriv a,b,c,d,initial;

	initial.instantAcceleration = initial.instantVelocity = Vector3(0,0,0);

	a = Evaluate(state, 0, initial);
	b = Evaluate(state, deltaTime/2, a);
	c = Evaluate(state, deltaTime/2, b);
	d = Evaluate(state, deltaTime,   c);

	Vector3 dxdt = (a.instantVelocity + b.instantVelocity * 2 
				  + c.instantVelocity * 2 + d.instantVelocity) * 1.0f/6;
	Vector3 dvdt = (a.instantAcceleration + b.instantAcceleration * 2 
				  + c.instantAcceleration * 2 + d.instantAcceleration) * 1.0f/6;

	deriv.instantVelocity = dxdt;
	deriv.instantAcceleration = dvdt;

	state.position = state.position + dxdt * deltaTime;
	transform->position = state.position;
	state.velocity = state.velocity + dvdt * deltaTime + Vector3(0, -10.0f, 0) * deltaTime;
	state.force = Vector3(0,0,0);
}

RBDeriv Evaluate(const RBState& init, float dt, const RBDeriv& d){
	RBDeriv deriv;
	deriv.instantVelocity = init.velocity + d.instantAcceleration * dt;
	deriv.instantAcceleration = init.force*init.invMass*dt;

	return deriv;
}

RigidBody::~RigidBody(){
	PhysicsSim* sim = gameObject->scene->physicsSim;
	for(auto iter = sim->dynamicBodies.begin(); iter != sim->dynamicBodies.end(); iter++){
		if(*iter == this){
			sim->dynamicBodies.erase(iter);
			break;
		}
	}
}