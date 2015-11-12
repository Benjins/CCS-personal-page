#include "../header/int/Scene.h"
#include "../header/int/Mat4.h"
#include "../header/int/Material.h"
#include "../header/int/Texture.h"
#include "../header/int/Vector4.h"
#include "../header/int/RaycastHit.h"
#include "../header/int/RigidBody.h"
#include "../header/int/Collider.h"
#include "../header/int/PhysicsSim.h"
#include "../header/int/Armature.h"
#include "../header/int/CubeMap.h"
#include "../header/int/FontBMPMaker.h"
#include "../header/ext/simple-xml.h"
#include <time.h>
#include <cstdlib>
#include <algorithm>

#if !defined(__APPLE__) && !defined(_WIN32) && !defined(_WIN64)
#include <sys/time.h>
#endif 

#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/freeglut.h>
#endif

#include "../header/int/UserComps.h"

Scene::Scene(){
}

Scene::Scene(int argc, char** argv){
	camera = NULL;
	physicsSim = new PhysicsSim();
	pathfinding.scene = this;

	skyBox = nullptr;

	myRandom = default_random_engine(time(NULL));

	audio.Initialise();

	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA|GLUT_DEPTH|GLUT_ALPHA);
    glutInitWindowSize(1280, 720);
    glutInitWindowPosition(500, 100);
    glutCreateWindow("my-engine");

#ifndef __APPLE__
	GLenum res = glewInit();
    if (res != GLEW_OK) {
      fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
    }
#endif

	glutMouseFunc(OnMouseFunc);
	glutPassiveMotionFunc(OnPassiveMouseFunc);
	glutMotionFunc(OnPassiveMouseFunc);
	glutKeyboardFunc(OnKeyFunc);
	glutKeyboardUpFunc(OnKeyUpFunc);
	
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    
	//glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	//glDepthFunc(GL_LESS);
	
	glutSetKeyRepeat(GLUT_KEY_REPEAT_OFF);

	xRot = 0;
	yRot = 0;

    printf("GL version: %s\n", glGetString(GL_VERSION));
    printf("GL vendor: %s\n", glGetString(GL_VENDOR));
	printf("GL Renderer: %s\n", glGetString(GL_RENDERER));
}

void Scene::Init(){
}

GameObject* Scene::FindGameObject(const string& name){
	for(auto iter = objects.begin(); iter != objects.end(); iter++){
		GameObject* obj = *iter;
		if(obj->name == name){
			return obj;
		}
	}

	return NULL;
}

GameObject* Scene::AddObject(GameObject* obj){
	objects.push_back(obj);
	obj->scene = this;

	if(obj->material != NULL && obj->mesh != NULL){
		drawCalls.push_back(DrawCall(obj));
	}

	return obj;
}

GameObject* Scene::AddObjectAndDescendants(GameObject* obj){
	AddObject(obj);
	for(SC_Transform* child : obj->transform.children){
		AddObjectAndDescendants(child->gameObject);
	}

	return obj;
}

void Scene::RemoveObject(GameObject* obj){
	if(std::find(destroyedObjects.begin(), destroyedObjects.end(), obj) != destroyedObjects.end()){
		//cout << "\n\nWarning: Tried to remove the same gameobject twice.  Subsequent calls ignored.\n";
	}
	else{
		destroyedObjects.push_back(obj);
	}
}

void Scene::DestroyObject(GameObject* obj){
	//We remove the child from its parent, which causes issue if we do that while iterating over the children list
	vector<GameObject*> childrenCopy; 
	childrenCopy.reserve(obj->transform.children.size());
	for(SC_Transform* child : obj->transform.children){
		childrenCopy.push_back(child->gameObject);
	}

	//Remove parent's reference to us
	obj->transform.SetParent(nullptr);

	for(GameObject* child : childrenCopy){
		DestroyObject(child);
	}

	for(auto iter = objects.begin(); iter != objects.end(); iter++){
		if(*iter == obj){
			objects.erase(iter);
			delete obj;
			break;
		}
	}

	for(auto iter = drawCalls.begin(); iter != drawCalls.end(); iter++){
		if((*iter).obj == obj){
			drawCalls.erase(iter);
			break;
		}
	}
}

