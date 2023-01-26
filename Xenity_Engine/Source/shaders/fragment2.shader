#version 330 core
out vec4 FragColor;
uniform vec4 ourColor;
in vec4 theColor;

in vec2 TexCoord;
uniform sampler2D ourTexture;
uniform sampler2D ourTexture2;

void main()
{
	//FragColor = ourColor;
	vec4 colTex1 = texture(ourTexture, TexCoord);
	vec4 colTex2 = texture(ourTexture2, TexCoord);
	FragColor = mix(colTex1, colTex2, 0.5);
	//FragColor = texture(ourTexture, TexCoord) * ourColor;
}