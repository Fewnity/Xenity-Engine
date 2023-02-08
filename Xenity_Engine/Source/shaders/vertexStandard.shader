#version 400 core

layout (location = 0) in vec3 pos;
layout(location = 1) in vec2 aTexCoord;
layout(location = 2) in vec3 aNormal;



out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;

uniform mat4 camera;
uniform mat4 projection;

uniform mat4 offset; //Model world position
uniform mat4 model; //Model world position
uniform vec3 offsetPosition; //Model local  position offset
uniform mat4 rotation; //Model rotation
uniform mat3 scale; //Model scale

void main()
{
	//gl_Position = projection * camera * offset * rotation * vec4((pos + offsetPosition) * scale, 1);
	gl_Position = projection * camera * model * vec4((pos + offsetPosition), 1);
	TexCoord = aTexCoord;
	FragPos = vec3(model * vec4((pos + offsetPosition), 1));
	//Normal = aNormal; //TODO Check an object with a bigger scale and with a offsetPosition, fix : add to offset * rotation this : * offsetPosition * scale
	Normal = mat3(transpose(inverse(model))) * aNormal; //TODO Check an object with a bigger scale and with a offsetPosition, fix : add to offset * rotation this : * offsetPosition * scale
}