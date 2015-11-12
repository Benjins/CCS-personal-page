#version 130

attribute vec3 Position;

uniform mat4 _perspMatrix;
uniform mat4 _cameraMatrix;

void main()
{
    gl_Position = _perspMatrix * _cameraMatrix * vec4(Position, 1.0);
}
