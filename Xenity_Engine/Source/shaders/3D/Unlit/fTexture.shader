#version 330 core

out vec4 FragColor;

in vec2 TexCoord;

struct Material {
	sampler2D diffuse;
};

uniform Material material;

void main()
{
	float alpha = texture(material.diffuse, TexCoord).a;
	FragColor = vec4(vec3(texture(material.diffuse, TexCoord)), alpha); //Add texture color
}