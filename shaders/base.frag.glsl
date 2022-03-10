#version 440

layout(location = 0)in vec3 v_WorldSpacePosition;
layout(location = 1)in vec3 v_Normal;
layout(location = 2)in vec3 v_Color;

layout(location = 0)out vec4 f_Color;

vec3 NormalToColor(vec3 normal) {
	return normalize(normal) * 0.5 + vec3(0.5);
}

void main() {
	f_Color = vec4(NormalToColor(v_Normal).xyz, 1.0);
}
