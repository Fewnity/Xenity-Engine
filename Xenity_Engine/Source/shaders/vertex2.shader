#version 330 core

layout (location = 0) in vec3 pos;
//layout (location = 1) in vec4 color;
//layout(location = 2) in vec2 aTexCoord;
layout(location = 1) in vec2 aTexCoord;
uniform vec3 offsetPosition;
out vec4 theColor;
out vec2 TexCoord;
void main()
{
	gl_Position = vec4(pos + offsetPosition, 0.7);
	//theColor = color;
	TexCoord = aTexCoord;
}