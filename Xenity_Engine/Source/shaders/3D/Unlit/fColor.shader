#version 330 core

out vec4 FragColor;
uniform vec3 color;
uniform vec3 cameraPos;

in vec3 Normal;
in vec3 FragPos;

in vec2 TexCoord;
uniform sampler2D ourTexture;

struct Material {
	sampler2D diffuse;
	sampler2D specular;
	float shininess;
};

uniform Material material;

void main()
{
	// Ambient
	vec3 coloredTex = color; //Get ambient and color
	FragColor = vec4(coloredTex, 1); //Add texture color
}