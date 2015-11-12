#version 130

in vec3 Position;
in vec2 UV;
in vec4 col;

out vec2 uv;
out vec4 _col;

uniform mat4 _perspMatrix;
uniform mat4 _cameraMatrix;
uniform mat4 _objectMatrix;

void main(){
	uv = UV;
	_col = col;
	gl_Position =  _perspMatrix * _cameraMatrix * _objectMatrix * vec4(Position, 1.0);
}