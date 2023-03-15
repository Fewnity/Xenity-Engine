#version 330 core

layout (location = 0) in vec2 vertex;
layout(location = 1) in vec2 aTexCoord;

out vec2 TexCoord;
out vec3 FragPos;

uniform mat4 projection;

uniform mat4 model; //Model world position
uniform mat4 camera; //Model world position

void main()
{
	//gl_Position = projection * camera * model * vec4((pos + offsetPosition), 1);
	gl_Position = projection * camera * model * vec4(vertex, 1.0, 1.0);
	//gl_Position = projection * model * vec4(vertex, 1.0, 1.0);
	TexCoord = aTexCoord;
}