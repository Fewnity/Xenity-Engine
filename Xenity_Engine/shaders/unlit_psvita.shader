//-------------- {psvita}
{vertex}

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

uniform vec3 color;

varying vec2 TexCoord;

struct Material {
	sampler2D diffuse;
	vec3 ambient;
};

uniform Material material;

void main()
{
	// Ambient

	vec3 ambient = color * vec3(tex2D(material.diffuse, TexCoord)); //Get ambient intensity and color

	//Result
	vec3 result = ambient; //Set face result

	float alpha = tex2D(material.diffuse, TexCoord).a;
	gl_FragColor = vec4(result, alpha); //Add texture color
}