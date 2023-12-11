in vec2 tex_coord;
in vec2 map_coord;

layout (location = 0) out vec4 frag_color;

layout (binding = 0) uniform sampler2D provinces_texture_sampler;
layout (binding = 13) uniform sampler2D province_fow;
layout (binding = 14) uniform sampler2D river_body;
layout (binding = 15) uniform sampler2D river_movement;

layout (location = 11) uniform float gamma;
layout (location = 12) uniform float time;
vec4 gamma_correct(vec4 colour) {
	return vec4(pow(colour.rgb, vec3(1.f / gamma)), colour.a);
}

void main() {
	//vec4 out_color = vec4(tex_coord.x, tex_coord.y, 1.f, 1.f);
	vec4 out_color = texture(river_body, vec2(tex_coord.y, 0.5f + tex_coord.x / 2.f));
	vec4 movement = texture(river_movement, vec2(tex_coord.y, mod(tex_coord.x + time, 1.f)));
	out_color += movement;

	vec2 prov_id = texture(provinces_texture_sampler, map_coord).xy;
	float fow = texture(province_fow, prov_id).r;
	out_color.r *= fow;
	out_color.g *= fow;
	out_color.b *= fow;
	frag_color = gamma_correct(out_color);
}
