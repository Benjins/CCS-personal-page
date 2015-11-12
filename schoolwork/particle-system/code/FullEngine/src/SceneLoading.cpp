#include "../header/int/Material.h"
#include "../header/int/Texture.h"
#include "../header/int/ResourceManager.h"
#include "../header/int/Collider.h"
#include "../header/int/Scene.h"
#include "../header/int/GuiElement.h"
#include "../header/int/UserComps.h"
#include "../header/int/RigidBody.h"
#include "../header/int/CubeMap.h"
#include "../header/int/Armature.h"
#include "../header/ext/simple-xml.h"
#include "../header/int/Component.h"
#include "../gen/MetaTypes.h"

struct FireGun;

#define DEFINE_USER_COMPONENT(compName) else if(name == #compName){compName* newComp = new compName (); newComp->metaType = metaStruct_##compName; return newComp;}

Component* GetUserDefinedComp(const string& name){
	if(name == ""){
		return nullptr;
	}
	DEFINE_USER_COMPONENT(BoxCollider)
	DEFINE_USER_COMPONENT(SphereCollider)
	DEFINE_USER_COMPONENT(MatChangeOnHit)
	DEFINE_USER_COMPONENT(FireGun)
	DEFINE_USER_COMPONENT(EnemyComp)
	DEFINE_USER_COMPONENT(CameraControl)
	DEFINE_USER_COMPONENT(LightComponent)
	DEFINE_USER_COMPONENT(AnimationComponent)
	DEFINE_USER_COMPONENT(PathNodeComponent)
	DEFINE_USER_COMPONENT(AudioComponent)
	DEFINE_USER_COMPONENT(RigidBody)
	DEFINE_USER_COMPONENT(BulletComponent)
	DEFINE_USER_COMPONENT(PlayerComponent)
	DEFINE_USER_COMPONENT(DoorComponent)
	DEFINE_USER_COMPONENT(AnimationControlTest)
	

	return nullptr;
}

string EncodeVector3(Vector3 vec){
	return to_string(vec.x) + "," + to_string(vec.y) + "," + to_string(vec.z);
}

string EncodeVector2(Vector2 vec){
	return to_string(vec.x) + "," + to_string(vec.y);
}

string EncodeQuaternion(Quaternion quat){
	return to_string(quat.w) + "," + to_string(quat.x) + "," + to_string(quat.y)+ "," + to_string(quat.z);
}

string EncodeAnimationType(AnimationType type){
	switch(type){
	case AnimationType::Float:
		return "float";
		break;
	case AnimationType::Vector2:
		return "vector2";
		break;
	case AnimationType::Vector3:
		return "vector3";
		break;
	case AnimationType::Quaternion:
		return "quaternion";
		break;
	default:
		return "";
		break;
	}
}

string EncodeAnimationTarget(AnimationTarget target){
	switch(target){
	case AnimationTarget::Position:
		return "position";
		break;
	case AnimationTarget::Scale:
		return "scale";
		break;
	case AnimationTarget::UVOffset:
		return "uvOffset";
		break;
	case AnimationTarget::UVScale:
		return "uvScale";
		break;
	case AnimationTarget::Rotation:
		return "rotation";
		break;
	default:
		return "";
		break;
	}
}

Vector3 ParseVector3(string encoded){
	vector<string> parts = SplitStringByDelimiter(encoded, ",");
	float x = atof(parts[0].c_str());
	float y = atof(parts[1].c_str());
	float z = atof(parts[2].c_str());

	return Vector3(x,y,z);
}

Vector2 ParseVector2(string encoded){
	vector<string> parts = SplitStringByDelimiter(encoded, ",");
	float x = atof(parts[0].c_str());
	float y = atof(parts[1].c_str());

	return Vector2(x,y);
}

Quaternion ParseQuaternion(string encoded){
	vector<string> parts = SplitStringByDelimiter(encoded, ",");

	float w = atof(parts[0].c_str());
	float x = atof(parts[1].c_str());
	float y = atof(parts[2].c_str());
	float z = atof(parts[3].c_str());

	return Quaternion(w,x,y,z);
}

AnimationType ParseAnimationType(string encoded){
	if(encoded == "float"){
		return AnimationType::Float;
	}
	else if(encoded == "vector2"){
		return AnimationType::Vector2;
	}
	else if(encoded == "vector3"){
		return AnimationType::Vector3;
	}
	else if(encoded == "quaternion"){
		return AnimationType::Quaternion;
	}
	else{
		cout << "\nError: could not understand animation type of: '" << encoded << "'.\n";
		return AnimationType::Float;
	}
}

