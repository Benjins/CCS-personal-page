#include "../header/int/Drawcall.h"
#include "../header/int/GameObject.h"
#include "../header/int/Mat4.h"
#include "../header/int/Material.h"
#include "../header/int/Texture.h"
#include "../header/int/Scene.h"
#include "../header/int/Armature.h"

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#endif

#include <algorithm>

using std::max;

DrawCall::DrawCall(GameObject* _obj){
	obj = _obj;
	int size = static_cast<int>(_obj->mesh->faces.size() * 3);
	vertCount = size;
	vector<Vector3> Vertices;
	vector<Vector3> Normals;
	vector<Vector2> uvCoords;
	vector<Vector3> tangentData;
	vector<float> boneCountsData;
	vector<float> boneIndicesData;
	vector<float> boneWeightsData;

    for(int i = 0; i < _obj->mesh->faces.size(); i++){
		Face face = _obj->mesh->faces[i];
		
		const Vertex& vert0 = _obj->mesh->vertices[face.v0];
		const Vertex& vert1 = _obj->mesh->vertices[face.v1];
		const Vertex& vert2 = _obj->mesh->vertices[face.v2];

		Vertices.push_back(vert2.position);
		Vertices.push_back(vert1.position);
		Vertices.push_back(vert0.position);

		Normals.push_back(vert2.normal);
		Normals.push_back(vert1.normal);
		Normals.push_back(vert0.normal);

		tangentData.push_back(vert2.tangent);
		tangentData.push_back(vert1.tangent);
		tangentData.push_back(vert0.tangent);
		//cout << Vertices[i].Magnitude() << endl;
		
		uvCoords.push_back(face.uv2);
		uvCoords.push_back(face.uv1);
		uvCoords.push_back(face.uv0);

		if(_obj->mesh->armature != nullptr){
			boneCountsData.push_back(min((int)vert2.numBones, 4));
			boneCountsData.push_back(min((int)vert1.numBones, 4));
			boneCountsData.push_back(min((int)vert0.numBones, 4));

			for(int i = 0; i < MAX_BONES_PER_VERT; i++){
				boneIndicesData.push_back(vert2.boneIndices[i]);
				boneWeightsData.push_back(vert2.boneWeights[i]);
			}
			for(int i = 0; i < MAX_BONES_PER_VERT; i++){
				boneIndicesData.push_back(vert1.boneIndices[i]);
				boneWeightsData.push_back(vert1.boneWeights[i]);
			}
			for(int i = 0; i < MAX_BONES_PER_VERT; i++){
				boneIndicesData.push_back(vert0.boneIndices[i]);
				boneWeightsData.push_back(vert0.boneWeights[i]);
			}
		}
	}

	GLuint shaderProg = _obj->material->shaderProgram;

	positionLoc = glGetAttribLocation(shaderProg, "Position");
	uvLoc = glGetAttribLocation(shaderProg, "UV");
	normalLoc = glGetAttribLocation(shaderProg, "_normal");
	tangentLoc = glGetAttribLocation(shaderProg, "_tangent");
	boneIndicesLoc= glGetAttribLocation(shaderProg, "boneIndices");
	boneWeightsLoc = glGetAttribLocation(shaderProg, "boneWeights");
	boneCountLoc= glGetAttribLocation(shaderProg, "boneCount");

 	glGenBuffers(1, &vertices);
	glBindBuffer(GL_ARRAY_BUFFER, vertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vector3)*Vertices.size(), &(Vertices[0]), GL_STATIC_DRAW);
	
	if(uvLoc != -1){
		glGenBuffers(1, &uvs);
		glBindBuffer(GL_ARRAY_BUFFER, uvs);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vector2)*uvCoords.size(), &(uvCoords[0]), GL_STATIC_DRAW);
	}

	if(normalLoc != -1){
		glGenBuffers(1, &normals);
		glBindBuffer(GL_ARRAY_BUFFER, normals);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vector3)*Normals.size(), &(Normals[0]), GL_STATIC_DRAW);
	}

	if(tangentLoc != -1){
		glGenBuffers(1, &tangents);
		glBindBuffer(GL_ARRAY_BUFFER, tangents);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vector3)*tangentData.size(), &(tangentData[0]), GL_STATIC_DRAW);
	}

	if(_obj->mesh->armature != nullptr){
		if(boneCountLoc != -1){
			glGenBuffers(1, &boneCount);
			glBindBuffer(GL_ARRAY_BUFFER, boneCount);
			glBufferData(GL_ARRAY_BUFFER, sizeof(int)*boneCountsData.size(), &(boneCountsData[0]), GL_STATIC_DRAW);
		}

		if(boneIndicesLoc != -1){
			glGenBuffers(1, &boneIndices);
			glBindBuffer(GL_ARRAY_BUFFER, boneIndices);
			glBufferData(GL_ARRAY_BUFFER, sizeof(int)*boneIndicesData.size(), &(boneIndicesData[0]), GL_STATIC_DRAW);
		}

		if(boneWeightsLoc != -1){
			glGenBuffers(1, &boneWeights);
			glBindBuffer(GL_ARRAY_BUFFER, boneWeights);
			glBufferData(GL_ARRAY_BUFFER, sizeof(float)*boneWeightsData.size(), &(boneWeightsData[0]), GL_STATIC_DRAW);
		}
	}
}

