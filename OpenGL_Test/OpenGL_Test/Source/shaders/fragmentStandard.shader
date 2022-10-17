#version 330 core
out vec4 FragColor;
uniform vec4 ourColor;
uniform vec3 ambiantLightColor;
uniform vec3 lightPos;
uniform vec3 cameraPos;
in vec4 theColor;

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

#define NR_POINT_LIGHTS 1  
uniform PointLight pointLights[NR_POINT_LIGHTS];
#define NR_SPOT_LIGHTS 1  
uniform SpotLight spotLights[NR_SPOT_LIGHTS];
#define NR_DIRECTIONAL_LIGHTS 1  
uniform DirectionalLight directionalLights[NR_DIRECTIONAL_LIGHTS];

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
	vec3 result = (diffuse) + (specular); //Set face result
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
	vec3 result = (diffuse * attenuation) + (specular * attenuation); //Set face result
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

	float theta = dot(lightDir, normalize(-light2.direction));
	float epsilon = light2.cutOff - light2.outerCutOff;
	float intensity = clamp((theta - light2.outerCutOff) / epsilon, 0.0, 1.0);
	diffuse *= intensity;
	specular *= intensity;

	//Result
	vec3 result = (diffuse * attenuation) + (specular * attenuation); //Set face result
	return result;
}

void main()
{
	// Ambient
	float ambientStrength = 1;
	vec3 ambient = ambientStrength * material.ambient * vec3(texture(material.diffuse, TexCoord)); //Get ambient intensity and color
	// Diffuse
	vec3 norm = normalize(Normal); //Direction of normals
	vec3 viewDir = normalize(cameraPos - FragPos);

	//Result
	vec3 result = ambient; //Set face result
	result += CalculatePointLight(pointLights[0], norm, FragPos, viewDir);
	result += CalculateSpotLight(spotLights[0], norm, FragPos, viewDir);
	result += CalculateDirectionalLight(directionalLights[0], norm, FragPos, viewDir);

	float alpha = texture(material.diffuse, TexCoord).a;
	//if (alpha <= 0.1)
		//discard;
	FragColor = vec4(result, alpha); //Add texture color
	//FragColor = texture(material.diffuse, TexCoord);
	//FragColor = vec4(result, 1.0); //Add texture color
}