AnimationTarget ParseAnimationTarget(string encoded){
	if(encoded == "position"){
		return AnimationTarget::Position;
	}
	else if(encoded == "scale"){
		return AnimationTarget::Scale;
	}
	else if(encoded == "uvOffset"){
		return AnimationTarget::UVOffset;
	}
	else if(encoded == "uvScale"){
		return AnimationTarget::UVScale;
	}
	else if(encoded == "rotation"){
		return AnimationTarget::Rotation;
	}
	else{
		cout << "\nError: could not understand animation type of: '" << encoded << "'.\n";
		return AnimationTarget::Position;
	}
	
}

void LoadAudioXML(Scene* scene, XMLElement elem){
	AudioClip* clip = scene->audio.AddClip();
	for(auto iter = elem.attributes.begin(); iter != elem.attributes.end(); iter++){
		if(iter->name == "fileName"){
			clip->clipFileName = iter->data;
			clip->LoadFromWavFile(iter->data.c_str());
		}
		else if(iter->name == "name"){
			clip->clipName = iter->data;
		}
	}
}

void LoadMaterialXML(Scene* scene, XMLElement elem){
	string matName="", shaderName="",textureName="",bumpMapName="";
	Vector2 uvOffset = Vector2(0,0), uvScale = Vector2(1,1);

	for(auto iter = elem.attributes.begin(); iter != elem.attributes.end(); iter++){
		if(iter->name == "name"){
			matName = iter->data;
		}
		else if(iter->name == "shader"){
			shaderName = iter->data;
		}
		else if(iter->name == "texture"){
			textureName = iter->data;
		}
		else if(iter->name == "bumpMap"){
			bumpMapName = iter->data;
		}
		else if(iter->name == "uvScale"){
			uvScale = ParseVector2(iter->data);
		}
		else if(iter->name == "uvOffset"){
			uvOffset = ParseVector2(iter->data);
		}
	}

	Material* mat = scene->resources.LoadMaterial(matName, shaderName, textureName, bumpMapName);
	mat->uvScale = uvScale;
	mat->uvOffset = uvOffset;
}

GameObject* Scene::LoadGameObjectXML(const XMLElement& elem, bool fireAwakeEvents){
	GameObject* go = new GameObject();
	go->scene = this;

	for(auto iter = elem.attributes.begin(); iter != elem.attributes.end(); iter++){
		XMLAttribute attr = *iter;
		if(attr.name == "name"){
			go->name = attr.data;
		}
	}

	for(auto iter = elem.children.begin(); iter != elem.children.end(); iter++){
		XMLElement child = *iter;
		if(child.name == "Transform"){
			for(auto iter2 = child.attributes.begin(); iter2 != child.attributes.end(); iter2++){
				XMLAttribute attr = *iter2;

				if(attr.name == "position"){
					go->transform.position = ParseVector3(attr.data);
				}
				else if(attr.name == "rotation"){
					go->transform.rotation = ParseQuaternion(attr.data);
				}
				else if(attr.name == "scale"){
					go->transform.scale = ParseVector3(attr.data);
				}
				else if(attr.name == "parent"){
					GameObject* parent = FindGameObject(attr.data);

					if(parent != NULL){
						go->transform.SetParent(&(parent->transform));
					}
				}
			}
		}
		else if(child.name == "Material"){
			string shaderName="", textureName="";
			bool instance=true;
			bool newMaterial=true;
			for(auto iter2 = child.attributes.begin(); iter2 != child.attributes.end(); iter2++){
				XMLAttribute attr = *iter2;
				if(attr.name == "name"){
					go->AddNamedMaterial(attr.data);
					newMaterial=false;
				}
				else if(attr.name == "shader"){
					shaderName = attr.data;
				}
				else if(attr.name == "texture"){
					textureName = attr.data;
				}
				else if(attr.name == "instance"){
					if(attr.data == "false"){
						instance=false;
					}
				}
			}

			if(newMaterial){
				go->AddMaterial(shaderName,textureName);
			}
		}
		else if(child.name == "Mesh"){

			auto iter = child.attributeMap.find("source");
			if(iter != child.attributeMap.end()){
				go->mesh = resources.LoadMesh(iter->second);
			}

			iter = child.attributeMap.find("anim");
			if(iter != child.attributeMap.end()){
				go->mesh->armature->ParseStateString(iter->second);
			}
		}
		else if(child.name == "Camera"){
			camera = &(go->transform);
		}
		else{
			Component* userComp = GetUserDefinedComp(iter->name);
			if(userComp != nullptr){
				userComp->Deserialize(*iter);
				if(fireAwakeEvents){
					go->AddComponent(userComp);
				}
				else{
					go->AddComponentDirect(userComp);
				}
			}
		}
	}

	return go;
}

