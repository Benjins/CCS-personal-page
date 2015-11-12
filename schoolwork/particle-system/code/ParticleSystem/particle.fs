#version 130

out vec4 FragColor;

in vec2 uv;
in vec4 _col;

uniform sampler2D _particleTex;

void main(){
	FragColor = texture2D(_particleTex, uv.st) * _col;
}
