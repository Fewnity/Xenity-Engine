//-------------- {pc}
//-------------- {vertex}

#version 330

layout (location = 0) in vec3 position;
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
	gl_Position = projection * camera * model * vec4(position, 1);
	TexCoord = uv;
	FragPos = vec3(model * vec4(position, 1));

	Normal = mat3(transpose(inverse(model))) * normal; //TODO Check an object with a bigger scale and with a 	offsetPosition, fix : add to offset * rotation this : * offsetPosition * scale
}

//-------------- {fragment}

#version 330

uniform vec3 ambiantLightColor;
uniform vec3 cameraPos;

in vec3 Normal;
in vec3 FragPos;

in vec2 TexCoord;
uniform sampler2D ourTexture;
uniform vec2 tiling;
uniform vec2 offset;

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

	//vec3 lightDir = normalize(-light2.direction); // Directional light
	//float diff = max(dot(norm, lightDir), 0.0); //If the light is behind the face, diff is 0
	//vec3 diffuse = (diff * vec3(texture(material.diffuse, TexCoord))) * light2.color * 2; //Set the light color and intensity TODO : Change the ambiantLightColor by the light color

	//Spectacular
	//float specularStrength = 0.5;
	//vec3 reflectDir = reflect(-lightDir, norm);
	//float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	//vec3 specular = specularStrength * (spec * vec3(texture(material.specular, TexCoord))) * light2.color;

	//Result
	//vec3 result = (diffuse) + (specular); //Set face result
	//vec3 result = diffuse; //Set face result


	vec3 result = (vec3(texture(material.diffuse, (TexCoord * tiling) + offset)) * light2.color); //Set face result
	return result;
}

vec3 CalculatePointLight(PointLight light2, vec3 norm, vec3 fragPos, vec3 viewDir) {

	vec3 lightDir = normalize(light2.position - fragPos); //Direction of the point light between the light source and the face
	float diff = max(dot(norm, lightDir), 0.0); //If the light is behind the face, diff is 0
	vec3 diffuse = (diff * vec3(texture(material.diffuse, (TexCoord * tiling) + offset))) * light2.color * 2; //Set the light color and intensity TODO : Change the ambiantLightColor by the light color

	//Spectacular
	float specularStrength = 0.5;
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = specularStrength * (spec * vec3(texture(material.specular, (TexCoord * tiling) + offset))) * light2.color;

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
	vec3 diffuse = (diff * vec3(texture(material.diffuse, (TexCoord * tiling) + offset))) * light2.color * 2; //Set the light color and intensity TODO : Change the ambiantLightColor by the light color

	//Spectacular
	float specularStrength = 0.5;
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = specularStrength * (spec * vec3(texture(material.specular, (TexCoord * tiling) + offset))) * light2.color;

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

	float alpha = texture(material.diffuse, (TexCoord * tiling) + offset).a;
	//if (alpha <= 0.1)
		//discard;
	gl_FragColor = vec4(result, alpha); //Add texture color
	//FragColor = texture(material.diffuse, TexCoord);
	//FragColor = vec4(result, 1.0); //Add texture color
}

//-------------- {psvita}
//-------------- {vertex}

attribute vec3 position;
attribute vec2 uv;
attribute vec3 normal;

varying vec2 TexCoord;
varying vec3 Normals;
varying vec3 FragPos;

uniform mat4 camera;
uniform mat4 projection;

uniform mat4 model; //Model matrice position, rotation and scale

mat3 inverse(mat3 m) {
  float a00 = m[0][0], a01 = m[0][1], a02 = m[0][2];
  float a10 = m[1][0], a11 = m[1][1], a12 = m[1][2];
  float a20 = m[2][0], a21 = m[2][1], a22 = m[2][2];

  float b01 = a22 * a11 - a12 * a21;
  float b11 = -a22 * a10 + a12 * a20;
  float b21 = a21 * a10 - a11 * a20;

  float det = a00 * b01 + a01 * b11 + a02 * b21;

  return mat3(b01, (-a22 * a01 + a02 * a21), (a12 * a01 - a02 * a11),
              b11, (a22 * a00 - a02 * a20), (-a12 * a00 + a02 * a10),
              b21, (-a21 * a00 + a01 * a20), (a11 * a00 - a01 * a10)) / det;
}

