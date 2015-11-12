#version 130

in vec3 Position;
in vec2 UV;
in vec3 _normal;

out vec2 uv_coord;
out vec3 normal;
out vec3 pos;

uniform mat3 _uvMatrix;
uniform mat4 _perspMatrix;
uniform mat4 _cameraMatrix;
uniform mat4 _objectMatrix;

void main()
{
	pos = (_objectMatrix * vec4(Position, 1.0)).xyz;
    gl_Position =  _perspMatrix * _cameraMatrix * vec4(pos, 1.0);
	uv_coord = (_uvMatrix * vec3(UV, 1.0)).xy;
	normal  = (_objectMatrix * vec4(_normal, 0.0)).xyz;
}
