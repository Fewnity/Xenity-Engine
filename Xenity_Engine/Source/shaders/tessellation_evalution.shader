#version 450 core

layout(vertices = 3) out;			//vertices = number of control points / patch

void main(void)
{	//tessellation 0 controls the level of tessellation for the entire patch
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
