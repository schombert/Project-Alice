in vec2 tex_coord;
out vec4 frag_color;

uniform sampler2D provinces_texture_sampler;
uniform sampler2D terrain_texture_sampler;
uniform sampler2DArray terrainsheet_texture_sampler;
uniform sampler2D water_normal;
uniform sampler2D colormap_water;
uniform sampler2D colormap_terrain;
uniform sampler2D overlay;
uniform sampler2DArray province_color;
uniform sampler2D colormap_political;
uniform sampler2D province_highlight;
uniform sampler2D stripes_texture;
uniform sampler2D province_fow;
uniform sampler2D provinces_sea_mask;
uniform usampler2D diag_border_identifier;
uniform uint subroutines_index_2;
// location 0 : offset
// location 1 : zoom
// location 2 : screen_size
uniform vec2 map_size;
uniform vec2 screen_size;
uniform float time;
uniform float gamma;
vec4 gamma_correct(vec4 colour) {
	return vec4(pow(colour.rgb, vec3(1.f / gamma)), colour.a);
}

// sheet is composed of 64 files, in 4 cubes of 4 rows of 4 columns
// so each column has 8 tiles, and each row has 8 tiles too
float xx = 1 / map_size.x;
float yy = 1 / map_size.y;
vec2 pix = vec2(xx, yy);

vec2 get_corrected_coords(vec2 coords) {
	coords.y -= (1 - 1 / 1.3) * 3 / 5;
	coords.y *= 1.3;
	return coords;
}

// The water effect
vec4 get_water_terrain()
{
	vec2 prov_id = texture(provinces_texture_sampler, gl_FragCoord.xy / screen_size).xy;

	// Water effect taken from Vic2 fx/water/PixelShader_HoiWater_2_0
	const float WRAP = 0.8;
	const float WaveModOne = 3.0;
	const float WaveModTwo = 4.0;
	const float SpecValueOne = 8.0;
	const float SpecValueTwo = 2.0;
	const float vWaterTransparens = 1.0; //more transparance lets you see more of background
	const float vColorMapFactor = 1.0f; //how much colormap

	vec2 tex_coord = tex_coord;
	vec2 corrected_coord = get_corrected_coords(tex_coord);
	vec3 WorldColorColor = texture(colormap_water, corrected_coord).rgb;
	if(corrected_coord.y > 1)
		WorldColorColor = vec3(0.14, 0.23, 0.36);
	if(corrected_coord.y < 0)
		WorldColorColor = vec3(0.20, 0.35, 0.43);
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
	OutColor += (specular / SpecValueTwo);
	OutColor *= 1.5;

	OutColor *= texture(province_fow, prov_id).rgb;

	return vec4(OutColor, vWaterTransparens);
}

vec4 get_water_political() {
	vec3 water_background = vec3(0.21, 0.38, 0.45);
	vec3 color = water_background.rgb;

	// The "foldable map" overlay effect
	vec4 OverlayColor = texture(overlay, tex_coord * vec2(11., 11.*map_size.y/map_size.x));
	vec4 OutColor;
	OutColor.r = OverlayColor.r < .5 ? (2. * OverlayColor.r * color.r) : (1. - 2. * (1. - OverlayColor.r) * (1. - color.r));
	OutColor.g = OverlayColor.r < .5 ? (2. * OverlayColor.g * color.g) : (1. - 2. * (1. - OverlayColor.g) * (1. - color.g));
	OutColor.b = OverlayColor.b < .5 ? (2. * OverlayColor.b * color.b) : (1. - 2. * (1. - OverlayColor.b) * (1. - color.b));
	OutColor.a = OverlayColor.a;

	return OutColor;
}

// The terrain color from the current texture coordinate offset with one pixel in the "corner" direction
vec4 get_terrain(vec2 corner, vec2 offset) {
	vec2 prov_id = texture(provinces_texture_sampler, gl_FragCoord.xy / screen_size).xy;
	float index = texture(terrain_texture_sampler, floor(tex_coord * map_size + vec2(0.5, 0.5)) / map_size + 0.5 * pix * corner).r;
	index = floor(index * 256);

	float is_water = 0.f; //step(64, index);

	if (texture(provinces_sea_mask, prov_id).x > 0.0f || (prov_id.x == 0.f && prov_id.y == 0.f)) {
        is_water = 1.f;
    }

	vec4 colour = texture(terrainsheet_texture_sampler, vec3(offset, index));
	return mix(colour, vec4(0.), is_water);
}

vec4 get_terrain_mix() {
	// Pixel size on map texture
	vec2 scaling = fract(tex_coord * map_size + vec2(0.5, 0.5));

	vec2 offset = tex_coord / (16. * pix);

	vec4 colourlu = get_terrain(vec2(-1, -1), offset);
	vec4 colourld = get_terrain(vec2(-1, +1), offset);
	vec4 colourru = get_terrain(vec2(+1, -1), offset);
	vec4 colourrd = get_terrain(vec2(+1, +1), offset);

	// Mix together the terrains based on close they are to the current texture coordinate
	vec4 colour_u = mix(colourlu, colourru, scaling.x);
	vec4 colour_d = mix(colourld, colourrd, scaling.x);
	vec4 terrain = mix(colour_u, colour_d, scaling.y);

	// Mixes the terrains from "texturesheet.tga" with the "colormap.dds" background color.
	vec4 terrain_background = texture(colormap_terrain, get_corrected_coords(tex_coord));
	terrain.rgb = (terrain.rgb * 2. + terrain_background.rgb) / 3.;
	return terrain;
}