void DrawCall::Draw() const{
	Material* material = obj->material;
	glUseProgram(material->shaderProgram);

	GLfloat uvMatrix[9] = { material->uvScale.x, 0, material->uvOffset.x,
							0, material->uvScale.y, material->uvOffset.y,
							0,                      0,                 1};

	glUniformMatrix3fv(material->GetUniformByName("_uvMatrix"), 1, GL_TRUE, uvMatrix);

	material->SetMat4Uniform("_objectMatrix", obj->transform.LocalToGlobalMatrix());

	glUniform1i(material->GetUniformByName("_mainTex"), 0);
	if(material->bumpMap != nullptr){
		glUniform1i(material->GetUniformByName("_bumpMap"), 1);
	}

	vector<Mat4x4> boneMatrices;
	if(obj->mesh->armature != nullptr){
		obj->mesh->armature->GetBoneMatrices(boneMatrices);
	}

	GLenum erroC = glGetError();		
	
	if(positionLoc != -1){
		glEnableVertexAttribArray(positionLoc);
		glBindBuffer(GL_ARRAY_BUFFER, vertices);
		glVertexAttribPointer(positionLoc, 3, GL_FLOAT, GL_FALSE, 0, 0);
	}

	static const int attributeCount = 7;
	GLint attrLocs[attributeCount] = {positionLoc, uvLoc, normalLoc, tangentLoc, boneIndicesLoc, boneWeightsLoc, boneCountLoc};
	GLuint buffers[attributeCount] = {vertices, uvs, normals, tangents, boneIndices, boneWeights, boneCount};
	GLint sizes[attributeCount] = {3,2,3,3,4,4,1};

	for(int i = 0; i < attributeCount; i++){
		if(attrLocs[i] != -1){
			glEnableVertexAttribArray(attrLocs[i]);
			glBindBuffer(GL_ARRAY_BUFFER, buffers[i]);
			glVertexAttribPointer(attrLocs[i], sizes[i], GL_FLOAT, GL_FALSE, 0, 0);
		}
	}

	if(obj->mesh->armature != nullptr){
		GLint armatureUniformLoc = material->GetUniformByName("_armatureMatrices");
		glUniformMatrix4fv(armatureUniformLoc, boneMatrices.size(), GL_TRUE, &(boneMatrices[0].m[0][0]));
	}

	if(material->mainTexture != nullptr){
		material->mainTexture->Bind(GL_TEXTURE0);
	}

	if(material->bumpMap != nullptr){
		material->bumpMap->Bind(GL_TEXTURE1);
	}
	
	glDrawArrays(GL_TRIANGLES, 0, vertCount);
	
	
	for(int i = 0; i < attributeCount; i++){
		if(attrLocs[i] != -1){
			glDisableVertexAttribArray(attrLocs[i]);
		}
	}
}

void CalculateTangents(Model* model, vector<Vector3>& outTangentData){

}

