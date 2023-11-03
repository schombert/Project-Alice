in vec2 tex_coord;
in float type;
layout (location = 0) out vec4 frag_color;

layout (binding = 0) uniform sampler2D texture0;
layout (binding = 1) uniform sampler2D texture1;
layout (binding = 2) uniform sampler2D texture2;
layout (binding = 3) uniform sampler2D texture3;

void main() {
	float border_size = 0.05;
	vec3 inner_color = vec3(1.0, 1.0, 1.0);
	vec4 color_in = texture(texture0, tex_coord);
	if(type == 0.f) {
		color_in = texture(texture0, tex_coord);
	} else if(type == 1.f) {
		color_in = texture(texture1, tex_coord);
	} else if(type == 2.f) {
		color_in = texture(texture2, tex_coord);
	} else if(type == 3.f) {
		color_in = texture(texture3, tex_coord);
	}
	
	if(color_in.r > 0.505) {
		frag_color = vec4(inner_color, 1.0);
	} else if(color_in.r > 0.50) {
		frag_color = vec4(mix(inner_color, vec3(1.0, 1.0, 1.0) - inner_color,  1.0f - (color_in.r - 0.5f) * 200.0f), 1.0);
	} else {
		float t = max(0.0f, color_in.r * 16.0f - 7.0f);
		frag_color = vec4(vec3(1.0, 1.0, 1.0) - inner_color, t * t);
	}
	//} else {
	//	float sm_val = smoothstep(0.5 - border_size * 1.5, 0.5 - border_size, color_in.r);
	//	sm_val = min(sm_val, 0.5);
	//	frag_color = vec4(vec3(0.0, 0.0, 0.0) - inner_color, sm_val);
	//}
}
