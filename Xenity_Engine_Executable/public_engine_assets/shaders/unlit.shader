//-------------- {pc}
//-------------- {vertex}

#version 330 core

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
}

//-------------- {fragment}

#version 330 core

out vec4 FragColor;
uniform vec4 color;
uniform vec3 cameraPos;

in vec2 TexCoord;

uniform vec2 tiling;
uniform vec2 offset;

struct Material {
	sampler2D diffuse;
	vec3 ambient;
};

uniform Material material;

void main()
{
	// Ambient

	vec3 ambient = color.xyz * vec3(texture(material.diffuse, (TexCoord * tiling) + offset)); //Get ambient intensity and color

	//Result
	vec3 result = ambient; //Set face result

	float alpha = texture(material.diffuse, (TexCoord * tiling) + offset).a * color.w;
	FragColor = vec4(result, alpha); //Add texture color
}

//-------------- {psvita}
//-------------- {vertex}

attribute vec3 position;
attribute vec2 uv;
attribute vec3 normal;

varying vec2 TexCoord;
varying vec3 Normal;
varying vec3 FragPos;

uniform mat4 camera;
uniform mat4 projection;

uniform mat4 model; //Model matrice position, rotation and scale

void main()
{
	//gl_Position = projection * camera * model * vec4(position, 1);
	gl_Position = mul(float4(position, 1.0f), mul(model, mul(camera, projection)));
	TexCoord = uv;
}

{fragment}

uniform vec4 color;

varying vec2 TexCoord;

struct Material {
	sampler2D diffuse;
	vec3 ambient;
};

uniform Material material;
uniform vec2 tiling;
uniform vec2 offset;

void main()
{
	// Ambient

	vec3 ambient = color.xyz * vec3(tex2D(material.diffuse, (TexCoord * tiling) + offset)); //Get ambient intensity and color

	//Result
	vec3 result = ambient; //Set face result

	float alpha = tex2D(material.diffuse, (TexCoord * tiling) + offset).a * color.w;
	gl_FragColor = vec4(result, alpha); //Add texture color
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
	out float2 oTexcoord : TEXCOORD2
)
{
	ePosition = mul(float4(vertexPosition, 1.0f), mul(camera, mul(model, projection)));
	oPosition = float4(vertexPosition, 1.0f);
	oNormal = vertexNormal;
	oTexcoord = vertexTexcoord;
}

//-------------- {fragment}

void main
(
	float4 position : TEXCOORD0,
	float3 normal : TEXCOORD1,
	float2 texcoord : TEXCOORD2,
	
	uniform sampler2D texture,
	// uniform vec4 color;
	// uniform vec2 tiling;
	// uniform vec2 offset;

	out float4 oColor
)
{
	// float3 result = color.xyz * tex2D(texture,texcoord).xyz;
	// float alpha = tex2D(texture,texcoord).w * color.w;
	
	float3 result = tex2D(texture,texcoord).xyz;
	float alpha = tex2D(texture,texcoord).w;

	oColor = float4(result, alpha);
}