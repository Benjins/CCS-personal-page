#version 130
#define MAX_NUM_LIGHTS 6

out vec4 FragColor;

in vec2 uv_coord;
in vec3 normal;
in vec3 pos;

uniform float reflectiveLevel = 0.3;

uniform sampler2D _mainTex;
uniform sampler2D _bumpMap;
uniform samplerCube _cubeMap;
uniform vec3 _cameraPos;

void main()
{
	vec4 texCol  = texture2D(_mainTex, uv_coord.st);
	
	vec3 lookVector = (pos - _cameraPos);
	vec3 normalProj = dot(lookVector, normal) * normal;
	vec3 sampleVec = (lookVector - (2 * (lookVector - normalProj)));
	
	vec4 reflectiveSample = texture(_cubeMap, sampleVec);
	
	FragColor = mix(texCol, reflectiveSample, reflectiveLevel);
}
