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
	//vec3 diffuse;
	//vec3 specular;
	float shininess;
};

uniform Material material;

void main()
{
	//FragColor = ourColor;
	//FragColor = theColor * ourColor;
	//FragColor = texture(ourTexture, TexCoord) * theColor; //With color coors
	//FragColor = texture(ourTexture, TexCoord) * vec4(ambiantLightColor,1.0); //With color coors

	// Ambint
	float ambientStrength = 1;
	//vec3 ambient = ambientStrength * material.ambient; //Get ambient intensity and color
	vec3 ambient = ambientStrength * material.ambient * vec3(texture(material.diffuse, TexCoord)); //Get ambient intensity and color
	// Diffuse
	vec3 norm = normalize(Normal); //Direction of normals
	vec3 lightDir = normalize(lightPos - FragPos); //Direction of the light between the light source and the face
	float diff = max(dot(norm, lightDir), 0.0); //If the light is behind the face, diff is 0
	//vec3 diffuse = (diff * material.diffuse) * ambiantLightColor * 2; //Set the light color and intensity TODO : Change the ambiantLightColor by the light color
	vec3 diffuse = (diff * vec3(texture(material.diffuse, TexCoord))) * ambiantLightColor * 1; //Set the light color and intensity TODO : Change the ambiantLightColor by the light color

	//Spectacular
	float specularStrength = 0.5;
	vec3 viewDir = normalize(cameraPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = specularStrength * (spec* vec3(texture(material.specular, TexCoord))) * ambiantLightColor;

	//Result
	vec3 result = (ambient + diffuse + specular); //Set face result
	//FragColor = texture(ourTexture, TexCoord) * vec4(result, 1.0); //Add texture color
	FragColor = vec4(result, 1.0); //Add texture color
}