#version 130

#define MAX_BONE_COUNT 24

in vec3 Position;
in vec2 UV;
in vec3 _normal;
in vec3 _tangent;
in vec4 boneIndices;
in vec4 boneWeights;
in float boneCount;

out vec2 uv_coord;
out vec3 normal;
out vec3 pos;
out vec3 tangent;

uniform mat3 _uvMatrix;
uniform mat4 _perspMatrix;
uniform mat4 _cameraMatrix;
uniform mat4 _objectMatrix;

uniform mat4 _armatureMatrices[MAX_BONE_COUNT];

void main()
{
	mat4 boneMatrix = mat4(0.0);
	
	for(int i = 0; i < int(boneCount + 0.5); i++){
		mat4 singleBoneMat = _armatureMatrices[int(boneIndices[i] + 0.5)] * boneWeights[i];
		boneMatrix += singleBoneMat;
	}
	
	mat4 boneObjMatrix = _objectMatrix * boneMatrix;
	
	vec3 _pos = (boneObjMatrix * vec4(Position, 1.0)).xyz;
	pos = _pos;
    gl_Position =  _perspMatrix * _cameraMatrix * vec4(_pos, 1.0);
	uv_coord = (_uvMatrix * vec3(UV, 1.0)).xy;
	normal  = (boneObjMatrix * vec4(_normal, 0.0)).xyz;
	tangent = (boneObjMatrix * vec4(_tangent, 0.0)).xyz;
}
