#version 440

layout(location = 0)in vec3 a_ModelSpacePosition;
layout(location = 1)in vec3 a_Normal;
layout(location = 2)in vec3 a_Color;

layout(location = 0)out vec3 v_WorldSpacePosition;
layout(location = 1)out vec3 v_Normal;
layout(location = 2)out vec3 v_Color;

layout(row_major, binding = 0) uniform Camera{
	layout(row_major)mat4 u_Projection;
	layout(row_major)mat4 u_View;
};

void main() {
	v_WorldSpacePosition = a_ModelSpacePosition;
	v_Normal = a_Normal;
	v_Color  = a_Color;

	gl_Position = u_Projection * u_View * vec4(v_WorldSpacePosition.xyz, 1.0);
}