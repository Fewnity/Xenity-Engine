#version 330 core

out vec4 FragColor;

in vec2 TexCoord;

struct Material {
	sampler2D diffuse;
	vec3 ambient;
	float shininess;
};

uniform Material material;

void main()
{
	vec3 ambient = vec3(texture(material.diffuse, TexCoord)); //Get ambient intensity and color
	float alpha = texture(material.diffuse, TexCoord).a;

	FragColor = vec4(ambient, alpha); //Add texture color
}