int Scene::AddLight(){
	int newId = 0;
	if(lights.size() > 0){
		newId = lights[lights.size() - 1].id + 1;
	}

	Light newLight = {};
	newLight.id = newId;
	lights.push_back(newLight);

	return newId;
}

void Scene::Start(){
	timer.Reset();

	prevDeltaTimeIndex = 0;
	for(int i = 0; i < FPS_SMOOTHING_FACTOR; i++){
		prevDeltaTime[i] = 0.0f;
	}

	ParticleSystem sys;
	sys.maxLifetime = 15.4f;
	sys.maxParticleCount = 10000;
	sys.particlesPerSec = 30.0f;
	sys.startScale = 0.3f;
	sys.endScale = 0.1f;
	sys.gravityFactor = 0.2f;
	sys.spawnAngle = 90;
	sys.startVelocity = 2.2f;

	CollisionPlane plane1 = {Vector3(0.0f, 0.1f, 0.0f), Vector3(0,1.0f,0)};
	CollisionPlane plane2 = {Vector3(0.0f, 1.6f, 0.4f), Vector3(0,0.6f,0.8f)};
	//CollisionPlane plane2 = {Vector3(0.0f, 0.7f, 1.2f), Vector3(0, 0.6f,0.8f)};
	//CollisionPlane plane3 = {Vector3(0.0f, 1.4f, 2.1f), Vector3(0,-0.6f,0.8f)};
	sys.AddCollisionPlane(plane1);
	sys.AddCollisionPlane(plane2);
	//sys.AddCollisionPlane(plane3);

	sys.startCol = Vector4(1.0f, 0.9f, 0.1f, 1.0f);
	sys.endCol   = Vector4(0.8f, 0.2f, 0.1f, 0.0f);

	particles.push_back(sys);
	particles[0].Start();

	running = true;
	while(running){
		clock_t start = clock();
#ifdef __APPLE__
		glutCheckLoop();
#else
		glutMainLoopEvent();
#endif
		clock_t afterCheck = clock();

		double loopEventTime = ((double)afterCheck - start)/CLOCKS_PER_SEC;

		Render();

		clock_t afterRender = clock();
		double renderTime = ((double)afterRender - afterCheck)/CLOCKS_PER_SEC;

		OnUpdate();
		physicsSim->Advance(deltaTime);

		clock_t afterUpdate = clock();

		double updateTime = ((double)afterUpdate - afterRender)/CLOCKS_PER_SEC;

		//glFinish();

		clock_t afterFinish = clock();
		double finishTime = ((double)afterFinish - afterUpdate)/CLOCKS_PER_SEC;

		glutSwapBuffers();

		clock_t afterSwap = clock();
		double swapTime = ((double)afterSwap - afterFinish)/CLOCKS_PER_SEC;

		double total = ((double)afterSwap - start)/CLOCKS_PER_SEC;

		double totalMSToWait = ((1.0f/60) - deltaTime)*1000;
#if defined(_WIN32) || defined(_WIN64)
		int msToWait = (int)(totalMSToWait + 0.5f);
		if(msToWait > 0){
			//printf("Waiting     %d ms.\n", msToWait);
			//Sleep(msToWait);
		} 
		else{
			//printf("Not waiting %d ms.\n", msToWait);
		}
#endif
		//printf("update time total: %.1f\n", deltaTime * 1000);

		GLenum err = glGetError();
		if(err != 0){
			printf("GL Error: %d, meaning: '%s'\n", err, gluErrorString(err));
		}

		input.EndFrame();
	}
}

