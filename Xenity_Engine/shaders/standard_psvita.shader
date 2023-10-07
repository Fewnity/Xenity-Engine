//-------------- {psvita}
{vertex}

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

mat4 inverse(mat4 m) {
  float
      a00 = m[0][0], a01 = m[0][1], a02 = m[0][2], a03 = m[0][3],
      a10 = m[1][0], a11 = m[1][1], a12 = m[1][2], a13 = m[1][3],
      a20 = m[2][0], a21 = m[2][1], a22 = m[2][2], a23 = m[2][3],
      a30 = m[3][0], a31 = m[3][1], a32 = m[3][2], a33 = m[3][3],

      b00 = a00 * a11 - a01 * a10,
      b01 = a00 * a12 - a02 * a10,
      b02 = a00 * a13 - a03 * a10,
      b03 = a01 * a12 - a02 * a11,
      b04 = a01 * a13 - a03 * a11,
      b05 = a02 * a13 - a03 * a12,
      b06 = a20 * a31 - a21 * a30,
      b07 = a20 * a32 - a22 * a30,
      b08 = a20 * a33 - a23 * a30,
      b09 = a21 * a32 - a22 * a31,
      b10 = a21 * a33 - a23 * a31,
      b11 = a22 * a33 - a23 * a32,

      det = b00 * b11 - b01 * b10 + b02 * b09 + b03 * b08 - b04 * b07 + b05 * b06;

  return mat4(
      a11 * b11 - a12 * b10 + a13 * b09,
      a02 * b10 - a01 * b11 - a03 * b09,
      a31 * b05 - a32 * b04 + a33 * b03,
      a22 * b04 - a21 * b05 - a23 * b03,
      a12 * b08 - a10 * b11 - a13 * b07,
      a00 * b11 - a02 * b08 + a03 * b07,
      a32 * b02 - a30 * b05 - a33 * b01,
      a20 * b05 - a22 * b02 + a23 * b01,
      a10 * b10 - a11 * b08 + a13 * b06,
      a01 * b08 - a00 * b10 - a03 * b06,
      a30 * b04 - a31 * b02 + a33 * b00,
      a21 * b02 - a20 * b04 - a23 * b00,
      a11 * b07 - a10 * b09 - a12 * b06,
      a00 * b09 - a01 * b07 + a02 * b06,
      a31 * b01 - a30 * b03 - a32 * b00,
      a20 * b03 - a21 * b01 + a22 * b00) / det;
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

{fragment}

uniform sampler2D textureBase;

struct PointLight {
	vec3 position;
	float constant;
	vec3 color;


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

float3 CalculateDirectionalLight(DirectionalLight light, vec3 norm, vec3 fragPos, vec2 texcoords, vec3 textureColor) 
{
	float3 tx = textureColor * light.color;
	return tx;
}

float3 CalculatePointLight(PointLight light, float3 norm, float3 fragPos, float2 texcoords, vec3 textureColor) 
{
	float3 lightDir = normalize(light.position - fragPos); //Direction of the point light between the light source and the face
	float diff = max(dot(norm, lightDir), 0.0f); //If the light is behind the face, diff is 0

	float3 diffuse = (diff * textureColor) * light.color * 2; //Set the light color and intensity TODO
	float distance2 = length(light.position - fragPos);
	float attenuation = 1.0f / (light.constant + light.linear * distance2 + light.quadratic * (distance2 * distance2));

	return diffuse * attenuation;
}

void main()
{
	vec3 result = float3(0.0f, 0.0f, 0.0f);
	vec3 textureColor = tex2D(textureBase, TexCoord).xyz;

	vec3 norm = normalize(Normals); //Direction of normals

	//for (int i = 0; i < usedPointLightCount; i++)
	for (int i = 0; i < 3; i++)
	{
		result += CalculatePointLight(pointLights[i], norm, FragPos, TexCoord, textureColor);
	}


	//for (int i = 0; i < usedDirectionalLightCount; i++)
	for (int i = 0; i < 1; i++)
	{
		result += CalculateDirectionalLight(directionalLights[i], norm, FragPos, TexCoord, textureColor);
	}



	float alpha = tex2D(material.diffuse, TexCoord).a;
	gl_FragColor = vec4(result, alpha); //Add texture color
}