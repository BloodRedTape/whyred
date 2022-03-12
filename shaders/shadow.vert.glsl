#version 440

layout(location = 0)in vec3 a_ModelSpacePosition;
layout(location = 1)in vec3 a_Normal;
layout(location = 2)in vec3 a_Color;

layout(row_major, binding = 0) uniform Camera{
	mat4 u_Projection;
	mat4 u_View;
};

layout(row_major, binding = 1) uniform Model{
	mat4 u_Model;
};

void main() {
	gl_Position = u_Projection * u_View * u_Model * vec4(a_ModelSpacePosition.xyz, 1.0);
}