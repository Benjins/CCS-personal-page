#ifndef DRAWCALL_H
#define DRAWCALL_H

#pragma once

#include "Model.h"
#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/glew.h>
#endif

struct Material; struct GameObject;

struct DrawCall{
	//Data for OpenGL
	GLuint vertices;
	GLuint normals;
	GLuint tangents;
	GLuint colors;
	GLuint uvs;
	GLuint boneWeights;
	GLuint boneIndices;
	GLuint boneCount;

	short positionLoc;
	short uvLoc;
	short normalLoc;
	short tangentLoc;
	short boneIndicesLoc;
	short boneWeightsLoc;
	short boneCountLoc;
	
	int vertCount;
	
	GameObject* obj;
	
	DrawCall(GameObject* _obj);
	
	void Draw() const;	
};

void CalculateTangents(Model* model, vector<Vector3>& outTangentData);

#endif