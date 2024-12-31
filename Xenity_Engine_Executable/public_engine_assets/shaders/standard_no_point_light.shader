//-------------- {pc}
//-------------- {vertex}

#version 330

layout (location = 0) in vec3 position;
layout(location = 1) in vec2 uv;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec3 color;

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
uniform vec4 color;
in vec3 Normal;
in vec3 FragPos;

in vec2 TexCoord;
uniform sampler2D ourTexture;
uniform vec2 tiling;
uniform vec2 offset;
uniform vec3 ambientLight;

struct Material 
{
	sampler2D diffuse;
	sampler2D specular;
	vec3 ambient;
	float shininess;
};

struct DirectionalLight 
{
	vec3 direction;
	vec3 color;
};

struct PointLight 
{
	vec3 position;
	vec3 color;
	vec3 light_data; // x = constant, y = linear, z = quadratic
};

struct SpotLight 
{
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

layout(std140) uniform LightIndices
{
	int usedPointLightCount;
	int usedSpotLightCount;
	int usedDirectionalLightCount;
	int padding0;

	ivec4 pointLightsIndices[NR_POINT_LIGHTS];
	ivec4 spotLightsIndices[NR_SPOT_LIGHTS];
	ivec4 directionalLightsIndices[NR_DIRECTIONAL_LIGHTS];
} lightIndices;

vec3 CalculateDirectionalLight(DirectionalLight light2, vec3 norm, vec3 fragPos, vec3 viewDir) 
{
	vec3 lightDir = normalize(-light2.direction); // Directional light
	float diff = max(dot(norm, lightDir), 0.0); //If the light is behind the face, diff is 0
	vec3 diffuse = (diff * vec3(texture(material.diffuse, (TexCoord * tiling) + offset))) * light2.color * 2; //Set the light color and intensity TODO : Change the ambiantLightColor by the light color

	return diffuse;
}

vec3 CalculatePointLight(PointLight light2, vec3 norm, vec3 fragPos, vec3 viewDir) 
{
	vec3 lightVec = light2.position - fragPos;
	vec3 lightDir = normalize(light2.position - fragPos); //Direction of the point light between the light source and the face
	float diff = max(dot(norm, lightDir), 0.0); //If the light is behind the face, diff is 0
	vec3 diffuse = (diff * vec3(texture(material.diffuse, (TexCoord * tiling) + offset))) * light2.color * 2; //Set the light color and intensity TODO : Change the ambiantLightColor by the light color

	float distanceSq = dot(lightVec, lightVec); // distance square
	float attenuation = 1.0f / (light2.light_data.x + light2.light_data.y * sqrt(distanceSq) + light2.light_data.z * distanceSq);

	vec3 result = (diffuse * attenuation);
	return result;
}

vec3 CalculateSpotLight(SpotLight light2, vec3 norm, vec3 fragPos, vec3 viewDir) 
{
	vec3 lightDir = normalize(light2.position - fragPos); //Direction of the point light between the light source and the face
	float diff = max(dot(norm, lightDir), 0.0); //If the light is behind the face, diff is 0
	vec3 diffuse = (diff * vec3(texture(material.diffuse, (TexCoord * tiling) + offset))) * light2.color * 2; //Set the light color and intensity TODO : Change the ambiantLightColor by the light color

	//Spectacular
	// float specularStrength = 0.5;
	// vec3 reflectDir = reflect(-lightDir, norm);
	// float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	// vec3 specular = specularStrength * (spec * vec3(texture(material.specular, TexCoord))) * light2.color;

	float distance = length(light2.position - fragPos);
	float attenuation = 1.0 / (light2.constant + light2.linear * distance + light2.quadratic * (distance * distance));

	float theta = dot(lightDir, normalize(-light2.direction));
	float epsilon = light2.cutOff - light2.outerCutOff;
	float intensity = clamp((theta - light2.outerCutOff) / epsilon, 0.0, 1.0);
	diffuse *= intensity;
	// specular *= intensity;

	//Result
	//vec3 result = (diffuse * attenuation) + (specular * attenuation); //Set face result
	vec3 result = (diffuse * attenuation); //Set face result
	return result;
}

void main()
{
	vec3 ambient = vec3(0,0,0);
	// Diffuse
	vec3 norm = normalize(Normal); //Direction of normals
	vec3 viewDir = normalize(cameraPos - FragPos);

	//Result
	vec3 result = ambient; //Set face result
	
	for (int i = 0; i < lightIndices.usedSpotLightCount; i++)
	{
		//result += CalculateSpotLight(spotLights[lightIndices.spotLightsIndices[i].x], norm, FragPos, viewDir);
	}
	for (int i = 0; i < lightIndices.usedDirectionalLightCount; i++)
	{
		result += CalculateDirectionalLight(directionalLights[lightIndices.directionalLightsIndices[i].x], norm, FragPos, viewDir);
	}
	result += vec3(texture(material.diffuse, (TexCoord * tiling) + offset)) * ambientLight;

	float alpha = texture(material.diffuse, (TexCoord * tiling) + offset).a* color.w;

	gl_FragColor = vec4(result * color.xyz, alpha); //Add texture color
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

mat3 inverse(mat3 m) 
{
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
	gl_Position = mul(float4(position, 1.0f), mul(model, mul(camera, projection)));
	FragPos = float3(mul(float4(position, 1.0f), model));	
	Normals = mat3(transpose(inverse(model))) * normal; //TODO Check an object with a bigger scale and with a 	offsetPosition, fix : add to offset * rotation this : * offsetPosition * scale

	TexCoord = uv;
}

//-------------- {fragment}

uniform sampler2D textureBase;

struct PointLight 
{
	vec3 position;

	vec3 color;

	vec3 light_data; // x = constant, y = linear, z = quadratic
};

struct SpotLight 
{
	vec3 position;
	vec3 direction;

	vec3 color;

	float constant;
	float linear;
	float quadratic;
	float cutOff;
	float outerCutOff;
};

struct DirectionalLight 
{
	vec3 direction;
	vec3 color;
};

#define NR_POINT_LIGHTS 10  
uniform PointLight pointLights[NR_POINT_LIGHTS];
#define NR_SPOT_LIGHTS 10  
uniform SpotLight spotLights[NR_SPOT_LIGHTS];
#define NR_DIRECTIONAL_LIGHTS 10  
uniform DirectionalLight directionalLights[NR_DIRECTIONAL_LIGHTS];

uniform vec3 cameraPos;
uniform vec4 color;

varying vec2 TexCoord;
varying vec3 Normals;
varying vec3 FragPos;

struct Material 
{
	sampler2D diffuse;
	vec3 ambient;
};

uniform Material material;
uniform vec2 tiling;
uniform vec2 offset;
uniform vec3 ambientLight;

uniform LightIndices
{
	int usedPointLightCount;
	int usedSpotLightCount;
	int usedDirectionalLightCount;
	int padding0;

	ivec4 pointLightsIndices[NR_POINT_LIGHTS];
	ivec4 spotLightsIndices[NR_SPOT_LIGHTS];
	ivec4 directionalLightsIndices[NR_DIRECTIONAL_LIGHTS];
} lightIndices : BUFFER[0];

float3 CalculateDirectionalLight(DirectionalLight light, vec3 norm, vec3 fragPos, vec3 viewDir, vec2 texcoords) 
{
	float3 lightDir = normalize(-light.direction); // Directional light
	float diff = max(dot(norm, lightDir), 0.0f); //If the light is behind the face, diff is 0
	float3 tx = (diff * tex2D(material.diffuse, (texcoords * tiling) + offset).xyz) * light.color; //Set the light color and intensity TODO : Change the ambiantLightColor by the light color
	return tx;
}

vec3 CalculateSpotLight(SpotLight light, vec3 norm, vec3 fragPos, vec3 viewDir, vec2 texcoords) 
{
	vec3 lightDir = normalize(light.position - fragPos); //Direction of the point light between the light source and the face
	float diff = max(dot(norm, lightDir), 0.0); //If the light is behind the face, diff is 0
	vec3 diffuse = (diff * vec3(tex2D(material.diffuse, (texcoords * tiling) + offset))) * light.color * 2; //Set the light color and intensity TODO : Change the ambiantLightColor by the light color

	float distance = length(light.position - fragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

	float theta = dot(lightDir, normalize(-light.direction));
	float epsilon = light.cutOff - light.outerCutOff;
	float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
	diffuse *= intensity;

	vec3 result = (diffuse * attenuation); //Set face result
	return result;
}

float3 CalculatePointLight(PointLight light, float3 norm, float3 fragPos, float3 viewDir, float2 texcoords) 
{
	float3 lightDir = normalize(light.position - fragPos); //Direction of the point light between the light source and the face
	float diff = max(dot(norm, lightDir), 0.0f); //If the light is behind the face, diff is 0
	float3 diffuse = (diff * tex2D(material.diffuse, (texcoords * tiling) + offset).xyz) * light.color * 2; //Set the light color and intensity

	float distance = length(light.position - fragPos);
	float attenuation = 1.0 / (light.light_data.x + light.light_data.y * distance + light.light_data.z * (distance * distance));
	return diffuse * attenuation;
}

void main()
{
	vec3 result = float3(0.0f, 0.0f, 0.0f);
	vec3 norm = normalize(Normals); //Direction of normals
	vec3 viewDir = float3(0.0f, 0.0f, 0.0f); // = normalize(cameraPos - FragPos);

	// Unrool for loop for better performances
	int currentLight = 0;
	if(lightIndices.usedPointLightCount > currentLight)
	{
		result += CalculatePointLight(pointLights[lightIndices.pointLightsIndices[currentLight].x], norm, FragPos, viewDir, TexCoord);
		currentLight += 1;
		if(lightIndices.usedPointLightCount > currentLight)
		{
			result += CalculatePointLight(pointLights[lightIndices.pointLightsIndices[currentLight].x], norm, FragPos, viewDir, TexCoord);
			currentLight += 1;
			if(lightIndices.usedPointLightCount > currentLight)
			{
				result += CalculatePointLight(pointLights[lightIndices.pointLightsIndices[currentLight].x], norm, FragPos, viewDir, TexCoord);
				currentLight += 1;
				if(lightIndices.usedPointLightCount > currentLight)
				{
					result += CalculatePointLight(pointLights[lightIndices.pointLightsIndices[currentLight].x], norm, FragPos, viewDir, TexCoord);
					currentLight += 1;
					if(lightIndices.usedPointLightCount > currentLight)
					{
						result += CalculatePointLight(pointLights[lightIndices.pointLightsIndices[currentLight].x], norm, FragPos, viewDir, TexCoord);
						currentLight += 1;
						if(lightIndices.usedPointLightCount > currentLight)
						{
							result += CalculatePointLight(pointLights[lightIndices.pointLightsIndices[currentLight].x], norm, FragPos, viewDir, TexCoord);
							currentLight += 1;
							if(lightIndices.usedPointLightCount > currentLight)
							{
								result += CalculatePointLight(pointLights[lightIndices.pointLightsIndices[currentLight].x], norm, FragPos, viewDir, TexCoord);
								currentLight += 1;
								if(lightIndices.usedPointLightCount > currentLight)
								{
									result += CalculatePointLight(pointLights[lightIndices.pointLightsIndices[currentLight].x], norm, FragPos, viewDir, TexCoord);
									currentLight += 1;
									if(lightIndices.usedPointLightCount > currentLight)
									{
										result += CalculatePointLight(pointLights[lightIndices.pointLightsIndices[currentLight].x], norm, FragPos, viewDir, TexCoord);
										currentLight += 1;
										if(lightIndices.usedPointLightCount > currentLight)
										{
											result += CalculatePointLight(pointLights[lightIndices.pointLightsIndices[currentLight].x], norm, FragPos, viewDir, TexCoord);
											currentLight += 1;
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}

	// Apply lights
	// for (int i = 0; i < usedPointLightCount; i++)
	// {
	// 	result += CalculatePointLight(pointLights[i], norm, FragPos, viewDir, TexCoord);
	// }
	for (int i = 0; i < lightIndices.usedSpotLightCount; i++)
	{
		result += CalculateSpotLight(spotLights[lightIndices.spotLightsIndices[i].x], norm, FragPos, viewDir, TexCoord);
	}
	for (int i = 0; i < lightIndices.usedDirectionalLightCount; i++)
	{
		result += CalculateDirectionalLight(directionalLights[lightIndices.directionalLightsIndices[i].x], norm, FragPos, viewDir, TexCoord);
	}

	// Apply ambient light
	result += vec3(tex2D(material.diffuse, (TexCoord * tiling) + offset)) * ambientLight;

	float alpha = tex2D(material.diffuse, (TexCoord * tiling) + offset).a * color.w;
	gl_FragColor = vec4(result * color.xyz, alpha); //Add texture color
}

//-------------- {ps3}
//-------------- {vertex}

void main
(
	float3 vertexPosition : POSITION,
	float3 vertexNormal : NORMAL,
	float2 vertexTexcoord : TEXCOORD0,
	
	uniform float4x4 projection,
	uniform float4x4 camera,
	uniform float4x4 model,
	
	out float4 ePosition : POSITION,
	out float4 oPosition : TEXCOORD0,
	out float3 oNormal : TEXCOORD1,
	out float2 oTexcoord : TEXCOORD2,
	out float3 oFragPos : TEXCOORD3
)
{
	ePosition = mul(float4(vertexPosition, 1.0f), mul(model, mul(camera, projection)));
	oPosition = float4(vertexPosition, 1.0f);
	oFragPos = float3(mul(float4(vertexPosition, 1.0f), model));	
	//oNormal = vertexNormal;
	float3x3 normalMatrix = float3x3(model[0].xyz, model[1].xyz, model[2].xyz);
	float3 test =  mul(vertexNormal, transpose(inverse(normalMatrix)));
	oNormal = test;
	// oNormal = mat3(transpose(inverse(model))) * vertexNormal;
	oTexcoord = vertexTexcoord;
}

//-------------- {fragment}

struct DirectionalLight 
{
	vec3 direction;
	vec3 color;
};

struct PointLight 
{
	vec3 position;

	vec3 color;

	vec3 light_data; // x = constant, y = linear, z = quadratic
};

struct SpotLight 
{
	vec3 position;
	vec3 direction;

	vec3 color;

	float constant;
	float linear;
	float quadratic;
	float cutOff;
	float outerCutOff;
};

#define NR_POINT_LIGHTS 10
#define NR_SPOT_LIGHTS 10
#define NR_DIRECTIONAL_LIGHTS 10

float3 CalculateDirectionalLight(DirectionalLight light, float3 norm, float4 textureFrag) 
{
	//float3 lightDir = normalize(-light.direction); // Directional light
	float diff = max(dot(norm, -light.direction), 0.0f); //If the light is behind the face, diff is 0
	float3 tx = (diff * textureFrag.xyz) * light.color; // Set the light color and intensity TODO : Change the ambiantLightColor by the light color
	return tx;
}

//float3 CalculatePointLight(PointLight light, float3 norm, float3 fragPos, float4 textureFrag) 
//{
	//float3 lightDir = normalize(light.position - fragPos); //Direction of the point light between the light source and the face
	//float diff = max(dot(norm, lightDir), 0.0f); //If the light is behind the face, diff is 0
	//float3 diffuse = (diff * textureFrag.xyz) * light.color * 2; //Set the light color and intensity

	//float distance = length(light.position - fragPos);
	//float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

	//return diffuse * attenuation;
//}

float3 CalculatePointLight(PointLight light, float3 norm, float3 fragPos, float4 textureFrag) 
{
	float3 lightVec = light.position - fragPos;
	float3 lightDir = normalize(lightVec); //Direction of the point light between the light source and the face
	float diff = max(dot(norm, lightDir), 0.0f); //If the light is behind the face, diff is 0
	float3 diffuse = (diff * textureFrag.xyz) * light.color; //Set the light color and intensity

	// float distance = length(lightVec);
	float distanceSq = dot(lightVec, lightVec); // distance²
	float attenuation = 1.0f / (light.light_data.x + light.light_data.y * sqrt(distanceSq) + light.light_data.z * distanceSq);
	// float attenuation = 1.0f / (light.constant + light.linear * distance);
	// float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

	return diffuse * attenuation;
}

void main
(
	float4 position : TEXCOORD0,
	float3 normal : TEXCOORD1,
	float2 texcoord : TEXCOORD2,
	float3 FragPos : TEXCOORD3,

	uniform sampler2D texture,

	uniform float3 ambientLight,
	uniform vec4 color,
	uniform vec2 tiling,
	uniform vec2 offset,
	
	uniform PointLight pointLights[NR_POINT_LIGHTS],
	uniform SpotLight spotLights[NR_SPOT_LIGHTS],
	uniform DirectionalLight directionalLights[NR_DIRECTIONAL_LIGHTS],

	uniform float usedPointLightCount,
	uniform float usedSpotLightCount,
	uniform float usedDirectionalLightCount,

	uniform float pointLightsIndices[NR_POINT_LIGHTS],
	uniform float spotLightsIndices[NR_SPOT_LIGHTS],
	uniform float directionalLightsIndices[NR_DIRECTIONAL_LIGHTS],

	out float4 oColor
)
{
	float4 textureFrag = tex2D(texture, (texcoord * tiling) + offset);

	float3 result = float3(0,0,0);
	result += textureFrag.xyz * ambientLight;
	float alpha = textureFrag.w * color.w;
	float3 N = normalize(normal);

	result += CalculateDirectionalLight(directionalLights[1], N, textureFrag);
	//result += CalculatePointLight(pointLights[1], N, FragPos, textureFrag);
	//result += CalculatePointLight(pointLights[2], N, FragPos, textureFrag);

	//result += CalculateDirectionalLight(directionalLights[1], N, textureFrag);
	//result += CalculateDirectionalLight(directionalLights[2], N, textureFrag);


	// int currentLight2 = 0;
	// if(usedPointLightCount > currentLight2)
	// {
	// 	result += CalculatePointLight(pointLights[1], N, FragPos, textureFrag);
	// 	currentLight++;
	// 	if(usedPointLightCount > currentLight2)
	// 	{
	// 		result += CalculatePointLight(pointLights[2], N, FragPos, textureFrag);
	// 	}
	// }

	// result += CalculatePointLight(pointLights[3], N, FragPos, textureFrag);
	// result += CalculatePointLight(pointLights[4], N, FragPos, textureFrag);
	// result += CalculatePointLight(pointLights[5], N, FragPos, textureFrag);
	// result += CalculatePointLight(pointLights[6], N, FragPos, textureFrag);
	// result += CalculatePointLight(pointLights[7], N, FragPos, textureFrag);
	// result += CalculatePointLight(pointLights[8], N, FragPos, textureFrag);
	// result += CalculatePointLight(pointLights[9], N, FragPos, textureFrag);

	// for (int i = 0; i < NR_DIRECTIONAL_LIGHTS; i++) 
	// {
	// 	if (i == directionalLightsIndices[0]) 
	// 	{
	// 		result += CalculateDirectionalLight(directionalLights[i], N, textureFrag);
	// 	}
	// 	else if (i == directionalLightsIndices[1]) 
	// 	{
	// 		result += CalculateDirectionalLight(directionalLights[i], N, textureFrag);
	// 	}
	// }
	// for (int i = 0; i < NR_POINT_LIGHTS; i++) 
	// {
	// 	if (i == pointLightsIndices[0]) 
	// 	{
	// 		result += CalculatePointLight(pointLights[i], N, FragPos, textureFrag);
	// 	}
		// else if (i == pointLightsIndices[1]) 
		// {
		// 	result += CalculatePointLight(pointLights[i], N,FragPos, textureFrag);
		// }
		// else if (i == pointLightsIndices[2]) 
		// {
		// 	result += CalculatePointLight(pointLights[i], N,FragPos, textureFrag);
		// }
		// else if (i == pointLightsIndices[3]) 
		// {
		// 	result += CalculatePointLight(pointLights[i], N,FragPos, textureFrag);
		// }
		// else if (i == pointLightsIndices[4]) 
		// {
		// 	result += CalculatePointLight(pointLights[i], N,FragPos, textureFrag);
		// }
		// else if (i == pointLightsIndices[5]) 
		// {
		// 	result += CalculatePointLight(pointLights[i], N,FragPos, textureFrag);
		// }
	// }
	result *= color.xyz;

	oColor = float4(result, alpha);
}