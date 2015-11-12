#version 130

out vec4 FragColor;

uniform vec4 _color;

void main()
{
    FragColor = _color;
}