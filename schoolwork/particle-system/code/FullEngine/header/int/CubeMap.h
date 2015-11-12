#ifndef CUBEMAP_H
#define CUBEMAP_H

#pragma once

#include <string>
#include "Texture.h"

using std::string;

struct Mat4x4; struct MaterialManager; struct Material;

struct CubeMap{
	Material* material;

	GLuint cubeMapObj;

	GLuint cubeMeshObj;

	int glVertexCount;

	string textureFileNames[6];

	static GLenum types[];

	CubeMap();

	void Load(string fileNames[], const string& matName, MaterialManager* resources);

	void Bind(GLenum textureUnit);

	void Render(const Mat4x4& cameraMat, const Mat4x4& perspMat);

};


#endif