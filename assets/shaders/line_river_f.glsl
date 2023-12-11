in vec2 tex_coord;
in vec2 map_coord;
in float tex_type;

layout (location = 0) out vec4 frag_color;

layout (binding = 0) uniform sampler2D provinces_texture_sampler;
layout (binding = 5) uniform sampler2D colormap_water;
layout (binding = 13) uniform sampler2D province_fow;
layout (binding = 14) uniform sampler2D river_body;
layout (binding = 15) uniform sampler2D river_movement;

layout (location = 11) uniform float gamma;
layout (location = 12) uniform float time;
layout (location = 13) uniform float pass;
vec4 gamma_correct(vec4 colour) {
	return vec4(pow(colour.rgb, vec3(1.f / gamma)), colour.a);
}

void main() {
	vec4 out_color = texture(river_body, vec2(tex_coord.y, (0.5f * (1.f - tex_type)) + tex_coord.x / 2.f));
	vec4 water_color = texture(colormap_water, map_coord);
	vec4 movement = texture(river_movement, vec2(tex_coord.y, mod(tex_coord.x - time, 1.f)));
	//out_color.rgb = (water_color.rgb + 0.275f) * movement.a + out_color.rgb * (1.f - movement.a);
	out_color.rgb = mix(out_color.rgb, water_color.rgb, 0.5f);
	out_color.rgb += movement.rgb;

	out_color.a += 0.5f;
	out_color.a *= out_color.a * out_color.a * out_color.a;

	if(pass == 0.f) {
		out_color = vec4(0.f,0.f,0.f,1.f);//vec4(water_color.r, water_color.g, water_color.b, 1.f);
	}

	//out_color = vec4(tex_coord.x, tex_coord.y, 1.f, 1.f);

	vec2 prov_id = texture(provinces_texture_sampler, map_coord).xy;
	out_color.rgb *= texture(province_fow, prov_id).r;
	frag_color = gamma_correct(out_color);
}