void Scene::OnUpdate(){
	deltaTime = timer.GetTimeSince();
	timer.Reset();

	prevDeltaTime[prevDeltaTimeIndex] = deltaTime;
	prevDeltaTimeIndex = (prevDeltaTimeIndex + 1) % FPS_SMOOTHING_FACTOR;

	float smoothedFPS = 0.0f;
	for(int i = 0; i < FPS_SMOOTHING_FACTOR; i++){
		smoothedFPS = smoothedFPS + prevDeltaTime[i];
	}

	smoothedFPS /= FPS_SMOOTHING_FACTOR;

	GuiText* fpsText = static_cast<GuiText*>(guiSystem.FindGUIElement("fpsText"));
	if(fpsText != nullptr && deltaTime > 0){
		string fps = "FPS: " + to_string((int)((1/smoothedFPS) + 0.5f));
		fpsText->text = fps;
	}

	audio.SetListenerPos(camera->GlobalPosition());

	for(auto iter = objects.begin(); iter != objects.end(); iter++){
		(*iter)->OnUpdate();
	}

	for(auto& sys : particles){
		sys.Update(deltaTime);
	}

	for(GameObject* obj : spawnedObjects){
		AddObjectAndDescendants(obj);
	}

	spawnedObjects.clear();

	for(GameObject* obj : destroyedObjects){
		DestroyObject(obj);
	}

	destroyedObjects.clear();

	if(input.GetKeyUp('o')){
		SaveScene("Quicksave.xml");
	}
	if(input.GetKeyUp('l')){
		LoadScene("Quicksave.xml");
	}

	if(input.GetKey('x')){
		Stop();
	}
}

void PhysicsUpdate(){
	
}

void Scene::OnPostLoad(){
	//FindGameObject("reticle")->material->SetVec4Uniform("_color", Vector4(0.0f, 0.0f, 1.0f, 1.0f));
	pathfinding.HookUpNodes();
}

