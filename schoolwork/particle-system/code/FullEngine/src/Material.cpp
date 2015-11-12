#include "../header/int/Material.h"
#include "../header/int/Texture.h"
#include "../header/int/Mat4.h"
#include "../header/int/Vector4.h"
#include "../header/int/ResourceManager.h"
#include "../header/ext/EasyBMP.h"
#include <fstream>
#include <iostream>
#include <cstring>

#ifdef TESTING
#if defined(_WIN32) || defined(_WIN64)
#  define _CRTDBG_MAP_ALLOC
#  define _CRTDBG_MAP_ALLOC_NEW
#  include <crtdbg.h>
#  include <assert.h>
#endif
#endif

using std::ifstream; using std::cout; using std::cerr; using std::strlen;

Material::Material(void){
	matName = "";
	mainTexture = NULL;
	bumpMap = NULL;
	sharedMaterial = false;
	ownBumpMap = false;
}


//Requires an OpenGL context
Material::Material(string _shaderName, MaterialManager* manager, string textureName, string bumpMapName){
	mainTexture = NULL;
	bumpMap = NULL;
	Switch(_shaderName, manager, textureName, bumpMapName);
	sharedMaterial = false;
	ownBumpMap = false;
}

Material* Material::Clone(MaterialManager* resources) const{
	Material* newMat = resources->GetFreeMaterial();

	newMat->matName = matName + "(clone)";
	newMat->Switch(shaderName, resources, mainTexture->fileName, bumpMap->fileName);

	newMat->uvOffset = uvOffset;
	newMat->uvScale = uvScale;

	return newMat;
}

void Material::AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType){
    GLuint ShaderObj = glCreateShader(ShaderType);

    const GLchar* p[1];
    p[0] = pShaderText;
    GLint Lengths[1];
    Lengths[0]= strlen(pShaderText);
    glShaderSource(ShaderObj, 1, p, Lengths);
    glCompileShader(ShaderObj);

	GLint success;
	glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);
	if (!success) {
		GLchar InfoLog[1024];
		glGetShaderInfoLog(ShaderObj, sizeof(InfoLog), NULL, InfoLog);
		fprintf(stderr, "Error compiling shader type %d: '%s'\n", ShaderType, InfoLog);
	}
	
    glAttachShader(ShaderProgram, ShaderObj);
}

