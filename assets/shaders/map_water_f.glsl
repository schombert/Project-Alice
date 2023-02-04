#version 430 core

in vec2 tex_coord;
layout (location = 0) out vec4 frag_color;
layout (binding = 4) uniform sampler2D water_normal;
layout (binding = 5) uniform sampler2D colormap_water;

// location 0 : offset
// location 1 : zoom
// location 2 : screen_size
layout (location = 3) uniform vec2 map_size;
layout (location = 4) uniform float time;

// sheet is composed of 64 files, in 4 cubes of 4 rows of 4 columns
// so each column has 8 tiles, and each row has 8 tiles too
float xx = 1 / map_size.x;
float yy = 1 / map_size.y;
vec2 pix = vec2(xx, yy);

// Water effect taken from Vic2 fx/water/PixelShader_HoiWater_2_0
const float WRAP = 0.8;
const float WaveModOne = 3.0;
const float WaveModTwo = 4.0;
const float SpecValueOne = 8.0;
const float SpecValueTwo = 2.0;
const float vWaterTransparens = 1.0; //more transparance lets you see more of background
const float vColorMapFactor = 1.0f; //how much colormap

vec4 get_water(vec2 tex_coord)
{
	vec3 WorldColorColor = texture(colormap_water, tex_coord).rgb;
	tex_coord *= 100.;
	tex_coord = tex_coord * 0.25 + time * 0.002;

	const vec3 eyeDirection = vec3(0.0, 1.0, 1.0);
	const vec3 lightDirection = vec3(0.0, 1.0, 1.0);

	vec2 coordA = tex_coord * 3 + vec2(0.10, 0.10);
	vec2 coordB = tex_coord * 1 + vec2(0.00, 0.10);
	vec2 coordC = tex_coord * 2 + vec2(0.00, 0.15);
	vec2 coordD = tex_coord * 5 + vec2(0.00, 0.30);

	// Uses textureNoTile for non repeting textures,
	// probably unnecessarily expensive
	vec4 vBumpA = texture(water_normal, coordA);
	coordB += vec2(0.03, -0.02) * time;
	vec4 vBumpB = texture(water_normal, coordB);
	coordC += vec2(0.03, -0.01) * time;
	vec4 vBumpC = texture(water_normal, coordC);
	coordD += vec2(0.02, -0.01) * time;
	vec4 vBumpD = texture(water_normal, coordD);

	vec3 vBumpTex = normalize(WaveModOne * (vBumpA.xyz + vBumpB.xyz +
		vBumpC.xyz + vBumpD.xyz) - WaveModTwo);


	vec3 eyeDir = normalize(eyeDirection);
	float NdotL = max(dot(eyeDir, (vBumpTex / 2)), 0);

	NdotL = clamp((NdotL + WRAP) / (1 + WRAP), 0.f, 1.f);
	NdotL = mix(NdotL, 1.0, 0.0);

	vec3 OutColor = NdotL * (WorldColorColor * vColorMapFactor);

	vec3	reflVector = -reflect(lightDirection, vBumpTex);
	float	specular = dot(normalize(reflVector), eyeDir);
	specular = clamp(specular, 0.0, 1.0);

	specular = pow(specular, SpecValueOne);
	specular = mix(specular, 0.0, 0.0);
	OutColor += (specular / SpecValueTwo);
	OutColor *= 1.5;

	return vec4(OutColor, vWaterTransparens);
}

void main() {
	frag_color = get_water(tex_coord);
}
