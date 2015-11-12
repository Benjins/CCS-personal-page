#ifndef MATERIAL_H
#define MATERIAL_H

#pragma once

#define MAX_NUMBER_OF_UNIFORMS 12

#include <string>
#include <map>
#include "Model.h"
#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/glew.h>
#endif

using std::string; using std::map;

enum struct MaterialFlag{
	None = 0,
	Normals =           (1 << 0),
	Tangents =          (1 << 1),
	UVs =               (1 << 2),
	Lighting =          (1 << 3),
	Objectmatrix =      (1 << 4),
	CameraMatrix =      (1 << 5),
	PerspectiveMatrix = (1 << 6),
	Skybox =            (1 << 7),
	TintColor =         (1 << 8),
	Armature =          (1 << 9),
	All = 0x7FFFFFFF
};

struct Texture;
struct Vector4;
struct Mat4x4;
struct MaterialManager;
struct ResourceManager;

struct Material{
	string shaderName;
	string vshaderText;
	string fshaderText;
	string matName;

	bool sharedMaterial;
	bool ownBumpMap;
	
	GLuint shaderProgram;
	Texture* mainTexture;
	Texture* bumpMap;

	GLuint uniforms[MAX_NUMBER_OF_UNIFORMS];
	vector<string> uniformNames;
	map<string, GLint> uniformLocCache;

	GLuint textureUniform;
	GLuint cameraUniform;
	GLuint objectMatrixUniform;

	Vector2 uvOffset;
	Vector2 uvScale;
	
	Material(void);

	Material(string _shaderName, MaterialManager* manager, string textureName = "", string bumpMapName = "");

	Material* Clone(MaterialManager* resources) const;
	
	void AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType);

	void Switch(string _shaderName, MaterialManager* manager, string textureName = "", string bumpMapName = "");
	
	void SetFloatUniform(string name, float value);
	void SetVec2Uniform(string name, Vector2 value);
	void SetVec3Uniform(string name, Vector3 value);
	void SetVec4Uniform(string name, Vector4 value);
	void SetMat4Uniform(string name, Mat4x4 value);
	void SetTextureUniform(string name, Texture* value);

	void SetMainTexture(Texture* value);

	GLint GetUniformByName(string name);

	void Release(ResourceManager* manager);

	~Material();
};

bool ReadFile(string fileName, string& readInto);

string TrimWhitespace(string param);

vector<string> GetUniformNames(string shaderCode);

#endif