void Scene::LoadSkyBoxXML(const XMLElement& res){
	const string& images = res.attributeMap.find("images")->second;
	vector<string> imagesSplit = SplitStringByDelimiter(images, ",");

	const string& materialName = res.attributeMap.find("material")->second;

	skyBox = new CubeMap();

	skyBox->Load(imagesSplit.data(), materialName, &resources);
}

XMLElement Scene::SaveSkyBoxXML(){
	XMLElement elem;
	elem.name = "Skybox";

	string textureFileNames = skyBox->textureFileNames[0];
	for(int i = 1; i < 6; i++){
		textureFileNames = textureFileNames + "," + skyBox->textureFileNames[i];
	}

	elem.AddAttribute("images", textureFileNames);
	elem.AddAttribute("material", skyBox->material->matName);

	return elem;
}

void Scene::LoadScene(string fileName){

	RemoveAllObjects(); 

	XMLDocument doc;
	LoadXMLDoc(doc,fileName);

	for(auto iter = doc.contents.begin(); iter != doc.contents.end(); iter++){
		XMLElement topElem = *iter;

		if(topElem.name == "Scene"){
			for(auto iter2 = topElem.children.begin(); iter2 != topElem.children.end(); iter2++){
				XMLElement res = *iter2;

				if(res.name == "GameObject"){
					AddObject(LoadGameObjectXML(res));
				}
				else if(res.name == "Prefab"){
					prefabs.push_back(LoadGameObjectXML(res, false));
				}
				else if(res.name == "Material"){
					LoadMaterialXML(this, res);
				}
				else if(res.name == "GuiElement"){
					guiSystem.LoadGuiElement(res, &resources);
				}
				else if(res.name == "GuiText"){
					guiSystem.LoadGuiText(res, &resources);
				}
				else if(res.name == "AudioClip"){
					LoadAudioXML(this, res);
				}
				else if(res.name == "Skybox"){
					LoadSkyBoxXML(res);
				}
			}
		}
	}

	OnPostLoad();
}


