//-------------- {pc}
//-------------- {vertex}

#version 330 core

layout (location = 2) in vec3 position;
layout(location = 0) in vec2 uv;

out vec2 TexCoord;
out vec3 FragPos;

uniform mat4 model; //Model matrice position, rotation and scale
uniform mat4 MVP;

void main()
{
	gl_Position = MVP * vec4(position, 1);
	TexCoord = uv;
}

//-------------- {fragment}

#version 330 core

out vec4 FragColor;
uniform vec4 color;

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

varying vec2 TexCoord;
varying vec3 FragPos;

uniform mat4 model; //Model matrice position, rotation and scale
uniform mat4 MVP;

void main()
{
	gl_Position = mul(float4(position, 1.0f), MVP);
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
	float2 vertexTexcoord : TEXCOORD0,
	
	uniform float4x4 MVP,

	out float4 ePosition : POSITION,
	out float4 oPosition : TEXCOORD0,
	out float2 oTexcoord : TEXCOORD2
)
{
	ePosition = mul(float4(vertexPosition, 1.0f), MVP);
	oPosition = float4(vertexPosition, 1.0f);
	oTexcoord = vertexTexcoord;
}

//-------------- {fragment}

void main
(
	float4 position : TEXCOORD0,
	float2 texcoord : TEXCOORD2,
	
	uniform sampler2D texture,
	uniform vec4 color,
	uniform vec2 tiling,
	uniform vec2 offset,

	out float4 oColor
)
{
	float3 result = color.xyz * tex2D(texture, (texcoord * tiling) + offset).xyz;
	float alpha = tex2D(texture, (texcoord * tiling) + offset).w * color.w;
	
	oColor = float4(result, alpha);
}