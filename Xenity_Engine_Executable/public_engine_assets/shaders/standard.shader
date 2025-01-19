//-------------- {pc}
//-------------- {vertex}

#version 330

layout(location = 0) in vec2 uv;
layout(location = 1) in vec3 normal;
layout (location = 2) in vec3 position;
layout(location = 3) in vec4 color2;

out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;
out vec4 VertexColor;

uniform mat4 model; //Model matrice position, rotation and scale
uniform mat4 MVP;
uniform mat3 normalMatrix;

void main()
{
	gl_Position = MVP * vec4(position, 1);
	TexCoord = uv;
	FragPos = vec3(model * vec4(position, 1));
	Normal = normalMatrix * normal; //TODO Check an object with a bigger scale and with a 	offsetPosition, fix : add to offset * rotation this : * offsetPosition * scale
	VertexColor = color2;
}

//-------------- {fragment}

#version 330

uniform vec3 ambiantLightColor;
uniform vec4 color;
in vec3 Normal;
in vec3 FragPos;
in vec4 VertexColor;
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

vec3 CalculateDirectionalLight(DirectionalLight light2, vec3 norm, vec3 fragPos) 
{
	vec3 lightDir = normalize(-light2.direction); // Directional light
	float diff = max(dot(norm, lightDir), 0.0); //If the light is behind the face, diff is 0
	vec3 diffuse = (diff * vec3(texture(material.diffuse, (TexCoord * tiling) + offset))) * light2.color * 2; //Set the light color and intensity TODO : Change the ambiantLightColor by the light color

	return diffuse;
}

vec3 CalculatePointLight(PointLight light2, vec3 norm, vec3 fragPos) 
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

vec3 CalculateSpotLight(SpotLight light2, vec3 norm, vec3 fragPos) 
{
	vec3 lightDir = normalize(light2.position - fragPos); //Direction of the point light between the light source and the face
	float diff = max(dot(norm, lightDir), 0.0); //If the light is behind the face, diff is 0
	vec3 diffuse = (diff * vec3(texture(material.diffuse, (TexCoord * tiling) + offset))) * light2.color * 2; //Set the light color and intensity TODO : Change the ambiantLightColor by the light color

	float distance = length(light2.position - fragPos);
	float attenuation = 1.0 / (light2.constant + light2.linear * distance + light2.quadratic * (distance * distance));

	float theta = dot(lightDir, normalize(-light2.direction));
	float epsilon = light2.cutOff - light2.outerCutOff;
	float intensity = clamp((theta - light2.outerCutOff) / epsilon, 0.0, 1.0);
	diffuse *= intensity;
	// specular *= intensity;

	//Result
	vec3 result = (diffuse * attenuation); //Set face result
	return result;
}

void main()
{
	vec3 ambient = vec3(0,0,0);
	// Diffuse
	vec3 norm = normalize(Normal); //Direction of normals

	//Result
	vec3 result = ambient; //Set face result
	for (int i = 0; i < lightIndices.usedPointLightCount; i++)
	{
		result += CalculatePointLight(pointLights[lightIndices.pointLightsIndices[i].x], norm, FragPos);
	}	
	for (int i = 0; i < lightIndices.usedSpotLightCount; i++)
	{
		result += CalculateSpotLight(spotLights[lightIndices.spotLightsIndices[i].x], norm, FragPos);
	}
	for (int i = 0; i < lightIndices.usedDirectionalLightCount; i++)
	{
		result += CalculateDirectionalLight(directionalLights[lightIndices.directionalLightsIndices[i].x], norm, FragPos);
	}
	result += vec3(texture(material.diffuse, (TexCoord * tiling) + offset)) * ambientLight;

	float alpha = texture(material.diffuse, (TexCoord * tiling) + offset).a* color.w;

	gl_FragColor = vec4(result * color.xyz, alpha) * VertexColor; //Add texture color
}

//-------------- {psvita}
//-------------- {vertex}

