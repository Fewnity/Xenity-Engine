#version 400 core

layout (location = 0) in vec3 pos;
//layout (location = 1) in vec4 color;
//layout(location = 2) in vec2 aTexCoord;
layout(location = 1) in vec2 aTexCoord;
layout(location = 2) in vec3 aNormal;

uniform vec3 offsetPosition;
out vec4 theColor;
out vec2 TexCoord;

out vec3 Normal;
out vec3 FragPos;

uniform mat4 camera;
uniform mat4 projection;
uniform mat4 rotation;
uniform mat4 offset;
//uniform mat4 scale;

void main()
{
	//gl_Position = vec4(pos + offsetPosition, 1);
	//gl_Position = projection * camera * vec4(pos, 1);
	mat3 scale = mat3(
		vec3(1.0, 0.0, 0.0),
		vec3(0.0, 1.0, 0.0),
		vec3(0.0, 0.0, 1.0)
	);

	//gl_Position = projection * camera * offset * rotation * vec4(pos * scale, 1);
	gl_Position = projection * camera * offset * rotation * vec4((pos + offsetPosition) * scale, 1);
	//gl_Position = projection * offset * rotation * vec4((pos + offsetPosition) * scale, 1);
	//gl_Position = offset * vec4(pos * scale, 1);

	//theColor = color;
	TexCoord = aTexCoord;
	FragPos = vec3(offset * rotation * vec4((pos + offsetPosition) * scale, 1));
	Normal = mat3(transpose(inverse(offset * rotation))) * aNormal; //TODO Check an object with a bigger scale and with a offsetPosition, fix : add to offset * rotation this : * offsetPosition * scale
	//Normal = aNormal;
}