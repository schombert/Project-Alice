out vec4 frag_color;
in vec2 tex_coord;

uniform vec4 d_rect;
uniform float border_size;
uniform vec3 inner_color;
uniform vec4 subrect;
uniform float gamma;
uniform uvec2 subroutines_index;

uniform sampler2D texture_sampler;
uniform sampler2D secondary_texture_sampler;

vec4 gamma_correct(vec4 colour) {
	return vec4(pow(colour.rgb, vec3(1.f / gamma)), colour.a);
}

//layout(index = 0) subroutine(font_function_class)
vec4 border_filter(vec2 tc) {
	vec4 color_in = texture(texture_sampler, tc);
	if(color_in.r > 0.5) {
		return vec4(inner_color, 1.0);
	} else if(color_in.r > 0.5 - border_size) {
		float sm_val = smoothstep(0.5 - border_size / 2.0, 0.5, color_in.r);
		return vec4(mix(vec3(1.0, 1.0, 1.0) - inner_color, inner_color, sm_val), 1.0);
	} else {
		float sm_val = smoothstep(0.5 - border_size * 1.5, 0.5 - border_size, color_in.r);
		return vec4(vec3(1.0, 1.0, 1.0) - inner_color, sm_val);
	}
}
//layout(index = 1) subroutine(font_function_class)
vec4 color_filter(vec2 tc) {
	vec4 color_in = texture(texture_sampler, tc);
	float sm_val = smoothstep(0.5 - border_size / 2.0, 0.5 + border_size / 2.0, color_in.r);
	return vec4(inner_color, sm_val);
}
//layout(index = 2) subroutine(font_function_class)
vec4 no_filter(vec2 tc) {
	return texture(texture_sampler, tc);
}
//layout(index = 5) subroutine(font_function_class)
vec4 subsprite(vec2 tc) {
	return texture(texture_sampler, vec2(tc.x * inner_color.y + inner_color.x, tc.y));
}
//layout(index = 6) subroutine(font_function_class)
vec4 use_mask(vec2 tc) {
	return vec4(texture(texture_sampler, tc).rgb, texture(secondary_texture_sampler, tc).a);
}
//layout(index = 7) subroutine(font_function_class)
vec4 progress_bar(vec2 tc) {
	return mix( texture(texture_sampler, tc), texture(secondary_texture_sampler, tc), step(border_size, tc.x));
}
//layout(index = 8) subroutine(font_function_class)
vec4 frame_stretch(vec2 tc) {
	float realx = tc.x * d_rect.z;
	float realy = tc.y * d_rect.w;
	vec2 tsize = textureSize(texture_sampler, 0);
	float xout = 0.0;
	float yout = 0.0;
	if(realx <= border_size)
		xout = realx / tsize.x;
	else if(realx >= (d_rect.z - border_size))
		xout = (1.0 - border_size / tsize.x) + (border_size - (d_rect.z - realx))  / tsize.x;
	else
		xout = border_size / tsize.x + (1.0 - 2.0 * border_size / tsize.x) * (realx - border_size) / (d_rect.z * 2.0 * border_size);
	if(realy <= border_size)
		yout = realy / tsize.y;
	else if(realy >= (d_rect.w - border_size))
		yout = (1.0 - border_size / tsize.y) + (border_size - (d_rect.w - realy))  / tsize.y;
	else
		yout = border_size / tsize.y + (1.0 - 2.0 * border_size / tsize.y) * (realy - border_size) / (d_rect.w * 2.0 * border_size);
	return texture(texture_sampler, vec2(xout, yout));
}
//layout(index = 9) subroutine(font_function_class)
vec4 piechart(vec2 tc) {
	if(((tc.x - 0.5) * (tc.x - 0.5) + (tc.y - 0.5) * (tc.y - 0.5)) > 0.25)
		return vec4(0.0, 0.0, 0.0, 0.0);
	else
		return texture(texture_sampler, vec2((atan((tc.y - 0.5), (tc.x - 0.5) ) + M_PI) / (2.0 * M_PI), 0.5));
}
//layout(index = 10) subroutine(font_function_class)
vec4 barchart(vec2 tc) {
	vec4 color_in = texture(texture_sampler, vec2(tc.x, 0.5));
	return vec4(color_in.rgb, step(1.0 - color_in.a, tc.y));
}
//layout(index = 11) subroutine(font_function_class)
vec4 linegraph(vec2 tc) {
	return mix(vec4(inner_color.r, 0.0, inner_color.b, 1.0), vec4(0.0, inner_color.g, inner_color.b, 1.0), tc.y);
}
//layout(index = 15) subroutine(font_function_class)
vec4 subsprite_b(vec2 tc) {
	return vec4(inner_color, texture(texture_sampler, vec2(tc.x * subrect.y + subrect.x, tc.y * subrect.a + subrect.z)).a);
}
//layout(index = 17) subroutine(font_function_class)
vec4 linegraph_color(vec2 tc) {
	return vec4(inner_color, 1.0);
}
//layout(index = 21) subroutine(font_function_class)
vec4 subsprite_c(vec2 tc) {
	return texture(texture_sampler, vec2(tc.x * subrect.y + subrect.x, tc.y * subrect.a + subrect.z));
}