void main()
{
	//gl_Position = projection * camera * model * vec4(position, 1);
	//gl_Position = projection * camera * model * vec4(position, 1);
	gl_Position = mul(float4(position, 1.0f), mul(model, mul(camera, projection)));
	FragPos = float3(mul(float4(position, 1.0f), model));	
	Normals = mat3(transpose(inverse(model))) * normal; //TODO Check an object with a bigger scale and with a 	offsetPosition, fix : add to offset * rotation this : * offsetPosition * scale
	//gl_Position = model * camera * projection * vec4(position, 1);
	TexCoord = uv;
}

//-------------- {fragment}

uniform sampler2D textureBase;

struct PointLight {
	vec3 position;

	vec3 color;

	float constant;
	float linear;
	float quadratic;
};


struct DirectionalLight {
	vec3 direction;
	vec3 color;
};

#define NR_POINT_LIGHTS 10  
uniform PointLight pointLights[NR_POINT_LIGHTS];
#define NR_DIRECTIONAL_LIGHTS 10  
uniform DirectionalLight directionalLights[NR_DIRECTIONAL_LIGHTS];

uniform int usedDirectionalLightCount;
uniform int usedPointLightCount;

//out vec4 FragColor;
uniform vec3 color;
uniform vec3 cameraPos;

varying vec2 TexCoord;
varying vec3 Normals;
varying vec3 FragPos;

struct Material {
	sampler2D diffuse;
	vec3 ambient;
};

uniform Material material;
uniform vec2 tiling;
uniform vec2 offset;

float3 CalculateDirectionalLight(DirectionalLight light, vec3 norm, vec3 fragPos, vec3 viewDir, vec2 texcoords) 
{
	//float3 lightDir = normalize(-light.direction); // Directional light
	//float diff = max(dot(norm, lightDir), 0.0f); //If the light is behind the face, diff is 0
	//float3 tx = (diff * tex2D(textureBase, texcoords).xyz) * light.color; //Set the light color and intensity TODO : Change the ambiantLightColor by the light color

	float3 tx = tex2D(textureBase, (texcoords * tiling) + offset)).xyz * light.color;
	return tx;
}

float3 CalculatePointLight(PointLight light, float3 norm, float3 fragPos, float3 viewDir, float2 texcoords) 
{
	//float3 lightDir = normalize(float3(0,0,0) - fragPos);
	float3 lightDir = normalize(light.position - fragPos); //Direction of the point light between the light source and the face
	//float3 lightDir = normalize(fragPos - light.position); //Direction of the point light between the light source and the face
	
	float diff = max(dot(norm, lightDir), 0.0f); //If the light is behind the face, diff is 0

	float3 diffuse = (diff * tex2D(textureBase, (texcoords * tiling) + offset)).xyz) * light.color * 2; //Set the light color and intensity TODO : Change the ambiantLightColor by the light color

	//float3 tx = tex2D(textureBase, texcoords).xyz * light.color;

	float distance = length(light.position - fragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
	return diffuse * attenuation;
}

void main()
{
	// Ambient

	//vec3 ambient = color * vec3(tex2D(material.diffuse, TexCoord)); //Get ambient intensity and color

	//Result
	//vec3 result = ambient; //Set face result

	vec3 result = float3(0.0f, 0.0f, 0.0f);
	vec3 norm = normalize(Normals); //Direction of normals
	vec3 viewDir = float3(0.0f, 0.0f, 0.0f); // = normalize(cameraPos - FragPos);

	for (int i = 0; i < usedPointLightCount; i++)
	{
		result += CalculatePointLight(pointLights[i], norm, FragPos, viewDir, TexCoord);
	}

	for (int i = 0; i < usedDirectionalLightCount; i++)
	{
		result += CalculateDirectionalLight(directionalLights[i], norm, FragPos, viewDir, TexCoord);
	}

	float alpha = tex2D(material.diffuse, TexCoord).a;
	gl_FragColor = vec4(result, alpha); //Add texture color
}