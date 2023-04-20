#version 450 core

layout(vertices = 3) out;			//vertices = number of control points / patch

//in vec2 TexCoord[];
//out vec2 TextureCoord[];

in vec2 TexCoord[];
in vec3 Normal[];
in vec3 FragPos[];
 
out vec2 TexCoord3[];
out vec3 Normal3[];
out vec3 FragPos3[];

void main(void)
{	
	TexCoord3[gl_InvocationID] = TexCoord[gl_InvocationID];
	FragPos3[gl_InvocationID] = FragPos[gl_InvocationID];
	Normal3[gl_InvocationID] = Normal[gl_InvocationID];

	//TextureCoord[gl_InvocationID] = TexCoord[gl_InvocationID];
	//tessellation 0 controls the level of tessellation for the entire patch
	if (gl_InvocationID == 0) {
		gl_TessLevelInner[0] = 5.0;
		//gl_TessLevelOuter[i] for each vertice => 3 here
		//number of divisions for each side of the patch
		gl_TessLevelOuter[0] = 5.0;
		gl_TessLevelOuter[1] = 5.0;
		gl_TessLevelOuter[2] = 5.0;
	}
	//Now copy its input into its output with gl_out and gl_in
	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}
