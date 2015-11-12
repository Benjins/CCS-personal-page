#version 130
#define MAX_NUM_LIGHTS 6

out vec4 FragColor;

in vec2 uv_coord;
in vec3 normal;
in lowp vec3 pos;

uniform sampler2D _mainTex;
uniform sampler2D _bumpMap;
uniform vec4 _color;

uniform lowp vec3 lightVectors[MAX_NUM_LIGHTS];
uniform int lightIsDirectional[MAX_NUM_LIGHTS];
uniform float numLights;

void main()
{
	vec4 texCol  = texture2D(_mainTex, uv_coord.st);
	texCol.a = 1.0;
	vec4 color = mix(texCol, _color, 0.5);
	
	lowp float lighting = 0.1;
	
	for(int i = 0; i < int(numLights + 0.5); i++){
		lowp float thisLighting = 0.0;
		if(lightIsDirectional[i] != 0){
			lighting += (dot(normal, lightVectors[i]) + 1)/2;
		}
		else{
			lowp vec3 difference = lightVectors[i] - pos;
			lowp float distance = length(difference);
			lowp float normalDot = dot(normal, difference/distance);
			lowp float clampedDist = min(distance, 5.0);
			thisLighting = max(0, (normalDot+1)/2 * (5 - clampedDist));
			lighting += thisLighting;
		}
	}
	
	
	FragColor = color * lighting;
}
