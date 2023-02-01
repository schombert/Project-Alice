#version 430 core

in vec2 tex_coord;
layout (location = 0) out vec4 frag_color;
layout (binding = 0) uniform sampler2D provinces_texture_sampler;
layout (binding = 1) uniform sampler2D terrain_texture_sampler;
layout (binding = 2) uniform sampler2D rivers_texture_sampler;
layout (binding = 3) uniform sampler2DArray terrainsheet_texture_sampler;
layout (binding = 4) uniform sampler2D water_normal;
layout (binding = 5) uniform sampler2D colormap_water;
layout (binding = 6) uniform sampler2D colormap_terrain;
layout (binding = 6) uniform sampler2D overlay;

// location 0 : offset
// location 1 : zoom
// location 2 : screen_size
layout (location = 3) uniform vec2 map_size;
layout (location = 4) uniform float time;

// sheet is composed of 64 files, in 4 cubes of 4 rows of 4 columns
// so each column has 8 tiles, and each row has 8 tiles too

const float xx = 1 / map_size.x;
const float yy = 1 / map_size.y;
const vec2 pix = vec2(xx, yy);

vec4 hash4( vec2 p ) { return fract(sin(vec4( 1.0+dot(p,vec2(37.0,17.0)),
                                              2.0+dot(p,vec2(11.0,47.0)),
                                              3.0+dot(p,vec2(41.0,29.0)),
                                              4.0+dot(p,vec2(23.0,31.0))))*103.0); }

// Makes textures non repetative https://www.iquilezles.org/www/articles/texturerepetition/texturerepetition.htm
vec4 textureNoTile(sampler2D samp, vec2 uv)
{
	vec2 iuv = floor(uv);
	vec2 fuv = fract(uv);

	// generate per-tile transform
	vec4 ofa = hash4(iuv + vec2(0, 0));
	vec4 ofb = hash4(iuv + vec2(1, 0));
	vec4 ofc = hash4(iuv + vec2(0, 1));
	vec4 ofd = hash4(iuv + vec2(1, 1));

	vec2 dDX = dFdx(uv);
	vec2 dDY = dFdy(uv);

	// transform per-tile uvs
	ofa.zw = sign(ofa.zw - 0.5);
	ofb.zw = sign(ofb.zw - 0.5);
	ofc.zw = sign(ofc.zw - 0.5);
	ofd.zw = sign(ofd.zw - 0.5);

	// uv's, and derivatives (for correct mipmapping)
	vec2 uva = uv * ofa.zw + ofa.xy, ddxa = dDX * ofa.zw, ddya = dDY * ofa.zw;
	vec2 uvb = uv * ofb.zw + ofb.xy, ddxb = dDX * ofb.zw, ddyb = dDY * ofb.zw;
	vec2 uvc = uv * ofc.zw + ofc.xy, ddxc = dDX * ofc.zw, ddyc = dDY * ofc.zw;
	vec2 uvd = uv * ofd.zw + ofd.xy, ddxd = dDX * ofd.zw, ddyd = dDY * ofd.zw;

	// fetch and blend
	vec2 b = smoothstep(0.25, 0.75, fuv);

	return mix(mix(textureGrad(samp, uva, ddxa, ddya),
		textureGrad(samp, uvb, ddxb, ddyb), b.x),
		mix(textureGrad(samp, uvc, ddxc, ddyc),
			textureGrad(samp, uvd, ddxd, ddyd), b.x), b.y);
}

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

	vec3 eyeDirection = vec3(0.0, 1.0, 1.0);
	vec3 lightDirection = vec3(0.0, 1.0, 1.0);

	vec2 coordA = tex_coord * 3;
	coordA.xy += 0.1;
	vec2 coordB = tex_coord;
	coordB.y += 0.1;
	vec2 coordC = tex_coord * 2;
	coordC.y += 0.15;
	vec2 coordD = tex_coord * 5;
	coordD.y += 0.3;

	// Uses textureNoTile for non repeting textures,
	// probably unnecessarily expensive
	vec4 vBumpA = textureNoTile(water_normal, coordA);
	coordB.x += 0.03 * time;
	coordB.y -= 0.02 * time;
	vec4 vBumpB = textureNoTile(water_normal, coordB);
	coordC.x += 0.03 * time;
	coordC.y -= 0.01 * time;
	vec4 vBumpC = textureNoTile(water_normal, coordC);
	coordD.x += 0.02 * time;
	coordD.y -= 0.01 * time;
	vec4 vBumpD = textureNoTile(water_normal, coordD);

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

vec4 get_terrain(vec2 tex_coords, vec2 corner, vec2 offset) {
	vec2 tex_coord_flipped = vec2(tex_coord.x, 1 - tex_coord.y);
	float index = texture(terrain_texture_sampler, tex_coord_flipped + 0.5 * pix * corner).r;
	index = floor(index * 256);
	float is_water = step(64, index);
	vec4 colour = texture(terrainsheet_texture_sampler, vec3(offset, index));
	return mix(colour, vec4(0.), is_water);
}

vec4 get_terrain_mix(vec2 tex_coords) {
	// Pixel size on map texture
	vec2 scaling = mod(tex_coord + 0.5 * pix, pix) / pix;

	vec2 offset = tex_coord / (16. * pix);

	vec4 colourlu = get_terrain(tex_coord, vec2(-1, -1), offset);
	vec4 colourld = get_terrain(tex_coord, vec2(-1, +1), offset);
	vec4 colourru = get_terrain(tex_coord, vec2(+1, -1), offset);
	vec4 colourrd = get_terrain(tex_coord, vec2(+1, +1), offset);

	vec4 colour_u = mix(colourlu, colourru, scaling.x);
	vec4 colour_d = mix(colourld, colourrd, scaling.x);
	return mix(colour_u, colour_d, 1-scaling.y);
}

void main() {
	vec4 terrain_background = texture(colormap_terrain, tex_coord);
	// vec4 water = get_water(tex_coord);
	vec4 terrain = get_terrain_mix(tex_coord);
	float is_terrain = terrain.w;
	frag_color = (terrain * 2. + terrain_background) / 3.;

	// frag_color = mix(water, frag_color, is_terrain);
}
