#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#pragma once

#include <list>
#include <cstdlib>
#include "SCTransform.h"

using std::list;

struct Component;
struct Material;
struct Model;
struct Scene;
struct EditorScene;
struct Collider;

struct GameObject{
	friend Scene; //Scene can access non-templated AddComponent
	friend EditorScene;

	private:
	list<Component*> components;
	int _id;

	void AddComponent(Component* comp);
	void AddComponentDirect(Component* comp);
	
	public:
	SC_Transform transform;

	string name;
	
	Material* material;
	Model* mesh;
	Scene* scene;
	
	GameObject();
	explicit GameObject(Scene& sceneRef);
	~GameObject();

	GameObject* Clone() const;
	
	void AddMesh(string fileName);
	void AddMaterial(string matName, string textureName = "");
	void AddNamedMaterial(string matName);

	void OnUpdate();
	void OnCollision(Collider* col);
	
	int GLVertexCount() const;
	Model* GetMesh();
	
	template<typename T>
	T* AddComponent();

	template<typename T>
	T* AddComponent(T* comp);
	
	template<typename T>
	T* GetComponent() const;
};

template<typename T>
T* GameObject::AddComponent(){
	T* component = new T();

	//Ensure that T is of type component
	if(T::isComponent){
		components.push_back(component);
		component->gameObject = this;
		component->OnAwake();
		
		return component;
	}
	else{
		delete component;
		return NULL;
	}
}

template<typename T>
T* GameObject::AddComponent(T* comp){
	//Ensure that T is of type component
	if(T::isComponent){
		components.push_back(comp);
		comp->gameObject = this;
		comp->OnAwake();
		
		return comp;
	}
}

template<typename T>
T* GameObject::GetComponent() const{
	for(auto iter = components.begin(); iter != components.end(); iter++){
		T* cast = dynamic_cast<T*>(*iter);
		if(cast != NULL){
			return cast;
		}
	}
	
	return NULL;
}

#endif