#version 330 core

out vec4 FragColor;

in vec2 TexCoord;

struct Material {
	sampler2D diffuse;
};

uniform Material material;
uniform vec4 color;
//uniform vec3 color;

void main()
{
	vec3 ambient = vec3(texture(material.diffuse, TexCoord)) * color.xyz; //Get ambient intensity and color
	float alpha = texture(material.diffuse, TexCoord).a * color.w;

	FragColor = vec4(ambient, alpha); //Add texture color
}