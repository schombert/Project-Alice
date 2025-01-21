in vec2 tex_coord;
layout(location = 0) out vec4 frag_color;

uniform sampler2D real_provinces_texture_sampler;

// Only province id is used here
void main() {
    vec2 prov_id = texture(real_provinces_texture_sampler, tex_coord).xy;
	frag_color = vec4(prov_id, 0.f, 1.f);
}
