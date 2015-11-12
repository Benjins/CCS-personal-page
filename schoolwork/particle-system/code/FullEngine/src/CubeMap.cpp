#include "../header/int/CubeMap.h"
#include "../header/int/Material.h"
#include "../header/int/ResourceManager.h"
#include "../header/int/Mat4.h"
#include "../header/int/Vector4.h"

GLenum CubeMap::types[] = { GL_TEXTURE_CUBE_MAP_NEGATIVE_X, GL_TEXTURE_CUBE_MAP_POSITIVE_X,
							GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
							GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, GL_TEXTURE_CUBE_MAP_POSITIVE_Z};

CubeMap::CubeMap(){
	material = nullptr;	
}

void CubeMap::Load(string fileNames[], const string& matName, MaterialManager* resources){
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	material = resources->GetMaterialByName(matName);
	if(material == nullptr){
		cout << "\n\nError: Could not find material '" << matName << "' for skyBox.\n";
	}

	Model mesh;
	mesh.ImportFromOBJ("data/sphere.obj");

	glVertexCount = mesh.GLVertexCount();

	vector<Vector3> Vertices;
	for(int i = 0; i < mesh.faces.size(); i++){
		Vertices.push_back(mesh.vertices[mesh.faces[i].v0].position);
		Vertices.push_back(mesh.vertices[mesh.faces[i].v1].position);
		Vertices.push_back(mesh.vertices[mesh.faces[i].v2].position);
	}

	glGenBuffers(1, &cubeMeshObj);
	glBindBuffer(GL_ARRAY_BUFFER, cubeMeshObj);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vector3)*Vertices.size(), &(Vertices[0]), GL_STATIC_DRAW);

	glGenTextures(1, &cubeMapObj);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapObj);

	for(int i = 0; i < 6; i++){
		Texture tex;
		tex.fileName = fileNames[i];
		textureFileNames[i] = fileNames[i];
		tex.Load(GL_TEXTURE0, GL_TEXTURE_2D, false);

		//Edge debugging.
		/*
		RGBApixel red = {0,0,255,255};
		for(int x = 0; x < tex.width; x++){
			tex.SetPixel(x, 0, red);
			tex.SetPixel(x, tex.height-1, red);
		}
		
		for(int y = 0; y < tex.height; y++){
			tex.SetPixel(0, y, red);
			tex.SetPixel(tex.width-1, y, red);
		}
		*/

		Bind(GL_TEXTURE2);
		glTexImage2D(types[i], 0, GL_RGBA, tex.width, tex.height, 0, GL_BGRA, GL_UNSIGNED_BYTE, tex.pixelData);
		
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	}
}

void CubeMap::Bind(GLenum textureUnit){
    glActiveTexture(textureUnit);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapObj);
}

void CubeMap::Render(const Mat4x4& cameraMat, const Mat4x4& perspMat){
	Mat4x4 scaleCamMat = cameraMat;
	glUseProgram(material->shaderProgram);

	GLint uniformLoc = material->GetUniformByName("_cameraMatrix");
	glUniformMatrix4fv(uniformLoc, 1, GL_TRUE, &scaleCamMat.m[0][0]);

	GLint perspUniformLoc = material->GetUniformByName("_perspMatrix");
	glUniformMatrix4fv(perspUniformLoc, 1, GL_TRUE, &perspMat.m[0][0]);
	
	Bind(GL_TEXTURE2);

	GLint textureLoc = material->GetUniformByName("_cubeMap");
	glUniform1i(textureLoc, 2);

	GLint oldCullFaceMode;
    glGetIntegerv(GL_CULL_FACE_MODE, &oldCullFaceMode);
    GLint oldDepthFuncMode;
    glGetIntegerv(GL_DEPTH_FUNC, &oldDepthFuncMode);

	glCullFace(GL_FRONT);
	glDepthFunc(GL_LEQUAL);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, cubeMeshObj);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glDrawArrays(GL_TRIANGLES, 0, glVertexCount);

	glDisableVertexAttribArray(0);

    glCullFace(oldCullFaceMode); 
    glDepthFunc(oldDepthFuncMode);
}