void Scene::SaveScene(string fileName){
	XMLDocument doc;

	XMLElement scene;
	scene.name = "Scene";
	scene.attributes.push_back(XMLAttribute("name", "my-engine"));

	GameObject* bgn = *objects.begin();

	for(int i = 0; i < resources.matAlloc; i++){
		Material* mat = &(resources.materials[i]);
		if(mat->matName != ""){
			XMLElement matElem;
			matElem.name = "Material";

			XMLAttribute attr;

			attr.name = "name";
			attr.data = mat->matName;
			matElem.attributes.push_back(attr);

			attr.name = "shader";
			attr.data = mat->shaderName;
			matElem.attributes.push_back(attr);

			if(mat->mainTexture != NULL){
				attr.name = "texture";
				attr.data = mat->mainTexture->fileName;
				matElem.attributes.push_back(attr);
			}

			if(mat->bumpMap != NULL){
				attr.name = "bumpMap";
				attr.data = mat->bumpMap->fileName;
				matElem.attributes.push_back(attr);
			}

			matElem.attributes.emplace_back("uvScale", EncodeVector2(mat->uvScale));
			matElem.attributes.emplace_back("uvOffset", EncodeVector2(mat->uvOffset));

			scene.children.push_back(matElem);
		}
	}
	
	for(auto iter = guiSystem.elements.begin(); iter != guiSystem.elements.end(); iter++){
		scene.children.push_back((*iter)->Serialize());
	}

	for(int i = 0; i < audio.clipCount; i++){
		XMLElement elem;
		elem.name = "AudioClip";
		elem.attributes.emplace_back("name", audio.clips[i].clipName);
		elem.attributes.emplace_back("fileName", audio.clips[i].clipFileName);
		scene.children.push_back(elem);
	}

	if(skyBox != nullptr){
		scene.children.push_back(SaveSkyBoxXML());
	}

	for(auto iter = prefabs.begin(); iter != prefabs.end(); iter++){
		GameObject* obj = *iter;
		XMLElement elem;
		elem.name = "Prefab";
		elem.attributes.push_back(XMLAttribute("name", obj->name));

		XMLElement trans;
		trans.name = "Transform";
		SC_Transform* parent = obj->transform.GetParent();
		if(parent != NULL && parent->gameObject != NULL){
			trans.attributes.push_back(XMLAttribute("parent", parent->gameObject->name));
		}
		trans.attributes.push_back(XMLAttribute("position", EncodeVector3(obj->transform.position)));
		trans.attributes.push_back(XMLAttribute("rotation", EncodeQuaternion(obj->transform.rotation)));
		trans.attributes.push_back(XMLAttribute("scale",    EncodeVector3(obj->transform.scale)));
		elem.children.push_back(trans);

		if(camera == &(obj->transform)){
			XMLElement cam;
			cam.name = "Camera";
			elem.children.push_back(cam);
		}

		Material* mat = obj->material;
		if(mat != NULL){
			XMLElement material;
			material.name = "Material";
			material.attributes.push_back(XMLAttribute("name",mat->matName));
			elem.children.push_back(material);
		}
		Model* model = obj->mesh;
		if(model != NULL){
			XMLElement mesh;
			mesh.name = "Mesh";
			mesh.attributes.push_back(XMLAttribute("source",model->name));
			
			if(model->armature != nullptr && model->armature->animCount > 0){
				mesh.AddAttribute("anim", model->armature->EncodeStateString());
			}

			elem.children.push_back(mesh);
		}

		for(auto iter = obj->components.begin(); iter != obj->components.end(); iter++){
			XMLElement newElem = (*iter)->Serialize();
			if(newElem.name != ""){
				elem.children.push_back(newElem);
			}
		}

		scene.children.push_back(elem);
	}

	for(auto iter = objects.begin(); iter != objects.end(); iter++){
		GameObject* obj = *iter;
		XMLElement elem;
		elem.name = "GameObject";
		elem.attributes.push_back(XMLAttribute("name", obj->name));

		XMLElement trans;
		trans.name = "Transform";
		SC_Transform* parent = obj->transform.GetParent();
		if(parent != NULL && parent->gameObject != NULL){
			trans.attributes.push_back(XMLAttribute("parent", parent->gameObject->name));
		}
		trans.attributes.push_back(XMLAttribute("position", EncodeVector3(obj->transform.position)));
		trans.attributes.push_back(XMLAttribute("rotation", EncodeQuaternion(obj->transform.rotation)));
		trans.attributes.push_back(XMLAttribute("scale",    EncodeVector3(obj->transform.scale)));
		elem.children.push_back(trans);

		if(camera == &(obj->transform)){
			XMLElement cam;
			cam.name = "Camera";
			elem.children.push_back(cam);
		}

		Material* mat = obj->material;
		if(mat != NULL){
			XMLElement material;
			material.name = "Material";
			material.attributes.push_back(XMLAttribute("name",mat->matName));
			elem.children.push_back(material);
		}
		Model* model = obj->mesh;
		if(model != NULL){
			XMLElement mesh;
			mesh.name = "Mesh";
			mesh.attributes.push_back(XMLAttribute("source",model->name));
			elem.children.push_back(mesh);
		}

		for(auto iter = obj->components.begin(); iter != obj->components.end(); iter++){
			XMLElement newElem = (*iter)->Serialize();
			if(newElem.name != ""){
				elem.children.push_back(newElem);
			}
		}

		scene.children.push_back(elem);
	}

	doc.contents.push_back(scene);

	cout << "Saving to: " << fileName << endl;
	SaveXMLDoc(doc, fileName);
}

void ProcessAssetFile(const string& assetFileName){
	XMLDocument assetDoc;
	LoadXMLDoc(assetDoc, assetFileName);

	if(assetDoc.contents.size() != 1){
		cout << "\n\nError: asset file '" << assetFileName << "' was not properly formatted.\n";
		return;
	}

	XMLElement rootElem = assetDoc.contents[0];
	if(rootElem.name != "Assets"){
		cout << "\n\nError: asset file '" << assetFileName << "' was not properly formatted.\n";
		return;
	}

	for(const XMLElement& childElem : rootElem.children){
		if(childElem.name == "FUV"){
			
			FUV fuv;

			int size =  atoi(childElem.attributeMap.find("size")->second.c_str());
			string trueTypeFileName = childElem.attributeMap.find("fontFile")->second;

			cout << "Rendering truetype file '" << trueTypeFileName << "' at size " << size << ".\n";

			ConvertTrueTypeToFUV(trueTypeFileName, size, fuv);

			string fuvFileName = childElem.attributeMap.find("out")->second;

			ExportFUV(fuv, fuvFileName);
		}
		else if(childElem.name == "Model"){
			Model mod;
			string  inModelName = childElem.attributeMap.find("in")->second;
			string outModelName = childElem.attributeMap.find("out")->second;

			cout << "Importing model '" << inModelName << "'.\n";

			mod.ImportFromFile(inModelName);
			mod.ExportToModelFile(outModelName);
		}
		else{
			cout << "\nError: unrecognised asset type '" << childElem.name << "'.\n";
		}
	}
}