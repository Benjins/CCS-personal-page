#include "../header/int/ResourceManager.h"
#include "../header/int/Texture.h"
#include "../header/int/Material.h"
#include "../header/int/Model.h"
#include "../header/int/FontBMPMaker.h"


MaterialManager::MaterialManager(int matCount){
	matAlloc = matCount;
	texAlloc = 20;
	meshAlloc = 20;
	fontUVAlloc = 10;
	
	materials = new Material[matAlloc];
	textures = new Texture[texAlloc];
	models = new Model[meshAlloc];
	fontUVs = new FUV[fontUVAlloc];
}

Material* MaterialManager::GetMaterialByName(string name){
	for(int i = 0; i < matAlloc; i++){
		if(materials[i].matName == name){
			return &(materials[i]);
		}
	}

	return NULL;
}
int GetMaterialIdByName(string name);
Material* GetMaterialById(int id);

Material* MaterialManager::LoadMaterial(string matName, string shaderName, string textureName, string bumpMapName){
	for(int i = 0; i < matAlloc; i++){
		if(materials[i].matName == ""){
			materials[i].Switch(shaderName, this, textureName, bumpMapName);
			materials[i].uvOffset = Vector2(0,0);
			materials[i].uvScale = Vector2(1,1);
			materials[i].matName = matName;
			if(materials[i].mainTexture != NULL){
				materials[i].mainTexture->fileName = textureName;
			}
			if(materials[i].bumpMap != NULL){
				materials[i].bumpMap->fileName = bumpMapName;
			}
			return &(materials[i]);
		}
	}

	return NULL;
}

Texture* MaterialManager::LoadTexture(const string& fileName, bool forceInstance /*= false*/){
	for(int i = 0; i < texAlloc; i++){
		if(!forceInstance && textures[i].fileName == fileName){
			return &textures[i];
		}
		else if(textures[i].fileName == ""){
			textures[i].fileName = fileName;
			textures[i].textureTarget = GL_TEXTURE_2D;
			textures[i].Load(GL_TEXTURE0);
			return &textures[i];
		}
	}

	return nullptr;
}

Texture* MaterialManager::GetTextureByFileName(const string& fileName){
	for(int i = 0; i < texAlloc; i++){
		if(textures[i].fileName == fileName){
			return &textures[i];
		}
	}

	return nullptr;
}

Material* MaterialManager::GetFreeMaterial(){
	for(int i = 0; i < matAlloc; i++){
		if(materials[i].matName == ""){
			return &(materials[i]);
		}
	}

	cout << "\n\nWarning: could not find free material, returning null.\n";
	return nullptr;
}

Model* MaterialManager::LoadMesh(const string& fileName, bool forceInstance){
	if(!forceInstance){
		for(int i = 0; i < meshAlloc; i++){
			if(models[i].fileName == fileName){
				return &models[i];
			}
		}
	}

	for(int i = 0; i < meshAlloc; i++){
		if(models[i].fileName == ""){
			models[i].ImportFromFile(fileName);
			return &models[i];
		}
	}

	return nullptr;
}

Model* MaterialManager::GetMeshByName(const string& name){
	for(int i = 0; i < meshAlloc; i++){
		if(models[i].name == name){
			return &models[i];
		}
	}

	return nullptr;
}

FUV* MaterialManager::LoadFUV(const string& fileName, bool forceInstance /*= false*/){
	if(!forceInstance){
		for(int i = 0; i < fontUVAlloc; i++){
			if(fontUVs[i].fileName == fileName){
				return &fontUVs[i];
			}
		}
	}

	for(int i = 0; i < fontUVAlloc; i++){
		if(fontUVs[i].fileName == ""){
			ImportFUV(fileName, fontUVs[i]);
			return &fontUVs[i];
		}
	}

	return nullptr;
}

FUV* MaterialManager::GetFUVByName(const string& name){
	for(int i = 0; i < fontUVAlloc; i++){
		if(fontUVs[i].fileName == name){
			return &fontUVs[i];
		}
	}

	return nullptr;
}

void MaterialManager::Clear(){
	if(materials != NULL){
		delete[] materials;
	}

	materials = new Material[matAlloc];
}

MaterialManager::~MaterialManager(){
	if(materials != NULL){
		delete[] materials;
	}

	if(textures != nullptr){
		delete[] textures;
	}

	if(models != nullptr){
		delete[] models;
	}

	if(fontUVs != nullptr){
		delete[] fontUVs;
	}
}