void Material::Switch(string _shaderName, MaterialManager* manager, string textureName, string bumpMapName){
	shaderProgram = glCreateProgram();
	shaderName = _shaderName;
	matName = _shaderName + textureName;
	
	vshaderText = fshaderText = ""; 
	
	if(ReadFile(_shaderName + ".vs", vshaderText) 
	&& ReadFile(_shaderName + ".fs", fshaderText)){
		AddShader(shaderProgram, vshaderText.c_str(), GL_VERTEX_SHADER);
		AddShader(shaderProgram, fshaderText.c_str(), GL_FRAGMENT_SHADER);
		
		glLinkProgram(shaderProgram);

		GLint Success;
		glGetProgramiv(shaderProgram, GL_LINK_STATUS, &Success);
		if (Success == 0) {
			GLchar ErrorLog[1024];
			glGetProgramInfoLog(shaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
			fprintf(stderr, "Error linking shader program: '%s'\n", ErrorLog);
		}

		glValidateProgram(shaderProgram);

		uniformNames = GetUniformNames(vshaderText + fshaderText);
		for(int i = 0; i < uniformNames.size(); i++){
			uniforms[i] = glGetUniformLocation(shaderProgram, uniformNames[i].c_str());
		}

		cameraUniform = GetUniformByName("_cameraMatrix");
		objectMatrixUniform = GetUniformByName("_objectMatrix");

		if(textureName != ""){
			mainTexture = manager->LoadTexture(textureName);
		}
		else{
			mainTexture = NULL;
		}

		if(ownBumpMap && bumpMap != nullptr){
			delete bumpMap;
		}

		if(bumpMapName != ""){
			bumpMap = manager->LoadTexture(bumpMapName);
			ownBumpMap = false;
		}
		else{
			ownBumpMap = true;
			bumpMap = new Texture(1,1);
			bumpMap->textureTarget = GL_TEXTURE_2D;
			RGBApixel pix = {};
			pix.Blue = 255;
			pix.Alpha = 255;
			bumpMap->SetPixel(0,0,pix);
			bumpMap->Apply(GL_TEXTURE1);
		}
	}
	else{
		//Probably should do something here...
	}
}

void Material::SetFloatUniform(string name, float value){
	auto iter = uniformLocCache.find(name);
	if(iter == uniformLocCache.end()){
		GLint loc = glGetUniformLocation(shaderProgram, name.c_str());
		uniformLocCache.insert(std::make_pair(name, loc));
		glUniform1f(loc, value);
	}
	else{
		glUniform1f(iter->second, value);
	}
}

void Material::SetVec2Uniform(string name, Vector2 value){
	auto iter = uniformLocCache.find(name);
	if(iter == uniformLocCache.end()){
		GLint loc = glGetUniformLocation(shaderProgram, name.c_str());
		uniformLocCache.insert(std::make_pair(name, loc));
		glUniform2fv(loc, 1, (GLfloat*)&value);
	}
	else{
		glUniform2fv(iter->second, 1, (GLfloat*)&value);
	}
}
void Material::SetVec3Uniform(string name, Vector3 value){
	auto iter = uniformLocCache.find(name);
	if(iter == uniformLocCache.end()){
		GLint loc = glGetUniformLocation(shaderProgram, name.c_str());
		uniformLocCache.insert(std::make_pair(name, loc));
		glUniform3fv(loc, 1, (GLfloat*)&value);
	}
	else{
		glUniform3fv(iter->second, 1, (GLfloat*)&value);
	}
}

void Material::SetVec4Uniform(string name, Vector4 value){
	auto iter = uniformLocCache.find(name);
	if(iter == uniformLocCache.end()){
		GLint loc = glGetUniformLocation(shaderProgram, name.c_str());
		uniformLocCache.insert(std::make_pair(name, loc));
		glUniform4fv(loc, 1, (GLfloat*)&value);
	}
	else{
		glUniform4fv(iter->second, 1, (GLfloat*)&value);
	}
}

void Material::SetMat4Uniform(string name, Mat4x4 value){
	auto iter = uniformLocCache.find(name);
	if(iter == uniformLocCache.end()){
		GLint loc = glGetUniformLocation(shaderProgram, name.c_str());
		uniformLocCache.insert(std::make_pair(name, loc));
		glUniformMatrix4fv(loc, 1, GL_TRUE, (GLfloat*)&value.m);
	}
	else{
		glUniformMatrix4fv(iter->second, 1,  GL_TRUE, (GLfloat*)&value.m);
	}
}

void Material::SetTextureUniform(string name, Texture* value){
	cerr << "At the moment, SetTextureUniform() is unimplemented." << endl;
}

void Material::SetMainTexture(Texture* value){
	if(mainTexture == value){
		return;
	}

	if(mainTexture != NULL){
		delete mainTexture;
	}

	mainTexture = value;
}

bool ReadFile(string fileName, string& readInto){
	ifstream fileIn(fileName.c_str());
	
	if (fileIn.is_open()) {
        string line;
        while(!fileIn.eof()) {
			getline(fileIn, line);
            readInto.append(line);
            readInto.append("\n");
        }
        
        fileIn.close();
        
        return true;
    }
    else {
        cerr << fileName << ": unable to open file.\n";
		return false;
    }
}

string TrimWhitespace(string param){
	string newStr = param;

	int leadingCount = 0;

	for(auto iter = newStr.begin(); iter != newStr.end(); iter++){
		char single = *iter;
		if(single == ' ' || single == '\n' || single == '\t'){
			leadingCount++;
		}
		else{
			break;
		}
	}

	int trailingCount = 0;

	for(auto iter = newStr.rbegin(); iter != newStr.rend(); iter++){
		char single = *iter;
		if(single == ' ' || single == '\n' || single == '\t'){
			trailingCount++;
		}
		else{
			break;
		}
	}

	return newStr.substr(leadingCount, newStr.size() - leadingCount - trailingCount);
}

vector<string> GetUniformNames(string shaderCode){
	vector<string> lines = SplitStringByDelimiter(shaderCode, "\n");

	vector<string> uniformNames = vector<string>();

	for(auto iter = lines.begin(); iter != lines.end(); iter++){
		string trimmedLine = TrimWhitespace(*iter);
		if(trimmedLine.substr(0,7) == "uniform"){
			int lastSpace = trimmedLine.find_last_of(' ') + 1;
			int uniformLength = trimmedLine.size() - lastSpace - 1;
			uniformNames.push_back(trimmedLine.substr(lastSpace, uniformLength));
		}
	}

	return uniformNames;
}

GLint Material::GetUniformByName(string name){
	auto iter = uniformLocCache.find(name);
	if(iter == uniformLocCache.end()){
		GLint loc = glGetUniformLocation(shaderProgram, name.c_str());
		uniformLocCache.insert(std::make_pair(name, loc));
		return loc;
	}
	else{
		return iter->second;
	}
}

Material::~Material(){
	if(mainTexture != NULL){
		//delete mainTexture;
	}
	if(bumpMap != NULL && ownBumpMap){
		delete bumpMap;
	}
}
