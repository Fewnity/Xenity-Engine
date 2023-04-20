#version 450 core

layout(triangles, equal_spacing, ccw) in;

in vec2 TexCoord3[];
in vec3 Normal3[];
in vec3 FragPos3[];

out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;

void main(void)
{
    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;
    float w = gl_TessCoord.z;

    vec4 pos0 = gl_in[0].gl_Position;
    vec4 pos1 = gl_in[1].gl_Position;
    vec4 pos2 = gl_in[1].gl_Position;

    TexCoord = u * TexCoord3[0] + v * TexCoord3[1] + w * TexCoord3[2];
    Normal = u * Normal3[0] + v * Normal3[1] + w * Normal3[2];
    FragPos = u * FragPos3[0] + v * FragPos3[1] + w * FragPos3[2];

    /*TexCoord = TexCoord3[u * pos0 + v * pos1 + w * pos2];
    Normal = Normal3[u * pos0 + v * pos1 + w * pos2];
    FragPos = FragPos3[u * pos0 + v * pos1 + w * pos2];*/

    //gl_TessCoord is the input storing normalized coordinates data of the generated vertice
    gl_Position = (gl_TessCoord.x * gl_in[0].gl_Position +
        gl_TessCoord.y * gl_in[1].gl_Position +
        gl_TessCoord.z * gl_in[2].gl_Position);
}