#version 400 core

layout (location = 0) in vec3 pos;
layout(location = 1) in vec2 aTexCoord;

out vec2 TexCoord;
out vec3 FragPos;

uniform mat4 projection;

uniform vec3 offsetPosition; //Model local position offset
uniform mat4 model; //Model world position

void main()
{
	gl_Position = projection * model * vec4((pos + offsetPosition), 1);
	TexCoord = aTexCoord;
}