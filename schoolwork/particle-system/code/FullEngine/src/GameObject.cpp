#include "../header/int/GameObject.h"
#include "../header/int/Component.h"
#include "../header/int/Model.h"
#include "../header/int/Armature.h"
#include "../header/int/Material.h"
#include "../header/int/Collider.h"
#include "../header/int/Scene.h"

#ifdef TESTING
#if defined(_WIN32) || defined(_WIN64)
#  define _CRTDBG_MAP_ALLOC
#  define _CRTDBG_MAP_ALLOC_NEW
#  include <crtdbg.h>
#  include <assert.h>
#endif
#endif

GameObject::GameObject(){
	scene = NULL;
	mesh = NULL;
	material = NULL;
	transform.gameObject = this;
}

GameObject::GameObject(Scene& sceneRef){
	scene = &sceneRef;
}

void GameObject::AddComponent(Component* comp){
	components.push_back(comp);
	comp->gameObject = this;
	comp->OnAwake();
}

void GameObject::AddComponentDirect(Component* comp){
	components.push_back(comp);
	comp->gameObject = this;
}

void GameObject::AddMesh(string fileName){
	if(mesh != NULL){
		delete[] mesh;
	}
	mesh = new Model(fileName);
	mesh->gameObject = this;
}

GameObject* GameObject::Clone() const{
	GameObject* newObj = new GameObject();
	newObj->transform.scale = transform.scale;
	newObj->transform.rotation = transform.rotation;
	newObj->transform.SetParent(nullptr);
	newObj->scene = scene;

	for(SC_Transform* child : transform.children){
		GameObject* newChild = child->gameObject->Clone();
		newChild->transform.scale = child->scale;
		newChild->transform.rotation = child->rotation;
		newChild->transform.position = child->position;
		newChild->transform.SetParent(&newObj->transform);
	}

	newObj->name = name + "(clone)";
	newObj->mesh = mesh;
	newObj->material = material;
	for(Component* comp : components){
		newObj->AddComponent(comp->Clone());
	}

	return newObj;
}

void GameObject::AddMaterial(string matName, string textureName){
	material = scene->resources.LoadMaterial(matName + textureName, matName, textureName);
}

void GameObject::AddNamedMaterial(string matName){
	material = scene->resources.GetMaterialByName(matName);
}

void GameObject::OnUpdate(){
	//cout << "About to update components." << endl;
	for(auto iter = components.begin(); iter != components.end(); iter++){
		(*iter)->OnUpdate();
	}

	if(mesh != nullptr){
		Armature* armature = mesh->armature;
		if(armature != nullptr){
			armature->Update(scene->deltaTime);
		}
	}

	//cout << "Done updating components." << endl;
}

void GameObject::OnCollision(Collider* col){
	//cout << "Gameobject::OnCollision(), on: " << name << ", with: " << col->gameObject->name << endl;

	for(auto iter = components.begin(); iter != components.end(); iter++){
		(*iter)->OnCollision(col);
	}

	if(transform.GetParent() != NULL && transform.GetParent()->gameObject != NULL){
		transform.GetParent()->gameObject->OnCollision(col);
	}
}


int GameObject::GLVertexCount() const{
	if(mesh == NULL){
		return 0;
	}
	else{
		return mesh->GLVertexCount();
	}
}

GameObject::~GameObject(){
	for(auto iter = components.begin(); iter != components.end(); iter++){
		Component* comp = *iter;
		delete comp;
	}

	/*
	if(mesh != NULL){
		delete mesh;
	}
	*/
	
	//The material is managed by MaterialManager, so we don't delete it.

}