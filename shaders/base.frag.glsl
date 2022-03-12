#version 440

struct PointLight {
	vec3 Position;
	float Radius;
	vec4 Color;
};

struct DirLight {
	vec4 Color;
	vec3 Direction;
};

struct Spotlight {
    vec3 Position;
    float CutoffAngle;
	vec4 Color;
    vec3 Direction;
    float DimAngle;
};

layout(location = 0)in vec3 v_WorldSpacePosition;
layout(location = 1)in vec3 v_Normal;
layout(location = 2)in vec3 v_Color;

layout(location = 0)out vec4 f_Color;

layout(row_major, binding = 0) uniform Camera{
	mat4 u_Projection;
	mat4 u_View;
	vec3 u_CameraPosition;
};

layout(binding = 2)uniform Lights {
	PointLight u_PointLights[32];
	int		   u_PointLightsCount;
    DirLight   u_DirLights[32];
    int        u_DirLightsCount;
    Spotlight  u_Spotlights[32];
    int        u_SpotlightsCount;
};

float Rad(float degrees) {
	return degrees / 180 * 3.14159265358979323;
}
float Deg(float radians) {
	return radians / 3.14159265358979323 * 180;
}

vec3 NormalToColor(vec3 normal) {
	return normalize(normal) * 0.5 + vec3(0.5);
}

vec3 Diffuse(vec3 frag_to_light, vec4 color) {
	float diff = dot(normalize(frag_to_light), v_Normal);

	return color.rgb * color.a * diff;
}

vec3 Specular(vec3 frag_to_light, vec4 color) {
	vec3 camera_direction = normalize(u_CameraPosition - v_WorldSpacePosition);
	vec3 ref_light_dir =  reflect(-normalize(frag_to_light), v_Normal);
	float spec = max(pow(dot(camera_direction, ref_light_dir), 256), 0);

	return color.rgb * color.a * spec;
}

vec3 EvalueatePointLights() {
	vec3 color = vec3(0);

	for (int i = 0; i < u_PointLightsCount; i++) {
		vec3 frag_to_light = u_PointLights[i].Position - v_WorldSpacePosition;
		float attenuation = min(u_PointLights[i].Radius/length(frag_to_light), 1.0);

		color += Diffuse(frag_to_light, u_PointLights[i].Color) * attenuation;
		color += Specular(frag_to_light, u_PointLights[i].Color);
	}
	
	return color;
}

vec3 EvalueateDirLights() {
	vec3 color = vec3(0);

	for (int i = 0; i < u_DirLightsCount; i++) {
		color += Diffuse(u_DirLights[i].Direction, u_DirLights[i].Color);
		color += Specular(u_DirLights[i].Direction, u_DirLights[i].Color);
	}

	return color;
}

vec3 EvalueateSpotlights() {
	vec3 color = vec3(0);

	for (int i = 0; i < u_SpotlightsCount; i++) {
		vec3 frag_to_light = u_Spotlights[i].Position - v_WorldSpacePosition;
		float outer_angle = u_Spotlights[i].CutoffAngle + u_Spotlights[i].DimAngle;
		float angle = acos(dot(normalize(frag_to_light), normalize(-u_Spotlights[i].Direction)));

		if(angle < Rad(outer_angle)){
			float attenumation = 1;
			attenumation = pow(1 - clamp((Deg(angle) - u_Spotlights[i].CutoffAngle)/u_Spotlights[i].DimAngle, 0, 1.0), 2);
			color += Diffuse(frag_to_light, u_Spotlights[i].Color) * attenumation;
			color += Specular(frag_to_light, u_Spotlights[i].Color);
		}
	}
	return color;
}

void main() {
	vec3 color = vec3(0);

	color += EvalueatePointLights();
	color += EvalueateDirLights();
	color += EvalueateSpotlights();

	f_Color = vec4(color, 1.0);
}
