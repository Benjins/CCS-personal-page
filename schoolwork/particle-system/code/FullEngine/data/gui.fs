#version 130

out vec4 FragColor;

in vec2 uv_coord;
uniform sampler2D _guiTex;

void main()
{
    FragColor = texture2D(_guiTex, uv_coord.st);
}