attribute vec2 uv;
attribute vec3 normal;
attribute vec3 position;
attribute vec4 color2;

varying vec2 TexCoord;
varying vec3 Normals;
varying vec3 FragPos;
varying vec4 VertexColor;

uniform mat4 model; //Model matrice position, rotation and scale
uniform mat4 MVP;
uniform mat3 normalMatrix;

void main()
{
	gl_Position = mul(float4(position, 1.0f), MVP);
	FragPos = float3(mul(float4(position, 1.0f), model));	
	Normals = mul(normal, normalMatrix);
	TexCoord = uv;
	VertexColor = color2;
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

uniform vec4 color;

varying vec2 TexCoord;
varying vec3 Normals;
varying vec3 FragPos;
varying vec4 VertexColor;

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

float3 CalculateDirectionalLight(DirectionalLight light, vec3 norm, vec3 textureColor) 
{
	float3 lightDir = normalize(-light.direction); // Directional light
	float diff = max(dot(norm, lightDir), 0.0f); //If the light is behind the face, diff is 0
	float3 tx = (diff * textureColor) * light.color; //Set the light color and intensity TODO : Change the ambiantLightColor by the light color
	return tx;
}

vec3 CalculateSpotLight(SpotLight light, vec3 norm, vec3 fragPos, vec3 textureColor) 
{
	vec3 lightDir = normalize(light.position - fragPos); //Direction of the point light between the light source and the face
	float diff = max(dot(norm, lightDir), 0.0); //If the light is behind the face, diff is 0
	vec3 diffuse = (diff * textureColor) * light.color * 2; //Set the light color and intensity TODO : Change the ambiantLightColor by the light color

	float distance = length(light.position - fragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

	float theta = dot(lightDir, normalize(-light.direction));
	float epsilon = light.cutOff - light.outerCutOff;
	float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
	diffuse *= intensity;

	vec3 result = (diffuse * attenuation); //Set face result
	return result;
}

float3 CalculatePointLight(PointLight light, float3 norm, float3 fragPos, float3 textureColor) 
{
	float3 lightVec = light.position - fragPos;
	float3 lightDir = normalize(lightVec); //Direction of the point light between the light source and the face
	//float3 lightDir = normalize(light.position - fragPos); //Direction of the point light between the light source and the face
	float diff = max(dot(norm, lightDir), 0.0f); //If the light is behind the face, diff is 0
	float3 diffuse = (diff * textureColor) * light.color * 2; //Set the light color and intensity

	float distanceSq = dot(lightVec, lightVec); // distance²
	float attenuation = 1.0f / (light.light_data.x + light.light_data.y * sqrt(distanceSq) + light.light_data.z * distanceSq);
	//float distance = length(light.position - fragPos);
	//float attenuation = 1.0 / (light.light_data.x + light.light_data.y * distance + light.light_data.z * (distance * distance));
	return diffuse * attenuation;
}

void main()
{
	vec3 result = float3(0.0f, 0.0f, 0.0f);
	vec3 norm = normalize(Normals); //Direction of normals
	vec4 textureColor = tex2D(material.diffuse, (TexCoord * tiling) + offset);

	// Apply lights
	// Unrool for loop for better performances
	int currentPointLight = 0;
	if(lightIndices.usedPointLightCount > currentPointLight)
	{
	 	result += CalculatePointLight(pointLights[lightIndices.pointLightsIndices[currentPointLight].x], norm, FragPos, textureColor);
	 	currentPointLight += 1;
		if(lightIndices.usedPointLightCount > currentPointLight)
		{
	 		result += CalculatePointLight(pointLights[lightIndices.pointLightsIndices[currentPointLight].x], norm, FragPos, textureColor);
	 		currentPointLight += 1;
			if(lightIndices.usedPointLightCount > currentPointLight)
			{
	 			result += CalculatePointLight(pointLights[lightIndices.pointLightsIndices[currentPointLight].x], norm, FragPos, textureColor);
	 			currentPointLight += 1;
			}
		}
	}

	int currentSpotLight = 0;
	if(lightIndices.usedSpotLightCount > currentSpotLight)
	{
		result += CalculateSpotLight(spotLights[lightIndices.spotLightsIndices[currentSpotLight].x], norm, FragPos, textureColor);
		currentSpotLight++;
		if(lightIndices.usedSpotLightCount > currentSpotLight)
		{
			result += CalculateSpotLight(spotLights[lightIndices.spotLightsIndices[currentSpotLight].x], norm, FragPos, textureColor);
			currentSpotLight++;
		}
	}

	int currentDirectionalLight = 0;
	if(lightIndices.usedDirectionalLightCount > currentDirectionalLight)
	{
		result += CalculateDirectionalLight(directionalLights[lightIndices.directionalLightsIndices[currentDirectionalLight].x], norm, textureColor);
		currentDirectionalLight++;
		if(lightIndices.usedDirectionalLightCount > currentDirectionalLight)
		{
			result += CalculateDirectionalLight(directionalLights[lightIndices.directionalLightsIndices[currentDirectionalLight].x], norm, textureColor);
			currentDirectionalLight++;
		}
	}

	// Apply ambient light
	result += textureColor.xyz * ambientLight;

	float alpha = textureColor.w * color.w;
	gl_FragColor = vec4(result * color.xyz, alpha); //Add texture color
}

//-------------- {ps3}
//-------------- {vertex}

void main
(
	float3 vertexPosition : POSITION,
	float3 vertexNormal : NORMAL,
	float2 vertexTexcoord : TEXCOORD0,
	
	uniform float4x4 model,
	uniform float4x4 MVP,
	uniform float3x3 normalMatrix,

	out float4 ePosition : POSITION,
	out float3 oNormal : TEXCOORD0,
	out float2 oTexcoord : TEXCOORD1,
	out float3 oFragPos : TEXCOORD2
)
{
	ePosition = mul(float4(vertexPosition, 1.0f), MVP);
	oFragPos = float3(mul(float4(vertexPosition, 1.0f), model));	
	oNormal = mul(vertexNormal, normalMatrix);
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
	//float attenuation = 1.0f / (light.light_data.x + light.light_data.y * distance + light.light_data.z * (distance * distance));

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
	float3 normal : TEXCOORD0,
	float2 texcoord : TEXCOORD1,
	float3 FragPos : TEXCOORD2,

	uniform sampler2D texture,
	// uniform sampler2D lightingDataTexture,

	uniform float3 ambientLight,
	uniform vec4 color,
	uniform vec2 tiling,
	uniform vec2 offset,
	
	uniform PointLight pointLights[NR_POINT_LIGHTS],
	uniform SpotLight spotLights[NR_SPOT_LIGHTS],
	uniform DirectionalLight directionalLights[NR_DIRECTIONAL_LIGHTS],

	// uniform float usedPointLightCount,
	// uniform float usedSpotLightCount,
	// uniform float usedDirectionalLightCount,

	// uniform float pointLightsIndices[NR_POINT_LIGHTS],
	// uniform float spotLightsIndices[NR_SPOT_LIGHTS],
	// uniform float directionalLightsIndices[NR_DIRECTIONAL_LIGHTS],

	out float4 oColor
)
{
	float4 textureFrag = tex2D(texture, (texcoord * tiling) + offset);

	float3 result = float3(0,0,0);
	result += textureFrag.xyz * ambientLight;
	//result += tex2D(lightingDataTexture, float2(1, 1)).xyz;
	float alpha = textureFrag.w * color.w;
	float3 N = normalize(normal);

	result += CalculateDirectionalLight(directionalLights[1], N, textureFrag);

	result += CalculatePointLight(pointLights[1], N, FragPos, textureFrag);
	//result += CalculatePointLight(pointLights[2], N, FragPos, textureFrag);

	result *= color.xyz;

	oColor = float4(result, alpha);
}