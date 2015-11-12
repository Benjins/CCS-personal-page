#ifndef MODEL_H
#define MODEL_H

#pragma once

#include <vector>
#include <string>
#include "../ext/Vector2.h"
#include "../ext/Vector3.h"

using std::vector; using std::string;

struct Armature; struct GameObject; struct Quaternion; struct Mat4x4; struct XMLElement;

#define MAX_BONES_PER_VERT 4

struct Face{
	int v0;
	int v1;
	int v2;
	
	Vector2 uv0;
	Vector2 uv1;
	Vector2 uv2;
	
	Face();
	Face(int _v0, int _v1, int _v2);
};

struct Vertex{
	Vector3 position;
	Vector3 color;
	Vector3 normal;
	Vector3 tangent;
	//Vector3 uv;

	float boneWeights[MAX_BONES_PER_VERT];
	int boneIndices[MAX_BONES_PER_VERT];
	short numBones;

	void AddBone(int index, float weight){
		if(numBones < MAX_BONES_PER_VERT){
			boneWeights[numBones] = weight;
			boneIndices[numBones] = index;
			numBones++;
		}
		else{
			cout << "\n\nError: Too many bones added to vertex, max is " << MAX_BONES_PER_VERT << ". The rest will be discarded.\n";
		}
	}
	
	Vertex();
	Vertex(Vector3 _position);
};

struct Model{
	vector<Vertex> vertices;
	vector<Face> faces;

	string name;
	string fileName;

	Armature* armature;

	GameObject* gameObject;
	
	Model();
	Model(const Model& model);
	Model(const string& fileName);

	~Model();
	
	int GLVertexCount() const;
	
	void ImportFromFile(const string& fileName);
	void ImportFromOBJ(const string& fileName);
	void ImportFromCollada(const string& fileName);
	void ImportFromModelFile(const string& fileName);
	void ExportToModelFile(const string& fileName);

	void ImportAnimationLibrary(const XMLElement& elem);

	void CalculateNormals();
	void CalculateTangents();
};


/*
	Model File (*.mdf) spec:
	  - 4 bytes: "MDF\0"
	  - series of chunks
	     Each chunk has:
		   * 4 bytes: chunk type
		   * chunk data (same length as above)
		   * (chunk data is always padded to 4-byte aligned)
		 Chunk types (not all may be present, should be in this order):
		   * 'VPOS' : [4 bytes: numVerts] [numVerts * 12 bytes : vertPositions]
		   * 'FIDX' : [4 bytes: numFaces] [numFaces * 12 bytes : faceVertIndices]
		   * 'FUVS' : [4 bytes: numFaces] [numFaces * 24 bytes : faceUVData]
		   * 'VBID' : [4 bytes: numVerts] [numVerts * [4 bytes : numBonesForVert] [numBonesForVert * 4 bytes : boneIndexData]] 
		   * 'VBWT' : [4 bytes: numVerts] [numVerts * [4 bytes : numBonesForVert] [numBonesForVert * 4 bytes : boneWeightData]] 
		   * 'BNMS' : [4 bytes: numBones] [numBones * [4 bytes: nameLength] [nameLength padded to 4 bytes: nameData]]
		   * 'BPOS' : [4 bytes: numBones] [numBones * 12 bytes : bonePositions]
		   * 'BROT' : [4 bytes: numBones] [numBones * 16 bytes : boneRotations (quaternion w,x,y,z form)]
		   * 'BAPS' : [4 bytes: numBones] [numBones * [4 bytes: numKeyframes] [numKeyFrames * [4 bytes: time] [12 bytes : position]]]
		   * 'BART' : [4 bytes: numBones] [numBones * [4 bytes: numKeyframes] [numKeyFrames * [4 bytes: time] [16 bytes : rotation]]]
		   * 'BBND' : [4 bytes: numBones] [numBones * 64 bytes : bindPoseData (4x4 matrix, row-major form)]
		   * 'BPNT' : [4 bytes: numBones] [numBones * [4 bytes : index of parent bone (or -1 if no parent)]]
*/

Vertex ParseVertexLine(string line);

Vector2 ParseUVLine(string line);

Face ParseFaceLine(string line, const vector<Vector2>& uvs);

vector<string> SplitStringByDelimiter(const string& searchIn, const string& delimiter);

Quaternion MatrixToQuaternion(const Mat4x4& mat);

#endif