void Scene::Render(){
	float aspectRatio = (float)glutGet(GLUT_WINDOW_WIDTH) / (float)glutGet(GLUT_WINDOW_HEIGHT);
	float fieldOfView = 90;
	float nearZ = 0.01;
	float farZ = 1000;
	                   
	glDepthMask(GL_TRUE);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	Mat4x4 perspMatrix = GetPerspectiveMatrix(aspectRatio,fieldOfView, nearZ, farZ);

	Mat4x4 camMatrix = camera->GetCameraMatrix();

	glViewport(0, 0, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
	glEnable(GL_DEPTH_TEST);
	
	glDisable(GL_BLEND);

	Vector3 lightVectors[6];
	GLint isDirectional[6];

	for(int i = 0; i < lights.size(); i++){
		lightVectors[i] = lights[i].isDirectional ? lights[i].direction : lights[i].position;
		isDirectional[i] = lights[i].isDirectional ? 1 : 0;
	}
	
	Vector3 cameraPos = camera->GlobalPosition();
	Vector3 cameraForward = camera->Forward();
	for(auto iter = drawCalls.begin(); iter != drawCalls.end(); iter++){
		Vector3 objPos = iter->obj->transform.GlobalPosition();
		

		//if(DotProduct(objPos - cameraPos, cameraForward) > 0){
			glUseProgram(iter->obj->material->shaderProgram);

			Material* mat = iter->obj->material;
			mat->SetMat4Uniform("_perspMatrix", perspMatrix);
			mat->SetMat4Uniform("_cameraMatrix", camMatrix);

			glUniform1f(mat->GetUniformByName("numLights"), lights.size());
			glUniform3fv(mat->GetUniformByName("lightVectors"), lights.size(), (GLfloat*)lightVectors);
			glUniform1iv(mat->GetUniformByName("lightIsDirectional"), lights.size(), isDirectional);

			GLint cubeMapLoc = mat->GetUniformByName("_cubeMap");
			if(cubeMapLoc != -1){
				skyBox->Bind(GL_TEXTURE2);
				glUniform1i(cubeMapLoc, 2);

				mat->SetVec3Uniform("_cameraPos", cameraPos);
			}

			iter->Draw();	
		//}
	}

	if(skyBox != nullptr){
		skyBox->Render(camMatrix, perspMatrix);
	}

	Material* colMat = resources.GetMaterialByName("color");
	glUseProgram(colMat->shaderProgram);
	colMat->SetMat4Uniform("_perspMatrix", perspMatrix);
	colMat->SetMat4Uniform("_cameraMatrix", camMatrix);
	colMat->SetVec4Uniform("_color", Vector4(0.8f, 0.8f, 0.8f, 1.0f));

#if 0
	glBegin(GL_QUADS);
	for(ParticleSystem& sys : particles){
		for(CollisionPlane& plane : sys.collisionPlanes){
			Vector3 temp = plane.normal;
			float tempX = temp.x;
			temp.x = -temp.y;
			temp.y = tempX;

			Vector3 left = CrossProduct(plane.normal, temp) * 5;
			Vector3 up = CrossProduct(plane.normal, left) * 5;

			Vector3 verts[4] = {plane.center + left + up,
								plane.center - left + up,
								plane.center - left - up,
								plane.center + left - up};
			for(int i = 0; i < 4; i++){
				glVertex3f(verts[i].x, verts[i].y, verts[i].z);
			}
		}
	}
	glEnd();
#endif

	Material* particleMat = resources.GetMaterialByName("particle");
	glUseProgram(particleMat->shaderProgram);
	GLint texLoc = particleMat->GetUniformByName("_particleTex"); 
	glUniform1i(texLoc, 0);

	particleMat->SetMat4Uniform("_perspMatrix", perspMatrix);
	particleMat->SetMat4Uniform("_cameraMatrix", camMatrix);

	GLint particleAttribLocs[3] = {
		glGetAttribLocation(particleMat->shaderProgram, "Position"), 
		glGetAttribLocation(particleMat->shaderProgram, "UV"),
		glGetAttribLocation(particleMat->shaderProgram, "col")
	};

	int dataSizes[3] = {3, 2, 4};
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glDepthMask(GL_FALSE);

	for(auto& particleSystem : particles){
		particleSystem.FillGLData(camera);

		if(particleSystem.transform != nullptr){
			particleMat->SetMat4Uniform("_objectMatrix", particleSystem.transform->LocalToGlobalMatrix());
		}
		else{
			particleMat->SetMat4Uniform("_objectMatrix", Mat4x4());
		}

		if(particleSystem.positions.size() > 0){
			particleMat->mainTexture->Bind(GL_TEXTURE0);
			//particleSystem.tex->Bind(GL_TEXTURE0);

			void*  bufferData[3]  = {particleSystem.positions.data(), particleSystem.uvs.data(), particleSystem.colors.data()};
			size_t bufferSize = particleSystem.positions.size();

			GLuint buffers[3];
			glGenBuffers(3, buffers);

			for(int i = 0 ; i< 3; i++){
				glEnableVertexAttribArray(particleAttribLocs[i]);
				glBindBuffer(GL_ARRAY_BUFFER, buffers[i]);
				glBufferData(GL_ARRAY_BUFFER, dataSizes[i]*4*bufferSize, bufferData[i], GL_DYNAMIC_DRAW);
				glVertexAttribPointer(particleAttribLocs[i], dataSizes[i], GL_FLOAT, GL_FALSE, 0, 0);
			}

			glDrawArrays(GL_QUADS, 0, bufferSize);

			for(int i = 0; i < 3; i++){
				glDisableVertexAttribArray(particleAttribLocs[i]);
			}

			glDeleteBuffers(3, buffers);
		}
	}
	
	//Gui and gizmos stuff 
	glDisable(GL_DEPTH_TEST);

	//Gui stuff
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	guiSystem.RenderGui();
}

void Scene::OnMouse(int button, int state, int x, int y){
	if(state == GLUT_DOWN){
		input.PushMouse(button);
	}
	else if(state == GLUT_UP){
		input.ReleaseMouse(button);
	}
}

void Scene::OnPassiveMouse(int x, int y){
	input.mouseX = x;
	input.mouseY = y;
}

void Scene::OnKey(unsigned char key, int x, int y){
	input.PushKey(key);
}

void Scene::OnKeyUp(unsigned char key, int x, int y){
	input.ReleaseKey(key);
}

GameObject* Scene::FindPrefab(const string& name){
	for(GameObject* prefab : prefabs){
		if(prefab->name == name){
			return prefab;
		}
	}

	return nullptr;
}

GameObject* Scene::Instantiate(GameObject* obj, Vector3 position, Quaternion rotation /*= QUAT_IDENTITY*/){
	GameObject* instance = obj->Clone();

	instance->transform.position = position;
	instance->transform.rotation = rotation;

	spawnedObjects.push_back(instance);
	return instance;
}

void Scene::RemoveAllObjects(){
	for(auto iter = objects.begin(); iter != objects.end(); iter++){
		delete (*iter);
	}

	objects.clear();

	for(auto iter = prefabs.begin(); iter != prefabs.end(); iter++){
		delete (*iter);
	}

	prefabs.clear();

	for(auto iter = guiSystem.elements.begin(); iter != guiSystem.elements.end(); iter++){
		delete (*iter);
	}

	guiSystem.elements.clear();

	physicsSim->staticBoxBodies.clear();
	physicsSim->staticSphereBodies.clear();
	for(auto iter = physicsSim->dynamicBodies.begin(); iter != physicsSim->dynamicBodies.end(); iter++){
		delete *iter;
	}
	physicsSim->dynamicBodies.clear();

	drawCalls.clear();

	resources.Clear();

	audio.ClearClips();

	if(skyBox != nullptr){
		delete skyBox;
		skyBox = nullptr;
	}
}

void Scene::Stop(){
	running = false;
}

Scene::~Scene(){
	for(auto iter = objects.begin(); iter != objects.end(); iter++){
		delete (*iter);
	}

	for(GameObject* prefab : prefabs){
		delete prefab;
	}

	delete physicsSim;

	if(skyBox != nullptr){
		delete skyBox;
		skyBox = nullptr;
	}

	//delete testArmature;
}

static void RenderScene(){
}

static void OnMouseFunc(int button, int state, int x, int y){
	Scene::getInstance().OnMouse(button, state, x, y);
}

static void OnPassiveMouseFunc(int x, int y){
	Scene::getInstance().OnPassiveMouse(x, y);
}

static void OnKeyFunc(unsigned char key, int x, int y){
	Scene::getInstance().OnKey(key, x, y);
}

Mat4x4 GetPerspectiveMatrix(float aspectRatio, float fieldOfView, float nearZ, float farZ){
	Mat4x4 persp;

	float fieldOfView_Rad = fieldOfView/180*3.14159265f;
	float tanHalfFOV = tanf(fieldOfView_Rad/2);
	float zRange = nearZ - farZ;

	float x1 = 1/aspectRatio/tanHalfFOV;
	float y2 = 1/tanHalfFOV;
	float z3 = (-nearZ - farZ)/zRange;
	float z4 = 2*farZ*nearZ/zRange;
	
	persp.SetRow(0, Vector4(x1,0,0,0));
	persp.SetRow(1, Vector4(0,y2,0,0));
	persp.SetRow(2, Vector4(0,0,z3,z4));
	persp.SetRow(3, Vector4(0,0,1,0));
	
	return persp;
}

static void OnKeyUpFunc(unsigned char key, int x, int y){
	Scene::getInstance().OnKeyUp(key, x, y);
}

