#ifndef SCENE_H
#define SCENE_H

#pragma once

#include <list>
#include "GameObject.h"
#include "SCTransform.h"
#include "Drawcall.h"
#include "Input.h"
#include "ResourceManager.h"
#include "GuiSystem.h"
#include "Pathfinding.h"
#include "Light.h"
#include "AudioSystem.h"
#include "Timer.h"
#include "ParticleSystem.h"
#include <random>

using std::list;

struct RigidBody;
struct PhysicsSim;
struct XMLElement;
struct Armature;
struct CubeMap;

#define FPS_SMOOTHING_FACTOR 30

struct Scene{
	SC_Transform* camera;
	float deltaTime;

	PhysicsSim* physicsSim;
	Input input;
	MaterialManager resources;
	Pathfinding pathfinding;
	AudioSystem audio;
	GuiSystem guiSystem;
	vector<Light> lights;
	vector<ParticleSystem> particles;

	Armature* testArmature;

	CubeMap* skyBox;

protected:
	vector<GameObject*> prefabs;
	vector<GameObject*> objects;
	vector<DrawCall> drawCalls;

	vector<GameObject*> spawnedObjects;
	vector<GameObject*> destroyedObjects;

	void DestroyObject(GameObject* obj);

	int prevX;
	int prevY;
	int buttonDown;
	float xRot;
	float yRot;

	float prevDeltaTime[FPS_SMOOTHING_FACTOR];
	int prevDeltaTimeIndex;

	bool running;

	Timer timer;
public:
	
	//NEVER call this version before the parameterized version
	static Scene& getInstance(){
		return getInstance(0,NULL);
	}
	static Scene& getInstance(int argc, char** argv){
		static Scene instance = Scene(argc, argv);
		return instance;
	}

	Scene();
	Scene(int argc, char** argv);

	int AddLight();

protected:
	//Hide this, because it'll break things
	Scene(const Scene& orig){}

public:

	virtual void Init();

	void LoadScene(string fileName);
	void SaveScene(string fileName);

	GameObject* AddObject(GameObject* obj);
	GameObject* AddObjectAndDescendants(GameObject* obj);

	void RemoveObject(GameObject* obj);

	
	virtual void Start();

	void UpdateVertexBuffer();
	
	void OnUpdate();

	void OnPostLoad();

	void Render();
	void PhysicsUpdate();

	GameObject* FindGameObject(const string& name);

	void OnMouse(int button, int state, int x, int y);
	void OnPassiveMouse(int x, int y);
	void OnKey(unsigned char key, int x, int y);
	void OnKeyUp(unsigned char key, int x, int y);

	void RemoveAllObjects();

	GameObject* FindPrefab(const string& name);
	GameObject* Instantiate(GameObject* obj, Vector3 position, Quaternion rotation = QUAT_IDENTITY);

	void Stop();

	GameObject* LoadGameObjectXML(const XMLElement& elem, bool fireAwakeEvents = true);
	void LoadSkyBoxXML(const XMLElement& res);
	XMLElement SaveSkyBoxXML();

	virtual ~Scene();
};

void ProcessAssetFile(const string& assetFileName);

static std::default_random_engine myRandom;

static void RenderScene();

static void OnMouseFunc(int button, int state, int x, int y);

static void OnPassiveMouseFunc(int x, int y);

static void OnKeyFunc(unsigned char key, int x, int y);

Mat4x4 GetPerspectiveMatrix(float aspectRatio, float fieldOfView, float nearZ, float farZ);

static void OnKeyUpFunc(unsigned char key, int x, int y);

#endif
