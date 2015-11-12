#ifndef PARTICLE_SYSTEM_H
#define PARTICLE_SYSTEM_H

#include <random>
#include <math.h>
#include "../../3dbasics/Vector3.h"
#include "../../3dbasics/Vector2.h"
#include "Vector4.h"

#pragma once

struct CollisionPlane{
	Vector3 center;
	Vector3 normal;
};

struct Particle{
	Vector3 position;
	Vector3 velocity;
	float size;
	float age;
};

struct SC_Transform;

struct ParticleSystem{
	Particle* particles;
	SC_Transform* transform;
	int maxParticleCount;
	int particleCount;

	float systemLifetime;

	Vector4 startCol;
	Vector4 endCol;

	float startScale;
	float endScale;

	float particlesPerSec;
	Texture* tex;
	float maxLifetime;
	float time;
	float timeSinceParticle;

	float startVelocity;
	float spawnAngle;

	float gravityFactor;

	int seed;

	vector<CollisionPlane> collisionPlanes;

	default_random_engine randomEngine;

	vector<Vector3> positions;
	vector<Vector2> uvs;
	vector<Vector4> colors;

	ParticleSystem(){
		transform = nullptr;
		maxParticleCount = 10000;
		startScale = endScale = 1.0f;
		particleCount = 0;
		spawnAngle = 90;
		startVelocity = 1.0f;
		particles = nullptr;
		gravityFactor = 0.0f;
		systemLifetime = 20.0f;
	}

	void Start(){
		if(particles == nullptr){
			delete[] particles;
		}

		time = 0.0f;
		timeSinceParticle = 0.0f;

		particles = new Particle[maxParticleCount];
		particleCount = 0;
		randomEngine.seed(seed);
	}

	void AddParticle(){
		particles[particleCount].position = Vector3(2,1.5f,1);

		int xi = randomEngine() % 20*1000*1000;
		int zi = randomEngine() % 20*1000*1000;

		double x = xi - 10*1000*1000;
		double y = 1;
		double z = zi - 10*1000*1000;

		int radiusi = randomEngine() % 20*1000*1000;
		double radius = radiusi - 10*1000*1000;
		radius /= 20*1000*1000;

		x /= 20*1000*1000;
		z /= 20*1000*1000;

		float xzRadius = tan(spawnAngle*3.141592653589f/180/2);

		Vector2 xzVelocity = Vector2(x,z);
		xzVelocity.Normalize();
		xzVelocity = xzVelocity * xzRadius * radius;

		x = xzVelocity.x;
		z = xzVelocity.y;



		particles[particleCount].velocity = Vector3(x,y,z).Normalized() * startVelocity;
		particles[particleCount].size = startScale;
		particles[particleCount].age = 0.0f;

		particleCount++;
	}

	void RemoveParticle(int index){
		particles[index] = particles[particleCount-1];
		particleCount--;
	}

	void AddCollisionPlane(const CollisionPlane& plane){
		collisionPlanes.push_back(plane);
	}

	void Update(float deltaTime){
		time += deltaTime;

		for(int i = 0; i < particleCount; i++){
			Vector3 newPos = particles[i].position + particles[i].velocity*deltaTime;

			bool hadCollision = false;
			for(const auto& plane : collisionPlanes){
				Vector3 center = plane.center;
				Vector3 normal = plane.normal;

				float oldOverlap = DotProduct(particles[i].position - center, normal);
				float newOverlap = DotProduct(newPos                - center, normal);
				if(oldOverlap * newOverlap <= 0.0f){
					
					Vector3 invVelocity = particles[i].velocity * -1;
					Vector3 velocityChange = invVelocity - VectorProject(invVelocity, normal);
					Vector3 newVelocity = invVelocity - velocityChange * 2;
					particles[i].velocity = newVelocity;

					hadCollision = true;
					break;
				}
			}

			if(!hadCollision){
				particles[i].position = newPos;
			}

			float lifetimeRatio = particles[i].age / maxLifetime;
			particles[i].size = endScale * lifetimeRatio + startScale * (1 - lifetimeRatio);
			particles[i].velocity = particles[i].velocity + Vector3(0,-9.8f,0) * gravityFactor * deltaTime;
			particles[i].age += deltaTime;

			if(particles[i].age > maxLifetime){
				RemoveParticle(i);
				i--;
			}
		}

		if(time < systemLifetime){
			timeSinceParticle += deltaTime;

			while(timeSinceParticle >= 1/particlesPerSec){
				AddParticle();
				timeSinceParticle -= 1/particlesPerSec;
			}
		}
	}

	void FillGLData(SC_Transform* camera){
		positions.reserve(4*particleCount);
		uvs.reserve(4*particleCount);
		colors.reserve(4*particleCount);

		positions.clear();
		uvs.clear();
		colors.clear();

		Vector3 up = camera->Up();
		Vector3 right = camera->Right();

		for(int i = 0; i < particleCount; i++){
			Vector3 particlePos = particles[i].position;
			
			positions.push_back(particlePos + up*particles[i].size + right*particles[i].size);
			positions.push_back(particlePos + up*particles[i].size - right*particles[i].size);
			positions.push_back(particlePos - up*particles[i].size - right*particles[i].size);
			positions.push_back(particlePos - up*particles[i].size + right*particles[i].size);
			
			uvs.push_back(Vector2(1, 0));
			uvs.push_back(Vector2(0, 0));
			uvs.push_back(Vector2(0, 1));
			uvs.push_back(Vector2(1, 1));

			float lifeRatio = particles[i].age/maxLifetime;
			Vector4 col = startCol * (1 - lifeRatio) + endCol * lifeRatio;
			for(int j = 0; j < 4; j++){
				colors.push_back(col);
			}
		}
	}

	~ParticleSystem(){
		if(particles != nullptr){
			delete[] particles;
		}
	}
};


#endif