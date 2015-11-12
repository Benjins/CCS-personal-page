#version 130

in vec3 Position;
out vec2 uv_coord;
uniform vec2 uvs[4] = vec2[](vec2(0,0), vec2(1,0), vec2(0,1), vec2(1,1));

void main()
{
    gl_Position = vec4(Position, 1.0);
	uv_coord = uvs[gl_VertexID];
}
