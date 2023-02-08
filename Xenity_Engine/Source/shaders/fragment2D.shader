#version 330 core
in vec2 TexCoords;
out vec4 color;

uniform sampler2D text;
uniform vec3 spriteColor;

void main()
{
    vec4 sampled = vec4(texture(text, TexCoords).r, texture(text, TexCoords).g, texture(text, TexCoords).b, texture(text, TexCoords).a);
    color = vec4(spriteColor, 1) * sampled;
}