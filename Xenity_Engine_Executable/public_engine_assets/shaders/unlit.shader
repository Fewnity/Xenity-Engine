//-------------- {pc}
//-------------- {vertex}

#version 330 core

layout(location = 0) in vec2 a_TexCoord;
layout(location = 2) in vec3 a_Position;
layout(location = 3) in vec4 a_Color;

out vec2 v_TexCoord;
out vec4 v_Color;

uniform mat4 MVP; // Model View Projection

void main()
{
	gl_Position = MVP * vec4(a_Position, 1);
	v_TexCoord = a_TexCoord;
	v_Color = a_Color;
}

//-------------- {fragment}

#version 330 core

in vec2 v_TexCoord;
in vec4 v_Color;

uniform sampler2D diffuse;
uniform vec4 color;
uniform vec2 tiling;
uniform vec2 offset;

void main()
{
	vec4 textureFrag = texture(diffuse, (v_TexCoord * tiling) + offset);
	vec3 result = color.xyz * textureFrag.xyz; // Multiply color material and texture fragment

	float alpha = textureFrag.w * color.w;

	gl_FragColor = vec4(result, alpha) * v_Color; //Add texture color
}

//-------------- {psvita}
//-------------- {vertex}

attribute vec3 a_Position;
attribute vec2 a_TexCoord;
attribute vec4 a_Color;

varying vec2 v_TexCoord;
varying vec4 v_Color;

uniform mat4 MVP;

void main()
{
	gl_Position = mul(float4(a_Position, 1.0f), MVP);
	v_TexCoord = a_TexCoord;
	v_Color = a_Color;
}

//-------------- {fragment}

varying vec2 v_TexCoord;
varying vec4 v_Color;

uniform sampler2D diffuse;
uniform vec4 color;
uniform vec2 tiling;
uniform vec2 offset;

void main()
{
	vec4 textureFrag = tex2D(diffuse, (v_TexCoord * tiling) + offset);

	vec3 result = color.xyz * textureFrag.xyz;
	float alpha = textureFrag.w * color.w;

	gl_FragColor = vec4(result, alpha) * v_Color; //Add texture color
}

//-------------- {ps3}
//-------------- {vertex}

void main
(
	float3 a_Position : POSITION,
	float2 a_TexCoord : TEXCOORD0,
	float4 a_Color : COLOR0,

	uniform float4x4 MVP,

	out float4 ePosition : POSITION,
	out float2 v_Texcoord : TEXCOORD0,
	out float4 v_Color : COLOR0
)
{
	ePosition = mul(float4(a_Position, 1.0f), MVP);
	v_Texcoord = a_TexCoord;
	v_Color = a_Color;
}

//-------------- {fragment}

void main
(
	float2 v_Texcoord : TEXCOORD0,
	float4 v_Color : COLOR0,

	uniform sampler2D texture,
	uniform vec4 color,
	uniform vec2 tiling,
	uniform vec2 offset,

	out float4 oColor
)
{
	float4 textureFrag = tex2D(texture, (v_Texcoord * tiling) + offset);

	float3 result = color.xyz * textureFrag.xyz;
	float alpha = textureFrag.w * color.w;
	
	oColor = float4(result, alpha) * v_Color;
}