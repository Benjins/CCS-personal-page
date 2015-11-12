#version 130

attribute vec3 Position;

uniform mat4 _perspMatrix;
uniform mat4 _cameraMatrix;

out vec3 texCoord;

void main()
{
	texCoord = Position;
	vec4 cameraPos = _cameraMatrix * vec4(0.0,0.0,0.0,1.0);
	cameraPos.w = 0;
    gl_Position = _perspMatrix * (_cameraMatrix * vec4(100 * Position, 1.0) - cameraPos);
}