//layout(index = 18) subroutine(font_function_class)
vec4 transparent_color(vec2 tc) {
	return vec4(inner_color, 0.5);
}
//layout(index = 19) subroutine(font_function_class)
vec4 solid_color(vec2 tc) {
	return vec4(inner_color, 1.0);
}
//layout(index = 20) subroutine(font_function_class)
vec4 alpha_color(vec2 tc) {
	return vec4(inner_color, border_size);
}

//layout(index = 3) subroutine(color_function_class)
vec4 disabled_color(vec4 color_in) {
	float amount = (color_in.r + color_in.g + color_in.b) / 4.0;
	return vec4(amount, amount, amount, color_in.a);
}
//layout(index = 4) subroutine(color_function_class)
vec4 enabled_color(vec4 color_in) {
	return color_in;
}
//layout(index = 12) subroutine(color_function_class)
vec4 tint_color(vec4 color_in) {
	return vec4(color_in.r * inner_color.r, color_in.g * inner_color.g, color_in.b * inner_color.b, color_in.a);
}
//layout(index = 13) subroutine(color_function_class)
vec4 interactable_color(vec4 color_in) {
	return vec4(color_in.r + 0.1, color_in.g + 0.1, color_in.b + 0.1, color_in.a);
}
//layout(index = 14) subroutine(color_function_class)
vec4 interactable_disabled_color(vec4 color_in) {
	float amount = (color_in.r + color_in.g + color_in.b) / 4.0;
	return vec4(amount + 0.1, amount + 0.1, amount + 0.1, color_in.a);
}
//layout(index = 16) subroutine(color_function_class)
vec4 alt_tint_color(vec4 color_in) {
	return vec4(color_in.r * subrect.r, color_in.g * subrect.g, color_in.b * subrect.b, color_in.a);
}

vec4 font_function(vec2 tc) {
	switch(int(subroutines_index.y)) {
case 0: return border_filter(tc);
case 1: return color_filter(tc);
case 2: return no_filter(tc);
case 5: return subsprite(tc);
case 6: return use_mask(tc);
case 7: return progress_bar(tc);
case 8: return frame_stretch(tc);
case 9: return piechart(tc);
case 10: return barchart(tc);
case 11: return linegraph(tc);
case 15: return subsprite_b(tc);
case 17: return linegraph_color(tc);
case 18: return transparent_color(tc);
case 19: return solid_color(tc);
case 20: return alpha_color(tc);
case 21: return subsprite_c(tc);
default: break;
	}
	return vec4(0.f, 0.f, 1.f, 1.f);
}
vec4 coloring_function(vec4 tc) {
	switch(int(subroutines_index.x)) {
case 3: return disabled_color(tc);
case 4: return enabled_color(tc);
case 12: return tint_color(tc);
case 13: return interactable_color(tc);
case 14: return interactable_disabled_color(tc);
case 16: return alt_tint_color(tc);
default: break;
	}
	return tc;
}

void main() {
	frag_color = gamma_correct(coloring_function(font_function(tex_coord)));
}
