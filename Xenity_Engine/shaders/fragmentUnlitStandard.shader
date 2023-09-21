#version 330 core

out vec4 FragColor;
uniform vec3 ambiantLightColor;
uniform vec3 cameraPos;

in vec2 TexCoord;

struct Material {
	sampler2D diffuse;
	vec3 ambient;
};

uniform Material material;

void main()
{
	// Ambient
	float ambientStrength = 1;
	vec3 ambient = ambientStrength * material.ambient * vec3(texture(material.diffuse, TexCoord)); //Get ambient intensity and color

	//Result
	vec3 result = ambient; //Set face result

	float alpha = texture(material.diffuse, TexCoord).a;
	FragColor = vec4(result, alpha); //Add texture color
}