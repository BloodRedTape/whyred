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

layout(row_major, binding = 3) uniform ShadowCamera{
	mat4 u_ShadowProjection;
	mat4 u_ShadowView;
};

layout(binding = 4)uniform sampler2D u_ShadowMap;

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
			attenumation = pow(1 - clamp((Deg(angle) - u_Spotlights[i].CutoffAngle)/u_Spotlights[i].DimAngle, 0.0, 1.0), 2);
			color += Diffuse(frag_to_light, u_Spotlights[i].Color) * attenumation;
			color += Specular(frag_to_light, u_Spotlights[i].Color);
		}
	}
	return color;
}



vec3 GetFragmentGlPosition(){
	vec4 res = u_ShadowProjection * u_ShadowView * vec4(v_WorldSpacePosition.xyz, 1);
	return res.xyz / res.w;
}

vec2 GetFragmentUV(){
	return GetFragmentGlPosition().xy * 0.5 + 0.5;
}

int Seed = 0;
int RandMax = 32767;

int RandInt() // RAND_MAX assumed to be 32767
{
    Seed = Seed * 645615245 + 7919;
    return int(uint(Seed/65536) % 32768);
}

float RandFloat(){
    return RandInt() / float(RandMax);
}

#define NUM_SHADOW_SAMPLES 64
#define SAMPLE_SPAN 2

float GetFragmentDepth(){
	return GetFragmentGlPosition().z;
}

float GetShadowMapDepth(){
	Seed = int((GetFragmentUV().x + GetFragmentUV().y) * 10000);

	float frag_depth = GetFragmentDepth();
	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(u_ShadowMap, 0);

	for(int i = 0; i<NUM_SHADOW_SAMPLES; i++){
		float pcfDepth = texture(u_ShadowMap, GetFragmentUV().xy + vec2(RandFloat()*SAMPLE_SPAN, RandFloat()*SAMPLE_SPAN) * texelSize).r; 
		shadow += (frag_depth > pcfDepth + 0.001) ? 0.0 : 1.0;        
	}
	shadow /= float(NUM_SHADOW_SAMPLES);
	return shadow;
	return texture(u_ShadowMap, GetFragmentUV()).r + 0.0001;
}

float EvalueateShadow(){
	return GetShadowMapDepth();	
}

void main() {
	vec3 color = vec3(0);

	color += EvalueatePointLights();
	color += EvalueateDirLights();
	color += EvalueateSpotlights();
	color *= EvalueateShadow();

	//color = vec3(GetShadowMapDepth());

	f_Color = vec4(color, 1.0);
}
