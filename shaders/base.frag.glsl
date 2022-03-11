#version 440

struct PointLight {
	vec3 Position;
	float Radius;
	vec4 Color;
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
	int u_PointLightsCount;
};

vec3 NormalToColor(vec3 normal) {
	return normalize(normal) * 0.5 + vec3(0.5);
}

vec3 CalculateDiffuseLightColor() {
	vec3 diff_color = vec3(0);

	for(int i = 0; i<u_PointLightsCount; i++){
		vec3 frag_to_light = u_PointLights[i].Position - v_WorldSpacePosition;
		float diff = dot(normalize(frag_to_light), v_Normal);

		diff *= min(u_PointLights[i].Radius/length(frag_to_light), 1.0);
		
		diff_color += diff * u_PointLights[i].Color.rgb * u_PointLights[i].Color.a;
	}	

	return diff_color;
}

vec3 CalculateSpecularLightColor() {
	vec3 spec_color = vec3(0);

	for(int i = 0; i<u_PointLightsCount; i++){
		vec3 camera_direction = normalize(u_CameraPosition - v_WorldSpacePosition);
		vec3 light_direction =  reflect(-normalize(u_PointLights[i].Position - v_WorldSpacePosition), v_Normal);
		float spec = max(pow(dot(camera_direction, light_direction), 256), 0);
		
		spec_color += spec * u_PointLights[i].Color.rgb * u_PointLights[i].Color.a;
	}	

	return spec_color;
}

void main() {
	vec3 diff_color = CalculateDiffuseLightColor() + CalculateSpecularLightColor();

	f_Color = vec4(diff_color, 1.0);
}
