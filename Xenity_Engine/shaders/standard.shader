{vertex}

#version 330 core

layout (location = 0) in vec3 vertex;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec3 normal;

out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;

uniform mat4 camera;
uniform mat4 projection;

uniform mat4 model; //Model matrice position, rotation and scale

void main()
{
	gl_Position = projection * camera * model * vec4(vertex, 1);
	TexCoord = uv;
	FragPos = vec3(model * vec4(vertex, 1));

	Normal = mat3(transpose(inverse(model))) * normal; //TODO Check an object with a bigger scale and with a 	offsetPosition, fix : add to offset * rotation this : * offsetPosition * scale
}

{fragment}

#version 330 core

out vec4 FragColor;
uniform vec3 ambiantLightColor;
uniform vec3 cameraPos;

in vec3 Normal;
in vec3 FragPos;

in vec2 TexCoord;
uniform sampler2D ourTexture;

struct Material {
	sampler2D diffuse;
	sampler2D specular;
	vec3 ambient;
	float shininess;
};

struct DirectionalLight {
	vec3 direction;
	vec3 color;
};

struct PointLight {
	vec3 position;

	vec3 color;

	float constant;
	float linear;
	float quadratic;
};


struct SpotLight {
	vec3 position;
	vec3 direction;

	vec3 color;

	float constant;
	float linear;
	float quadratic;
	float cutOff;
	float outerCutOff;
};

uniform Material material;

#define NR_POINT_LIGHTS 10  
uniform PointLight pointLights[NR_POINT_LIGHTS];
#define NR_SPOT_LIGHTS 10  
uniform SpotLight spotLights[NR_SPOT_LIGHTS];
#define NR_DIRECTIONAL_LIGHTS 10  
uniform DirectionalLight directionalLights[NR_DIRECTIONAL_LIGHTS];

uniform int usedPointLightCount;
uniform int usedSpotLightCount;
uniform int usedDirectionalLightCount;

vec3 CalculateDirectionalLight(DirectionalLight light2, vec3 norm, vec3 fragPos, vec3 viewDir) {

	vec3 lightDir = normalize(-light2.direction); // Directional light
	float diff = max(dot(norm, lightDir), 0.0); //If the light is behind the face, diff is 0
	vec3 diffuse = (diff * vec3(texture(material.diffuse, TexCoord))) * light2.color * 2; //Set the light color and intensity TODO : Change the ambiantLightColor by the light color

	//Spectacular
	float specularStrength = 0.5;
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = specularStrength * (spec * vec3(texture(material.specular, TexCoord))) * light2.color;

	//Result
	//vec3 result = (diffuse) + (specular); //Set face result
	//vec3 result = (diffuse); //Set face result


	vec3 result = (vec3(texture(material.diffuse, TexCoord)) * light2.color); //Set face result
	return result;
}

vec3 CalculatePointLight(PointLight light2, vec3 norm, vec3 fragPos, vec3 viewDir) {

	vec3 lightDir = normalize(light2.position - fragPos); //Direction of the point light between the light source and the face
	float diff = max(dot(norm, lightDir), 0.0); //If the light is behind the face, diff is 0
	vec3 diffuse = (diff * vec3(texture(material.diffuse, TexCoord))) * light2.color * 2; //Set the light color and intensity TODO : Change the ambiantLightColor by the light color

	//Spectacular
	float specularStrength = 0.5;
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = specularStrength * (spec * vec3(texture(material.specular, TexCoord))) * light2.color;

	float distance = length(light2.position - fragPos);
	float attenuation = 1.0 / (light2.constant + light2.linear * distance + light2.quadratic * (distance * distance));

	//Result
	//vec3 result = (diffuse * attenuation) + (specular * attenuation); //Set face result
	vec3 result = (diffuse * attenuation); //Set face result
	return result;
}

vec3 CalculateSpotLight(SpotLight light2, vec3 norm, vec3 fragPos, vec3 viewDir) {
	vec3 lightDir = normalize(light2.position - fragPos); //Direction of the point light between the light source and the face
	float diff = max(dot(norm, lightDir), 0.0); //If the light is behind the face, diff is 0
	vec3 diffuse = (diff * vec3(texture(material.diffuse, TexCoord))) * light2.color * 2; //Set the light color and intensity TODO : Change the ambiantLightColor by the light color

	//Spectacular
	float specularStrength = 0.5;
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = specularStrength * (spec * vec3(texture(material.specular, TexCoord))) * light2.color;

	float distance = length(light2.position - fragPos);
	float attenuation = 1.0 / (light2.constant + light2.linear * distance + light2.quadratic * (distance * distance));

	//Result
	//vec3 result = (diffuse * attenuation) + (specular * attenuation); //Set face result
vec3 result = (diffuse * attenuation); //Set face result
	return result;
}

void main()
{
	// Ambient

	//vec3 ambient = vec3(texture(material.diffuse, TexCoord)); //Get ambient intensity and color
	vec3 ambient = vec3(0,0,0);
	// Diffuse
	vec3 norm = normalize(Normal); //Direction of normals
	vec3 viewDir = normalize(cameraPos - FragPos);

	//Result
	vec3 result = ambient; //Set face result
	for (int i = 0; i < usedPointLightCount; i++)
	{
		result += CalculatePointLight(pointLights[i], norm, FragPos, viewDir);
	}	
	for (int i = 0; i < usedSpotLightCount; i++)
	{
		result += CalculateSpotLight(spotLights[i], norm, FragPos, viewDir);
	}
	for (int i = 0; i < usedDirectionalLightCount; i++)
	{
		result += CalculateDirectionalLight(directionalLights[i], norm, FragPos, viewDir);
	}

	//result += CalculateSpotLight(spotLights[1], norm, FragPos, viewDir);

	float alpha = texture(material.diffuse, TexCoord).a;
	//if (alpha <= 0.1)
		//discard;
	FragColor = vec4(result, alpha); //Add texture color
	//FragColor = texture(material.diffuse, TexCoord);
	//FragColor = vec4(result, 1.0); //Add texture color
}