vec4 get_land_terrain() {
	return get_terrain_mix();
}

vec4 get_land_political_close() {
	vec4 terrain = get_terrain_mix();
	float is_land = terrain.a;

	// Make the terrain a gray scale color
	const vec3 GREYIFY = vec3( 0.212671, 0.715160, 0.072169 );
    float grey = dot( terrain.rgb, GREYIFY );
	terrain.rgb = vec3(grey);

	vec2 tex_coords = tex_coord;
	vec2 rounded_tex_coords = (floor(tex_coord * map_size) + vec2(0.5, 0.5)) / map_size;

	vec2 rel_coord = tex_coord * map_size - floor(tex_coord * map_size) - vec2(0.5);
	uint test = texture(diag_border_identifier, rounded_tex_coords).x;
	int shift = int(sign(rel_coord.x) + 2 * sign(rel_coord.y) + 3);

	rounded_tex_coords.y += ((int(test >> shift) & 1) != 0) && (abs(rel_coord.x) + abs(rel_coord.y) > 0.5) ? sign(rel_coord.y) / map_size.y : 0;

	vec2 prov_id = texture(provinces_texture_sampler, gl_FragCoord.xy / screen_size).xy;

	// The primary and secondary map mode province colors
	vec4 prov_color = texture(province_color, vec3(prov_id, 0.));
	vec4 stripe_color = texture(province_color, vec3(prov_id, 1.));

	vec2 stripe_coord = tex_coord * vec2(512., 512. * map_size.y / map_size.x);

	// Mix together the primary and secondary colors with the stripes
	float stripeFactor = texture(stripes_texture, stripe_coord).a;
	float prov_highlight = texture(province_highlight, prov_id).r * (abs(cos(time * 3.f)) + 1.f);
	vec3 political = clamp(mix(prov_color, stripe_color, stripeFactor) + vec4(prov_highlight), 0.0, 1.0).rgb;
	political *= texture(province_fow, prov_id).rgb;
	political = political - 0.7;

	// Mix together the terrain and map mode color
	terrain.rgb = mix(terrain.rgb, political, 0.3);
	terrain.rgb *= 1.5;
	//terrain.rgb += vec3((test * 255) == id);
	//terrain.r += ((abs(rel_coord.y) + abs(rel_coord.x)) > 0.5 ? 6 : 0) * 0.3;
	return terrain;
}

vec4 get_land_political_far() {
	vec4 terrain = get_terrain(vec2(0, 0), vec2(0));
	float is_land = terrain.a;
	vec2 prov_id = texture(provinces_texture_sampler, gl_FragCoord.xy / screen_size).xy;

	// The primary and secondary map mode province colors
	vec4 prov_color = texture(province_color, vec3(prov_id, 0.));
	vec4 stripe_color = texture(province_color, vec3(prov_id, 1.));

	vec2 stripe_coord = tex_coord * vec2(512., 512. * map_size.y / map_size.x);

	// Mix together the primary and secondary colors with the stripes
	float stripeFactor = texture(stripes_texture, stripe_coord).a;
	float prov_highlight = texture(province_highlight, prov_id).r * (abs(cos(time * 3.f)) + 1.f);
	vec3 political = clamp(mix(prov_color, stripe_color, stripeFactor) + vec4(prov_highlight), 0.0, 1.0).rgb;
	political = political - 0.7;

	// The "foldable map" overlay effect
	vec4 OverlayColor = texture(overlay, tex_coord * vec2(11., 11.*map_size.y/map_size.x));
	vec4 OutColor;
	OutColor.r = OverlayColor.r < .5 ? (2. * OverlayColor.r * political.r) : (1. - 2. * (1. - OverlayColor.r) * (1. - political.r));
	OutColor.g = OverlayColor.r < .5 ? (2. * OverlayColor.g * political.g) : (1. - 2. * (1. - OverlayColor.g) * (1. - political.g));
	OutColor.b = OverlayColor.b < .5 ? (2. * OverlayColor.b * political.b) : (1. - 2. * (1. - OverlayColor.b) * (1. - political.b));
	OutColor.a = OverlayColor.a;

	vec3 background = texture(colormap_political, get_corrected_coords(tex_coord)).rgb;
	OutColor.rgb = mix(background, OutColor.rgb, 0.3);

	OutColor.rgb *= 1.5;
	OutColor.a = is_land;

	return OutColor;
}

vec4 get_land() {
	switch(int(subroutines_index_2)) {
case 0: return get_land_terrain();
case 1: return get_land_political_close();
case 2: return get_land_political_far();
default: break;
	}
	return vec4(1.f);
}
vec4 get_water() {
	switch(int(subroutines_index_2)) {
case 0: return get_water_terrain();
case 1: return get_water_terrain();
case 2: return get_water_political();
default: break;
	}
	return vec4(1.f);
}

// The terrain map
// No province color is used here
void main() {
	vec4 terrain = get_land();
	vec4 water = get_water();
	frag_color.rgb = mix(water.rgb, terrain.rgb, terrain.a);
	frag_color.a = 1.f;
	frag_color = gamma_correct(frag_color);
}
